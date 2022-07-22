#include <errno.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include <lsl_cpp.h>
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

    float arr[3];
    int32_t pos[3];
    Hman hman;
    try
    {
        hman.connect(argv[1]);
	int nb_ch = 5;
        lsl::stream_info lsl_info("acc", "sample", nb_ch, 0,
                                     lsl::cf_float32);
        lsl::stream_outlet lsl_outlet(lsl_info);
	std::vector<float> sample(nb_ch);
	
	lsl::stream_info lsl_info_pos("pos", "sample", 2, 0,
                                     lsl::cf_int32);
	lsl::stream_outlet lsl_outlet_pos(lsl_info_pos);
	std::vector<int32_t> sample_pos(2);
	std::cout << "[INFOS] Now sending data... " << std::endl;
	for(;;)
	{
	    
	  hman.get_acc(arr, pos);
	    //     hman.set_cartesian_pos(posx, posy);
	    //     hman.turn_off_current();
	    //     std::vector<Hman::Pos> poses(4000);
	    //     hman.record_path(5000000, poses);
	    //     hman.play_path(poses);

	    sample[0] = arr[0];
            sample[1] = arr[1];
	    sample[2] = arr[2];
            lsl_outlet.push_sample(sample);

	    sample_pos[0] = pos[0];
	    sample_pos[1] = pos[1];
	    lsl_outlet_pos.push_sample(sample_pos);
	}
    }
    catch(const std::string err)
    {
      std::cout << err << std::endl;
    }
    catch(std::exception &e)
    {
        std::cerr << "[ERROR] Got an exception: " << e.what() << std::endl;
    }
    
    
    
    return EXIT_SUCCESS;
}
