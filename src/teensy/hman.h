#include "motor.h"

class Hman
{
  public:
  Hman(uint8_t nb_mot=2)
  {
    for (int i = 0; i < NB_MOT; i++)
    m_motors[i] = new Motor( 22 - 3 * i, 22 - 3 * i - 1,       2 + 3 * i,    2 + 3 * i + 1,   2 + 3 * i + 2);
  };

  void set_cartesian_pos(int32_t posx, int32_t posy, int32_t posz=0);
  void set_articular_pos(uint8_t index, int32_t pos);
  void set_motor_current(uint8_t index, int32_t pos);

private:
  Motor* m_motors[NB_MOT];
  
};
