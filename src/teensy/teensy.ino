#include <NativeEthernet.h>
#define DATA_TYPE int32_t

#include "hman.h"

Hman hman;





byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 127, 254);
EthernetServer server(5000);
int pkgSize = 1 + 1 + 8 * NB_MOT; //data on 4 byte

long int ts = micros();
int i;

byte buff[255];
IntervalTimer myTimer;

void setup()
{

  Serial.begin(9600);
  Serial.println("Ethernet Teensy");


  Ethernet.begin(mac, ip);// start the Ethernet connection and the server:
  if (Ethernet.hardwareStatus() == EthernetNoHardware)// Check for Ethernet hardware present
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
  if (Ethernet.linkStatus() == LinkOFF)
    Serial.println("Ethernet cable is not connected.");

  server.begin();// start the server
  Serial.println("server is at " + Ethernet.localIP());
  ts = micros();

  myTimer.begin(update, 1000);
}

void update()
{
  hman.update();
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
              hman.mode() = *(int32_t*)(buff + 2);
              break;
            }
          case 'V'://set value (current, position, speed depending of the mode seleted)
            {
              switch (hman.mode())
              {
                case Hman::position:
                  {
                    hman.set_articular_pos((int32_t*)(buff + 2), index);
                    break;
                  }
                case Hman::current:
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
        }
      }
    }

    client.stop();// close the connection
    Serial.println("client disconnected");
    delay(10);
  }

}
