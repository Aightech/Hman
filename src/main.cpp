#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "joystick.h"
#include "TCPclient.h"

#define DATA_TYPE int32_t

int main(int argc, char **argv)
{
   if(argc < 2)
   {
      printf("Usage : %s [address] [pseudo]\n", argv[0]);
      return EXIT_FAILURE;
   }

   
  cJoystick js;

   TCPclient client;
   client.open_connection(argv[1], atoi(argv[2]));

   int pkgSize = 2 + sizeof(DATA_TYPE); 
   uint8_t cmd[pkgSize] = {"M0"};
   *(uint32_t*)(cmd+2) = 0;
   for(int i =0; i<2; i++)
     {
       cmd[1]=i;
       client.write(cmd, pkgSize);
       usleep(10);
     }

   cmd[0]='V';
   for(;;)
     {
       
       for(int i =0; i<2; i++)
	 {
	   *(int32_t*)(cmd+2) = js.joystickValue(i)/6;
	   cmd[1]=i;
	   client.write(cmd, pkgSize);
	   usleep(10000);
	 }
     }
     
   
   client.close_connection();
   

   return EXIT_SUCCESS;
}
