#include "motor.h"

class Hman
{
  public:
    enum Mode {position = 0, current = 2};

    Hman(uint8_t nb_mot = (uint8_t)2)
    {
      for (i = 0; i < NB_MOT; i++)
        m_motors[i] = new Motor( 22 - 3 * i, 22 - 3 * i - 1,       2 + 3 * i,    2 + 3 * i + 1,   2 + 3 * i + 2);
      
    };

    void set_cartesian_pos(int32_t posx, int32_t posy, int32_t posz = 0)
    {
      m_motors[0]->set_pos(-posx + posy);// -(js.joystickValue(0) - (1 - 2 * (i % 2))*js.joystickValue(1)) / 15;
      m_motors[1]->set_pos(-posx - posy);
    }

    void set_articular_pos(int32_t* pos, uint8_t n)
    {
      for (i = 0; i < n && i < NB_MOT; i++)
        m_motors[i]->set_pos(pos[i]);
    };

    void set_articular_pos(uint8_t index, int32_t pos)
    {
      if (index < NB_MOT)
        m_motors[index]->set_pos(pos);
    };

    void set_motor_current(int32_t* cur, uint8_t n)
    {
      for (i = 0; i < n && i < NB_MOT; i++)
        m_motors[i]->set_current(cur[i]);
    };

    static void test()
    {};

    void update()
    {
      for (i = 0; i < NB_MOT; i++)
        m_motors[i]->update();
    }

    int32_t* get_pos()
    {
      for (i = 0; i < NB_MOT; i++)
        m_pos_motor[i] = m_motors[i]->get_pos();
      return m_pos_motor;
    };

    int8_t& mode()
    {
      return m_mode;
    }


  private:
    int i;
    int8_t m_mode;
    Motor* m_motors[NB_MOT];
    int32_t m_pos_motor[NB_MOT];
    // Create an IntervalTimer object
    


};
