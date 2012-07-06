#include <platform/hw/bmp085_baro.h> 
#include <iostream>


int main() {

	autopilot::Bmp085baro b(2, true);
	while(true) {
		std::cout << b.get_data() << std::endl;
	}

}
