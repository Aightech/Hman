#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <vector>

#include "joystick.h"

#include "hman.hpp"

#define DATA_TYPE int32_t

int main(int argc, char **argv)
{
   if(argc < 2)
   {
      printf("Usage : %s [address]\n", argv[0]);
      return EXIT_FAILURE;
   }

   
  cJoystick js;

   Hman hman;

   std::vector<Hman::Pos> poses(100000);
   hman.record_path(500000, poses);
   // hman.connect(argv[1]);

   // for(;;)
   //   {
   // 	int32_t posx = js.joystickValue(0)/15;
   // 	int32_t posy = js.joystickValue(1)/15;
   // 	hman.set_cartesian_pos(posx, posy);
   // 	usleep(1000);
   //   }
   

   return EXIT_SUCCESS;
}
