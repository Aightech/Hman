#include <errno.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include "hman.hpp"
#include "joystick.h"

int main(int argc, char **argv)
{
    float arr[3];
    int32_t pos[3];
    Hman hman(2, 2);
    cJoystick js;
    double posx;
    double posy;
    try
    {
        js.connect("/dev/input/js0", "");
        hman.connect("192.168.127.250");

        bool running = true;
        while(running)
        {
            posx = js.joystickValue(0) / 100;
            posy = js.joystickValue(1) / 100;
            std::cout << posx << " " << posy << std::endl;

            if(js.buttonPressed(1) == 1 && js.buttonPressed(0) == 1)
                running = false;
            hman.set_cartesian_pos(posx, posy);
            usleep(10000);
            std::cout << posx << " " << posy << std::endl;
            // std::vector<Hman::Pos> poses(4000);
            // hman.record_path(5000000, poses);
            // hman.play_path(poses);
        }
        std::cout <<running<< std::endl;
    }
    catch(const std::string err)
    {
        hman.turn_off_current();
        std::cout << err << std::endl;
    }
    hman.turn_off_current();

    return EXIT_SUCCESS;
}
