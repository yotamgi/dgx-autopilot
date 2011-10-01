#include <iostream>
#include <linux/i2c-dev.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string>
#include <cstdlib>
#include <stdint.h>

const int COMPASS_I2C_ADD=0x1E;

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

    // CONFIGURING I2C
    if (ioctl(file, I2C_SLAVE, COMPASS_I2C_ADD) < 0) {
      /* ERROR HANDLING; you can check errno to see what went wrong */
      std::cout << "Fuck!" << std::endl;
      std::exit(1);
    }
    std::cout << "Yeh again!" << std::endl;

    // CONFIGURE DEVICE
    uint8_t data[] = {0x02, 0x00};
    if (write(file, data, 2) != 2) {
        std::cout << "Fuck" << std::endl;
        std::exit(1);
    }

    // READING
    while(true) {
            // write 
            uint8_t reg = 0x03;
            if (write(file, &reg, 1) != 1) {
                std::cout << "Fuck" << std::endl;
                std::exit(1);
            }

            uint16_t datax;
            if (read(file, &datax, 2) != 2) {
                std::cout << "Fuck" << std::endl;
                std::exit(1);
            }
            reg = 0x05;
            if (write(file, &reg, 1) != 1) {
                std::cout << "Fuck" << std::endl;
                std::exit(1);
            }

            uint16_t datay;
            if (read(file, &datay, 2) != 2) {
                std::cout << "Fuck" << std::endl;
                std::exit(1);
            }
            reg = 0x07;
            if (write(file, &reg, 1) != 1) {
                std::cout << "Fuck" << std::endl;
                std::exit(1);
            }

            uint16_t dataz;
            if (read(file, &dataz, 2) != 2) {
                std::cout << "Fuck" << std::endl;
                std::exit(1);
            }
            std::cout << "Yeh! data=0x" << std::hex << (unsigned int)datax << ", " << (unsigned int)datay << ", " << (unsigned int)dataz << std::endl;
            usleep(10000);
    }

}
