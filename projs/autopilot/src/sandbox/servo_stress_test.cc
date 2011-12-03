#include "hw/maestro_servo_controller.h"
#include <stdlib.h>
#include <iostream>

int main(int argc, char** argv) {
	if (argc != 2) {
		std::cout << "USAGE: " << argv[0] << " PORT. i.e. /dev/ttyACM1.\n" << std::endl;
		exit(-1); // EXIT
	}

	MaestroServoController m(argv[1]);
	int target;

	while(1) {
		for(target=4000; target < 8001; target++){
			std::cout << "target is " <<  target << std::endl;
			for (int i=0; i<1000; i++ ) {
				int shaked_target = target + (((i*13)%500) - 250);

                try {
				m.getServoChannel(5)->set_data(float(target-4000)/40); // in percentage
                } catch (...) {
                }
			}
		}
	}
	return 0;
}
