#include "motor.h"

#define NB_MOT 3
#define NB_END_SWITCH 2
#define TRAJ_MAX_SIZE 10000

class Hman {
public:

  Hman(uint8_t nb_mot = (uint8_t)2) {
    m_running = true;
    m_conv_coef = 2 * 16 * 3.14169 / 10024;  //mm by encoder count
    for (int i = 0; i < NB_END_SWITCH; i++) {
      pinMode(m_pinEndSwitch[i], INPUT_PULLUP);
      m_valEndSwitch[i] = digitalRead(m_pinEndSwitch[i]);
    }

    for (i = 0; i < nb_mot; i++)
      m_motors[i] = new Motor(23 - 2 * i - 1, 23 - 2 * i, 2 + 3 * i, 2 + 3 * i + 1, 2 + 3 * i + 2);
    for (i = 0; i < 2; i++)
      m_motors[i]->update(m_valEndSwitch, NB_END_SWITCH);
  };

  void set_cartesian_pos(double posx, double posy, double posz = 0)  //in mm
  {
    if (m_mode != Motor::position)
      this->set_mode(Motor::position);
    m_motors[0]->set_pos((-posx + posy) / m_conv_coef);
    m_motors[1]->set_pos((-posx - posy) / m_conv_coef);
    //      Serial.print((-posx + posy)/m_conv_coef);
    //      Serial.print("\t");
    //      Serial.println((-posx - posy)/m_conv_coef);
  }

  void set_articular_pos(int32_t* pos, uint8_t n)  //in encoder count
  {
    if (m_mode != Motor::position)
      this->set_mode(Motor::position);
    for (i = 0; i < n && i < NB_MOT; i++)
      m_motors[i]->set_pos(pos[i]);
    Serial.print(pos[0]);
    Serial.print("\t");
           Serial.println(pos[1]);
  };

  void set_articular_pos(uint8_t index, int32_t pos)  //in encoder count
  {
    if (m_mode != Motor::position)
      this->set_mode(Motor::position);
    if (index < NB_MOT)
      m_motors[index]->set_pos(pos);
  };

  void set_motor_current(int32_t* cur, uint8_t n)  //in pwm
  {
    if (m_mode != Motor::current)
      this->set_mode(Motor::current);
    for (i = 0; i < n && i < NB_MOT; i++)
      m_motors[i]->set_current(cur[i]);
  };

  void set_current(int32_t current)  //in pwm
  {
    if (m_mode != Motor::current)
      this->set_mode(Motor::current);
    for (i = 0; i < NB_MOT; i++)
      m_motors[i]->set_current(current);
  };

  static void test(){};

  void update() {
    if (m_running) {
      for (int i = 0; i < NB_END_SWITCH; i++)
        m_valEndSwitch[i] = digitalRead(m_pinEndSwitch[i]);
      for (i = 0; i < NB_MOT; i++)
        m_motors[i]->update(m_valEndSwitch, NB_END_SWITCH);
    }
  }

  int32_t* get_pos() {
    for (i = 0; i < NB_MOT; i++)
      m_pos_motor[i] = m_motors[i]->get_pos();
    return m_pos_motor;
  };


  void get_cart_pos(long* px, long* py)  //milimeter
  {
    long m1 = m_motors[0]->get_pos();
    long m2 = m_motors[1]->get_pos();
    *px = -(m1 + m2) / 2 * m_conv_coef;
    *py = (m1 - m2) / 2 * m_conv_coef;
  }

  Motor::Mode& mode() {
    return m_mode;
  }

  void set_mode(Motor::Mode mode) {
    m_mode = mode;
    for (i = 0; i < NB_MOT; i++)
      m_motors[i]->m_mode = mode;
  }

  uint8_t home() {
    return 1;
  }


  void goto_rel_cart_pos(int32_t Dx, int32_t Dy, double vmax, double amax, double dt = 0.005) {
    double D = sqrt(Dx * Dx + Dy * Dy);
    double coef_x = Dx / D, coef_y = Dy / D;

    vmax = (D > vmax * vmax / amax) ? vmax : sqrt(D * amax);  //check if triangle or trapeze

    long dt_micro = dt * 1000000;
    long t1 = 0;
    long t2 = 0;

    double curr_d = 0;
    double d1 = vmax * vmax / 2 / amax;
    double d2 = D - d1;

    double curr_v = 0;

    long posx_0, posy_0;
    this->get_cart_pos(&posx_0, &posy_0);

    //phase1 a=amax , v from 0 to vmaxp (vmaxp=vmax if possible)
    while (curr_d < d1) {
      curr_v += amax * dt;
      curr_d += curr_v * dt;
      set_cartesian_pos(posx_0 + curr_d * coef_x, posy_0 + curr_d * coef_y);
      delayMicroseconds(dt_micro);
      t1 += dt_micro;
    }

    //phase2 a=0    , v=vmaxp
    while (curr_d < d2) {
      curr_d += curr_v * dt;
      set_cartesian_pos(posx_0 + curr_d * coef_x, posy_0 + curr_d * coef_y);
      delayMicroseconds(dt_micro);
    }

    //phase1 a=-amax, v from vmaxp to 0
    while (curr_d < D && t2 < t1 + dt_micro) {
      curr_v -= amax * dt;
      curr_v = (curr_v < 0.01) ? 0.01 : curr_v;
      curr_d += curr_v * dt;
      set_cartesian_pos(posx_0 + curr_d * coef_x, posy_0 + curr_d * coef_y);
      delayMicroseconds(dt_micro);
      t2 += dt_micro;
    }
  }

  void trajectory() {
    if (m_following_traj) {
      set_cartesian_pos(m_traj[0][m_traj_ind], m_traj[1][m_traj_ind]);
      m_traj_ind++;
      if (m_traj_ind == m_traj_size)  //TODO ensure not too far away
        m_traj_ind = 0;
    }
  }

  void clear_traj() {
    m_traj_size = 0;
  }

  void start_traj() {
    m_traj_ind = 0;
    m_following_traj = true;
  }

  void stop_traj() {
    m_following_traj = false;
  }

  int add_delta_traj(int32_t Dx, int32_t Dy, double vmax, double amax) {
    double D = sqrt(Dx * Dx + Dy * Dy);
    double coef_x = Dx / D, coef_y = Dy / D;

    vmax = (D > vmax * vmax / amax) ? vmax : sqrt(D * amax);  //check if triangle or trapeze

    long dt_micro = m_traj_dt_micro;
    double dt = m_traj_dt_micro * (double)0.000001;
    long t1 = 0;
    long t2 = 0;

    double curr_d = 0;
    double d1 = vmax * vmax / 2 / amax;
    double d2 = D - d1;

    double curr_v = 0;

    double posx_0, posy_0;
    if (m_traj_size == 0) {
      posx_0 = 0;
      posy_0 = 0;
    } else {
      posx_0 = m_traj[0][m_traj_size - 1];
      posy_0 = m_traj[1][m_traj_size - 1];
    }

    //phase1 a=amax , v from 0 to vmaxp (vmaxp=vmax if possible)
    while (curr_d < d1) {
      curr_v += amax * dt;
      curr_d += curr_v * dt;
      if (m_traj_size == TRAJ_MAX_SIZE)
        return 1;
      m_traj[0][m_traj_size] = posx_0 + curr_d * coef_x;
      m_traj[1][m_traj_size] = posy_0 + curr_d * coef_y;
      t1 += dt_micro;
      if (m_traj_size % 10 == 0) {
        //          Serial.print(curr_v);
        //          Serial.print("\t");
        //          Serial.print(m_traj[0][m_traj_size]);
        //          Serial.print("\t");
        //          Serial.println(m_traj[1][m_traj_size]);
      }
      m_traj_size++;
    }

    //phase2 a=0    , v=vmaxp
    while (curr_d < d2) {
      curr_d += curr_v * dt;
      if (m_traj_size == TRAJ_MAX_SIZE)
        return 1;
      m_traj[0][m_traj_size] = posx_0 + curr_d * coef_x;
      m_traj[1][m_traj_size] = posy_0 + curr_d * coef_y;
      if (m_traj_size % 10 == 0) {
        //          Serial.print(curr_v);
        //          Serial.print("\t");
        //          Serial.print(m_traj[0][m_traj_size]);
        //          Serial.print("\t");
        //          Serial.println(m_traj[1][m_traj_size]);
      }
      m_traj_size++;
    }

    //phase1 a=-amax, v from vmaxp to 0
    while (curr_d < D && t2 < t1 + dt_micro) {
      curr_v -= amax * dt;
      curr_v = (curr_v < 0.01) ? 0.01 : curr_v;
      curr_d += curr_v * dt;
      if (m_traj_size == TRAJ_MAX_SIZE)
        return 1;
      m_traj[0][m_traj_size] = posx_0 + curr_d * coef_x;
      m_traj[1][m_traj_size] = posy_0 + curr_d * coef_y;
      t2 += dt_micro;
      if (m_traj_size % 10 == 0) {
        //          Serial.print(curr_v);
        //          Serial.print("\t");
        //          Serial.print(m_traj[0][m_traj_size]);
        //          Serial.print("\t");
        //          Serial.println(m_traj[1][m_traj_size]);
      }
      m_traj_size++;
    }
    return 0;
  }


  //private:
  int i;
  Motor::Mode m_mode;
  Motor* m_motors[NB_MOT];
  int32_t m_pos_motor[NB_MOT];
  // Create an IntervalTimer object
  int m_pinEndSwitch[NB_END_SWITCH] = { 25, 26 };
  int8_t m_valEndSwitch[NB_END_SWITCH];
  bool m_running;
  float m_conv_coef;
  float m_traj[2][TRAJ_MAX_SIZE] = { { 0 }, { 0 } };
  long m_traj_size = 0;
  long m_traj_ind = 0;
  long m_traj_dt_micro = 1000;
  bool m_following_traj = false;
};
