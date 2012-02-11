#include <iostream>
#include <linux/i2c-dev.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string>
#include <cstdlib>
#include <stdint.h>

const int BARO_I2C_ADD=0x77;

int main() {
    std::string filename = "/dev/i2c-2";

    // OPENING
    int file = open(filename.c_str(), O_RDWR);
    if (file < 0) {
        /* ERROR HANDLING; you can check errno to see what went wrong */
        std::cout << "Fuck!" << std::endl;
        std::exit(1);
    }
    std::cout << "Yeh!" << std::endl;

    // CONFIGURING
    if (ioctl(file, I2C_SLAVE, BARO_I2C_ADD) < 0) {
      /* ERROR HANDLING; you can check errno to see what went wrong */
      std::cout << "Fuck!" << std::endl;
      std::exit(1);
    }
    std::cout << "Yeh again!" << std::endl;


    // READING
    while(true) {

			// tell it to choose temprature
			uint8_t packet[2] = {0xf4, 0x2e};
			if (write(file, packet, 2) != 2) {
				std::cout << "Fuck" << std::endl;
				std::exit(1);
			}
			usleep(5000);

            // write
            uint8_t reg = 0xf6;
            if (write(file, &reg, 1) != 1) {
                std::cout << "Fuck" << std::endl;
                std::exit(1);
            }

            uint8_t data1;
            if (read(file, &data1, 1) != 1) {
                std::cout << "Fuck" << std::endl;
                std::exit(1);
            }

            // write
            reg = 0xf7;
            if (write(file, &reg, 1) != 1) {
                std::cout << "Fuck" << std::endl;
                std::exit(1);
            }

            uint8_t data2;
            if (read(file, &data2, 1) != 1) {
                std::cout << "Fuck" << std::endl;
                std::exit(1);
            }
            std::cout << "Yeh! data=0x" << std::hex << (unsigned int)data1 << (unsigned int)data2 << std::endl;
            usleep(10000);
    }

}
