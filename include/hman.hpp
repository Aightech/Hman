#ifndef HMAN_HPP
#define HMAN_HPP

#include "com_client.hpp"
#define int64 iint64 //conflicting typedef of int64
#include "multi_camera.hpp"
#undef int64
#include "ni-daqmx.hpp"

#include <chrono>
#include <iostream>
#include <vector>

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

    Hman(int nb_mot = 2, bool verbose=false);

    ~Hman();

    /**
     * @brief Starts the communication with the Hman.
     *
     * @param address The IP address of the Hman.
     */
    void connect(const char *address);

    void start_cameras();

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

    /**
     * @brief Gets the acceleration and position of the Hman's encoders.
     * 
     * @param arr The acceleration of the Hman.
     * @param pos The articular position of the Hman.
     */
    void get_acc(float arr[3]);

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

    /**
     * @brief Starts the hman trajectory loop
     */
    void start_trajectory();

    /**
     * @brief Stops the hman trajectory loop
     */
    void stop_trajectory();

    /**
     * @brief Adds a control point to the trajectory loop
     *
     * @param dx The distance to the next control point in x direction
     * @param dy The distance to the next control point in y direction
     * @param vmax The maximum velocity
     * @param amax The maximun acceleration
     */
    void add_to_trajectory(int32_t dx, int32_t dy, int32_t vmax, int32_t amax);

    MultiCam cameras;
    NI::ATI::FT6_sensor FTsensor;

    private:
    Communication::Client m_client;
    int m_pkgSize;
    int m_nb_mot;
    int m_mode = -1;
    uint8_t m_cmd[255];
    uint8_t m_buff[255];
    bool m_verbose=false;

    
};

#endif
