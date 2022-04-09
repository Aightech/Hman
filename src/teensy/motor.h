#include <Encoder.h>
#include <QuickPID.h>


#define NB_MOT 2

float Kp = 2, Ki = 0.2, Kd = 0.1;

class Motor
{
  public:
  enum uint8_t {positionARTI_mode=0, positionCART_mode=1, current_mode=2};

  
  Motor(int encPin1, int encPin2, int drvPinPWM_, int drvPinEn_, int drvPinDir_): encoder(Encoder(encPin1, encPin2)),
    pid(QuickPID(&position, &output, &setPoint))
  {
    
    pid.SetTunings(Kp, Ki, Kd);
    pid.SetMode(pid.Control::automatic);
    pid.SetOutputLimits(-3686, 3686),
                        analogWriteResolution(12);
    drvPinPWM = drvPinPWM_;
    drvPinEn = drvPinEn_;
    drvPinDir = drvPinDir_;
    pinMode(drvPinPWM, OUTPUT);
    analogWrite(drvPinPWM, 410);
    pinMode(drvPinDir, OUTPUT);
    digitalWrite(drvPinDir, LOW);
    pinMode(drvPinEn, OUTPUT);
    digitalWrite(drvPinEn, HIGH);

  }

  void update()
  {
     position = encoder.read();
      pid.Compute();
      int16_t o = output;//Kp*(setPoint-position);//pid.step(setPoint, position );
      if (o > 0)
      {
        digitalWrite(drvPinDir, LOW);
      }
      else
      {
        o = -o;
        digitalWrite(drvPinDir, HIGH);
      }
  
      o += 410;
      //    Serial.print(output);
      //    Serial.print("\t");
      //Serial.println(o);
      o = (o > 3686) ? 3686 : o;
      analogWrite(drvPinPWM, o);
  }


  void set_pos(int sp)
  {
    setPoint = sp;
   
  };

  void set_current(int16_t o)
  {
    if (o > 0)
      digitalWrite(drvPinDir, LOW);
    else
    {
      o = -o;
      digitalWrite(drvPinDir, HIGH);
    }
    o += 410;
    o = (o > 3686) ? 3686 : o;
    analogWrite(drvPinPWM, o);
  };

  long get_position()
  {
    return encoder.read();
  }
  
  static long posCart[NB_MOT];

  Encoder encoder;
  QuickPID pid;
  float position = 0;
  float setPoint = 0;
  float output = 0;
  int drvPinPWM;
  int drvPinEn;
  int drvPinDir;
  int mode = 0;


};
