#include <NativeEthernet.h>
#define DATA_TYPE int32_t

#include "motor.h"
long Motor::posCart[NB_MOT]={0,0};


Motor* motors[NB_MOT];


byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 127, 254);
EthernetServer server(5000);
int pkgSize = 1 + 1 + 8*NB_MOT; //data on 4 byte

long int ts = micros();
int i;

byte buff[255];

void setup()
{

  for (int i = 0; i < NB_MOT; i++)
    motors[i] = new Motor( 22 - 3 * i, 22 - 3 * i - 1,       2 + 3 * i,    2 + 3 * i + 1,   2 + 3 * i + 2);

  Serial.begin(9600);
  Serial.println("Ethernet Teensy");

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware)
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
  if (Ethernet.linkStatus() == LinkOFF)
    Serial.println("Ethernet cable is not connected.");
  // start the server
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  ts = micros();
}

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
              int32_t val = *(int32_t*)(buff + 2);
              motors[index]->mode = val;
              break;
            }
          case 'V'://set value (current, position, speed depending of the mode seleted)
            {
              switch (motors[index]->mode)
              {
                case Motor::positionARTI_mode:
                  {
                    for(i=0; i<NB_MOT;i++)
                      motors[i]->set_pos(*(int32_t*)(buff + 2 + 4*i));
                    break;
                  }
                  case Motor::positionCART_mode:
                  {
                    int sign = (index==0)?-1:1;
                    //motors[0]->set_pos(Motor::posCart[index]/2 + val/2);
                    //motors[0]->set_pos(sign*Motor::posCart[index]/2 - sign*val/2);
                    break;
                  }
                case Motor::current_mode:
                  {
                    //motors[index]->update_current(val);
                    break;
                  }
              }
              break;
            }
          case 'P':// return encoder position
            {
              long pos = motors[index]->get_position();
              client.write((uint8_t*)(&pos), 4);
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
        }
      }
    }
    
    client.stop();// close the connection
    Serial.println("client disconnected");
    delay(10);
  }
  for (int i = 0; i < NB_MOT; i++)
      motors[i]->update();
    //delay(10);    // give the web browser time to receive the data




}
