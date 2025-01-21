#ifndef HMAN_HPP
#define HMAN_HPP

#include "tcp_client.hpp"
#include "strANSIseq.hpp"

#include <chrono>
#include <iostream>
#include <vector>

#define HMAN_PORT 5000

class Hman : virtual public ESC::CLI
{
    public:
    enum Mode
    {
        position = 0,
        current = 1
    };

    typedef struct
    {
        int32_t pos[3];
        int32_t t;
    } Pos;

    /**
     * @brief Construct a new Hman object
     *
     * @param nb_mot number of motors
     **/
    Hman(int nb_mot = 2) : m_nb_mot(nb_mot) { m_pkgSize = 2 + nb_mot * 8; };

    ~Hman();


  void
    connect(const char *address)
    {
        m_client.open_connection(Communication::Client::TCP, address, HMAN_PORT,
                                 -1);
    };

    /**
     * @brief Sets the mode of the Hman.
     *
     * @param mode Can be Hman::position or Hman::current.
     */
    void set_mode(Hman::Mode mode);

    /**
     * @brief Sets the values of the motors.
     *
     * @param val The values of the motors.
     * @param n The index of motor.
     */
    void set_values(int32_t *val, int index);

    /**
     * @brief Sets the cartesian position of the Hman.
     *
     * @param posx The x position.
     * @param posy The y position.
     * @param posz The z position.
     */
    void set_cartesian_pos(int32_t posx, int32_t posy, int32_t posz = 0);

    /**
     * @brief Set the articular position of the Hman.
     *
     * @param pos1 The position of the first motor.
     * @param pos2 The position of the second motor.
     * @param pos3 The position of the third motor.
     */
    void set_articular_pos(int32_t pos1, int32_t pos2, int32_t pos3 = 0);

    /**
     * @brief Sets the current of the motors.
     *
     * @param cur1 The current of the first motor.
     * @param cur2 The current of the second motor.
     * @param cur3 The current of the third motor.
     */
    void set_motors_current(int32_t cur1, int32_t cur2, int32_t cur3 = 0);

    /**
     * @brief Turns off the current of the motors.
     */
    void turn_off_current();

    /**
     * @brief Gets the position of the Hman's encoders.
     *
     * @param pos The position of the Hman.
     */
    void get_pos(Pos &pos);

    void
    get_pos(Pos &pos)
    {
        m_cmd[0] = 'P';
        m_cmd[1] = m_nb_mot;
        m_client.writeS(m_cmd, m_pkgSize);
        m_client.readS(m_buff, m_nb_mot * 4);
        for(int i = 0; i < m_nb_mot; i++) pos.pos[i] = ((int32_t *)(m_buff))[i];
    }

    /**
     * @brief Records the hman's position and store them in a vector.
     * 
     * @param time The amount of time to record.
     * @param listPos the vector where the positions are stored.
     */
    void record_path(int32_t time, std::vector<Pos> &listPos);

    /**
     * @brief Plays back the recorded position of the hman.
     * 
     * @param listPos The vector containing the positions.
     */
    void play_path(std::vector<Pos> &listPos);

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
            std::cout << p.pos[0] << ":" << p.pos[1] << ":" << p.t << std::endl;
        }
    }

    private:
    Communication::Client* m_client;
    int m_pkgSize;
    int m_nb_mot;
    int m_mode = -1;
    uint8_t m_cmd[255];
    uint8_t m_buff[255];
    bool m_verbose=false;
};

#endif
