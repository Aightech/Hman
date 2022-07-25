#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

#include <NativeEthernet.h>
#define DATA_TYPE int32_t

#include "hman.h"

int Motor::nb_mot =0;

Hman hman;


byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 127, 250);
EthernetServer server(5000);
int pkgSize = 1 + 1 + 8 * NB_MOT; //data on 4 byte

long int ts = micros();
int i;

byte buff[255];
IntervalTimer pid_timer;
IntervalTimer traj_timer;

int state = HIGH;

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
sensors_event_t event;

void displaySensorDetails(void)
{
  sensor_t sensor;
  accel.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" m/s^2");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" m/s^2");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" m/s^2");
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}

void setup()
{
  pid_timer.begin(update_pid, 100);
  traj_timer.begin(update_traj, hman.m_traj_dt_micro);

  Serial.begin(9600);

  Ethernet.begin(mac, ip);// start the Ethernet connection and the server:
  if (Ethernet.hardwareStatus() == EthernetNoHardware)// Check for Ethernet hardware present
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
  if (Ethernet.linkStatus() == LinkOFF)
    Serial.println("Ethernet cable is not connected.");

  server.begin();
  ts = micros();
  
  accel.begin();
  accel.setRange(ADXL345_RANGE_2_G);
  accel.setDataRate(ADXL345_DATARATE_3200_HZ);
  //displaySensorDetails();
  
  int dx=40,dy=0, vm=20, am=60;
  hman.add_delta_traj(dx, dy, vm,am);
  hman.add_delta_traj(-dx, -dy,vm,am);
  hman.start_traj();
}


void update_pid()
{
  hman.update();
}

void update_traj()
{
  hman.trajectory();
}

float v = 0;
void loop()
{
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    while (client.connected())
    {
      
      int len = client.available();
      if (len >= pkgSize)
      {
        client.read(buff, pkgSize);
        uint8_t  index = buff[1];
        switch (buff[0])
        {
          case 'M'://mode
            {
              hman.set_mode((Motor::Mode)*(int32_t*)(buff + 2));
              Serial.println(*(int32_t*)(buff + 2));
              break;
            }
          case 'V'://set value (current, position, speed depending of the mode seleted)
            {
              switch (hman.mode())
              {
                case Motor::position:
                  {
                    hman.set_articular_pos((int32_t*)(buff + 2), index);
                    break;
                  }
                case Motor::current:
                  {
                    hman.set_motor_current((int32_t*)(buff + 2), index);
                    break;
                  }
              }
              break;
            }
          case 'P':// return encoder position
            {
              client.write((uint8_t*)(hman.get_pos()), 4 * index);
              break;
            }
          case 'I':// return digital input
            {
              uint8_t v = digitalRead(index);
              client.write((uint8_t*)(&v), 1);
              break;
            }
          case 'A':// return analog input
            {
              uint16_t v = analogRead(index);
              client.write((uint8_t*)(&v), 2);
              break;
            }
            case 'X':// return encoder position
            {
              //buff
              accel.getEvent(&event);
              client.write((uint8_t*)(event.acceleration.v), 4 * 3);
              client.write((uint8_t*)(hman.get_pos()), 4 * 2);
              break;
            }
            case 'H':// return encoder position
            {
              uint8_t r = hman.home();
              client.write(&r, 1);
              break;
            }
            case 'T':// trajecory mode
            {
              if(index ==1)
                hman.start_traj()
              else if(index==2)
              {
                int dx = (int32_t*)(buff + 2+4*0)
                int dy = (int32_t*)(buff + 2+4*1)
                int vm = (int32_t*)(buff + 2+4*2)
                int am = (int32_t*)(buff + 2+4*3)
                hman.add_delta_traj(dx, dy,vm,am);
              }
              else
                hman.stop_traj()
              break;
            }
        }
      }
    }

    client.stop();// close the connection
    hman.set_current(0);
    
    Serial.println("client disconnected");
    delay(10);
  }

}
