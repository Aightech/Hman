#include "hman.hpp"
#include "strANSIseq.hpp"

Hman::Hman(int nb_mot, bool verbose)
    : ESC::CLI(verbose, "Hman"), m_nb_mot(nb_mot), m_verbose(verbose)
{
    logln("Initialised.", true);
    m_pkgSize = 2 + nb_mot * 8;
};

Hman::~Hman()
{
    logln("Disconnected.", true);
    m_client->get_stat('d', 18);
    m_client->close_connection();
};

void Hman::connect(const char *address)
{
    logln("Connected.", true);
    m_client = new Communication::TCP(m_verbose);
    m_client->open_connection(address, HMAN_PORT, 1);
}


void Hman::set_mode(Hman::Mode mode)
{
    m_mode = mode;
    m_cmd[0] = 'M';
    *(uint32_t *)(m_cmd + 2) = mode;
    m_client->writeS(m_cmd, m_pkgSize);
}

void Hman::set_values(int32_t *val, int n)
{
    m_cmd[0] = 'V';
    m_cmd[1] = m_nb_mot;
    for(int i = 0; i < n; i++) { ((int32_t *)(m_cmd + 2))[i] = val[i]; }
    m_client->writeS(m_cmd, m_pkgSize);
}

void Hman::set_cartesian_pos(int32_t posx, int32_t posy, int32_t posz)
{
    if(m_mode != Hman::position)
        this->set_mode(Hman::position);

    int32_t pos[3] = {-posx + posy, -posx - posy, posz};
    this->set_values(pos, 3);
}

void Hman::set_articular_pos(int32_t pos1, int32_t pos2, int32_t pos3)
{
    if(m_mode != Hman::position)
        this->set_mode(Hman::position);
    int32_t pos[3] = {pos1, pos2, pos3};
    this->set_values(pos, 3);
}

void Hman::set_motors_current(int32_t cur1, int32_t cur2, int32_t cur3)
{
    if(m_mode != Hman::current)
        this->set_mode(Hman::current);
    int32_t pos[3] = {cur1, cur2, cur3};
    this->set_values(pos, 3);
}

void Hman::turn_off_current()
{
    if(m_mode != Hman::current)
        this->set_mode(Hman::current);
    int32_t cur[m_nb_mot];
    for(int i = 0; i < m_nb_mot; i++) cur[i] = 0;
    this->set_values(cur, m_nb_mot);
}

void Hman::get_pos(Pos &pos)
{
    m_cmd[0] = 'P';
    m_cmd[1] = m_nb_mot;
    m_client->writeS(m_cmd, m_pkgSize);
    m_client->readS(m_buff, m_nb_mot * 4 + 2, true);
    for(int i = 0; i < m_nb_mot; i++) pos.pos[i] = ((int32_t *)(m_buff))[i];
}

void Hman::get_acc(float arr[3])
{
    m_cmd[0] = 'X';
    m_cmd[1] = m_nb_mot;
    m_client->writeS(m_cmd, m_pkgSize);
    m_client->readS((uint8_t *)arr, 3 * 4);
}

void Hman::record_path(int32_t time, std::vector<Pos> &listPos)
{
    this->turn_off_current();
    usleep(10000);
    using namespace std::chrono;
    int i = 0;
    steady_clock::time_point begin = steady_clock::now();
    steady_clock::time_point now = steady_clock::now();
    while(duration_cast<microseconds>(now - begin).count() < time)
    {

        if(i >= listPos.size())
            listPos.push_back(Hman::Pos());

        this->get_pos(listPos[i]);
        listPos[i].t = duration_cast<microseconds>(now - begin).count();
        std::cout << listPos[i].pos[0] << ":" << listPos[i].pos[1] << ":"
                  << listPos[i].t << std::endl;
        i++;
        usleep(1000);
        now = steady_clock::now();
    }
    std::cout << i << std::endl;
}

void Hman::play_path(std::vector<Pos> &listPos)
{
    this->set_mode(Hman::position);
    usleep(10000);
    using namespace std::chrono;
    typedef std::chrono::steady_clock sc;
    typedef microseconds us;
    sc::time_point begin = sc::now();
    for(auto p : listPos)
    {
        while(duration_cast<us>(sc::now() - begin).count() < p.t) {};
        this->set_values(p.pos, m_nb_mot);
        std::cout << p.pos[0] << ":" << p.pos[1] << ":" << p.t << std::endl;
    }
}

void Hman::start_trajectory()
{
    m_cmd[0] = 'T';
    m_cmd[1] = 1;
    m_client->writeS(m_cmd, m_pkgSize);
}

void Hman::stop_trajectory()
{
    m_cmd[0] = 'T';
    m_cmd[1] = 0;
    m_client->writeS(m_cmd, m_pkgSize);
}

void Hman::add_to_trajectory(int32_t dx, int32_t dy, int32_t vmax, int32_t amax)
{
    m_cmd[0] = 'T';
    m_cmd[1] = 2;
    *(int32_t *)(m_cmd + 2) = dx;
    *(int32_t *)(m_cmd + 6) = dy;
    *(int32_t *)(m_cmd + 10) = vmax;
    *(int32_t *)(m_cmd + 14) = amax;
    m_client->writeS(m_cmd, m_pkgSize);
}
