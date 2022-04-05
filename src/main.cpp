#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "client.h"

int main(int argc, char **argv)
{
   if(argc < 2)
   {
      printf("Usage : %s [address] [pseudo]\n", argv[0]);
      return EXIT_FAILURE;
   }

   TCPclient client;

   client.open_connection(argv[1], atoi(argv[2]));

   char buff[5] = {0};
   for(int i =0; i<100; i++)
     {
       buff[0]++;
       client.write(buff, 5);
       usleep(10);
     }
   
   client.close_connection();
   

   return EXIT_SUCCESS;
}
