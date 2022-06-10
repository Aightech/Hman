#ifndef HMAN_HPP
#define HMAN_HPP

#include "com_client.hpp"
#include <chrono>
#include <vector>
#include <iostream>

#define HMAN_PORT 5000

class Hman
{

    public:
    enum Mode
    {
        position = 0,
        current = 2
    };

    typedef struct
    {
        int32_t pos[3];
        int32_t t;
    } Pos;

    Hman(int nb_mot = 2) : m_nb_mot(nb_mot) { m_pkgSize = 2 + nb_mot * 8; };

    ~Hman() { m_client.close_connection(); };

    void
    connect(const char *address)
    {
      m_client.open_connection(address, HMAN_PORT,-1);
    };

    void
    set_mode(Hman::Mode mode)
    {
        m_mode = mode;
        m_cmd[0] = 'M';
        *(uint32_t *)(m_cmd + 2) = mode;
        m_client.writeS(m_cmd, m_pkgSize);
    };

    void
    set_values(int32_t *val, int n)
    {
        m_cmd[0] = 'V';
        m_cmd[1] = m_nb_mot;
        for(int i = 0; i < n; i++) { ((int32_t *)(m_cmd + 2))[i] = val[i]; }
        m_client.writeS(m_cmd, m_pkgSize);
    };

    void
    set_cartesian_pos(int32_t posx, int32_t posy, int32_t posz = 0)
    {
        if(m_mode != Hman::position)
            this->set_mode(Hman::position);

        int32_t pos[3] = {-posx + posy, -posx - posy, posz};
        this->set_values(pos, 3);
    };

    void
    set_articular_pos(int32_t pos1, int32_t pos2, int32_t pos3 = 0)
    {
        if(m_mode != Hman::position)
            this->set_mode(Hman::position);
        int32_t pos[3] = {pos1, pos2, pos3};
        this->set_values(pos, 3);
    };

    void
    set_motors_current(int32_t cur1, int32_t cur2, int32_t cur3 = 0)
    {
        if(m_mode != Hman::current)
            this->set_mode(Hman::current);
        int32_t pos[3] = {cur1, cur2, cur3};
        this->set_values(pos, 3);
    };

    void
    turn_off_current()
    {
        if(m_mode != Hman::current)
            this->set_mode(Hman::current);
        int32_t cur[m_nb_mot];
        for(int i = 0; i < m_nb_mot; i++) cur[i] = 0;
        this->set_values(cur, m_nb_mot);
    }

    void
    get_pos(Pos &pos)
    {
        m_cmd[0] = 'P';
        m_cmd[1] = m_nb_mot;
        m_client.writeS(m_cmd, m_pkgSize);
        m_client.readS(m_buff, m_nb_mot*4);
        for(int i = 0; i < m_nb_mot; i++)
            pos.pos[i] = ((int32_t *)(m_buff))[i];
    }

    void
    record_path(int32_t time, std::vector<Pos> &listPos)
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
	    std::cout << listPos[i].pos[0] << ":"<< listPos[i].pos[1] << ":"<< listPos[i].t  << std::endl;
            i++;
	    usleep(1000);
            now = steady_clock::now();
        }
	std::cout << i << std::endl;
    }

    void
    play_path(std::vector<Pos> &listPos)
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
	    std::cout << p.pos[0] << ":"<< p.pos[1] << ":"<< p.t  << std::endl;
        }
    }

    private:
    Communication::Client m_client;
    int m_pkgSize;
    int m_nb_mot;
    int m_mode = -1;
    uint8_t m_cmd[255];
    uint8_t m_buff[255];
};

#endif
