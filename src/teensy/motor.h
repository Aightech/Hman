#include <Encoder.h>
#include <QuickPID.h>




float Kp = 5, Ki = 0 , Kd = 0;

class Motor
{
  public:
    enum Mode {position = 0, current = 1};


    Motor(int encPin1, int encPin2, int drvPinPWM_, int drvPinEn_, int drvPinDir_): m_encoder(Encoder(encPin1, encPin2)),
      m_pid(QuickPID(&m_position, &m_output, &m_setPoint))
    {
      m_id = nb_mot;
      nb_mot++;
      m_pid.SetTunings(Kp, Ki, Kd);
      m_pid.SetMode(m_pid.Control::automatic);
      m_pid.SetOutputLimits(-3686, 3686);
      analogWriteResolution(12);
      m_drvPinPWM = drvPinPWM_;
      m_drvPinEn = drvPinEn_;
      m_drvPinDir = drvPinDir_;
      pinMode(m_drvPinPWM, OUTPUT);
      analogWrite(m_drvPinPWM, 410);
      pinMode(m_drvPinDir, OUTPUT);
      digitalWrite(m_drvPinDir, LOW);
      pinMode(m_drvPinEn, OUTPUT);
      digitalWrite(m_drvPinEn, HIGH);
      //SetAntiWindupMode();

    }

    void update(int8_t* valEndSwitch, int n)
    {
      double o=0;
      double v = m_position;
      if(m_mode==Motor::position)
      {
        
        m_position = m_encoder.read();
        v -= m_position;
        v = abs(v);
        
        
//        noInterrupts();
//        //m_output = 1*(m_setPoint-m_position);
//        //m_pid.Compute();
//        interrupts();
        double kp = 18;
        double kd = 20;
        o = kp*(m_setPoint-m_position) + kd*v;
        
      }
      else if(m_mode==Motor::current)
      {
        o = m_output;
      }

      
      if (o > 0)
      {
        digitalWrite(m_drvPinDir, LOW);   
      }
      else
      {
        o = -o;
        digitalWrite(m_drvPinDir, HIGH);
      }

      o += 410;
      o = (o > 2686) ? 2686 : o;
//      for(int i = 0; i<n;i++)
//        if(!valEndSwitch[i])
//          o=410;
      analogWrite(m_drvPinPWM, o);
//      if(m_id==0)
//        {
//          Serial.print(o);
//          Serial.print("\t");
//          Serial.print(m_position);
//          Serial.print("\t");
//          Serial.print(m_setPoint);
//          Serial.print("\t");
//        }
//        else
//        {
//          Serial.print(o);
//          Serial.print("\t");
//          Serial.print(m_position);
//          Serial.print("\t");
//          Serial.println(m_setPoint);
//        }
    }


    void set_pos(int sp)
    {
      m_setPoint = sp;

    };

    void set_current(int16_t o)
    {
      m_current = o;
    };

    long get_pos()
    {
      return m_encoder.read();
    }

    static int nb_mot;

    int m_id;

    Encoder m_encoder;
    QuickPID m_pid;
    float m_current = 0;
    float m_position = 0;
    float m_setPoint = 0;
    float m_output = 0;
    int m_drvPinPWM;
    int m_drvPinEn;
    int m_drvPinDir;
    Motor::Mode m_mode =  Motor::position;


};
