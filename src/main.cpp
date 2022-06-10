#include <errno.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include "hman.hpp"
#include "joystick.h"

#define DATA_TYPE int32_t

int
main(int argc, char **argv)
{
    if(argc < 2)
    {
        printf("Usage : %s [address]\n", argv[0]);
        return EXIT_FAILURE;
    }

    cJoystick js;
    try
    {
        js.connect();
    }
    catch(const std::string err)
    {
        std::cout << err << std::endl;
    }

    Hman hman;
    try
    {
      hman.connect(argv[1]);
    }
    catch(const std::string err)
    {
        std::cout << err << std::endl;
    }

    std::cout << "\n\nh" << std::endl;
 
    std::vector<Hman::Pos> poses(4000);

    for(;;)
    {
        int32_t posx = js.joystickValue(0) / 15;
        int32_t posy = js.joystickValue(1) / 15;
        std::cout << "                            \xd" << posx << " " << posy
                  << "\xd" << std::flush;

        if(js.buttonPressed(0))
        {
            hman.set_cartesian_pos(posx, posy);
            usleep(1000);
        }
        if(js.buttonPressed(1))
        {
            hman.turn_off_current();
            usleep(1000);
        }
        if(js.buttonPressed(2))
        {
            std::cout << "start recording" << std::endl;
            hman.record_path(5000000, poses);
            std::cout << "stop recording" << std::endl;
        }
        if(js.buttonPressed(3))
        {
            std::cout << "start playing" << std::endl;
            hman.play_path(poses);
            std::cout << "stop playing" << std::endl;
        }
    }

    return EXIT_SUCCESS;
}
