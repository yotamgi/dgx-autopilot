#include <iostream>
#include <linux/i2c-dev.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string>
#include <cstdlib>
#include <stdint.h>

const int GYRO_I2C_ADD=0x68;

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
    if (ioctl(file, I2C_SLAVE, GYRO_I2C_ADD) < 0) {
      /* ERROR HANDLING; you can check errno to see what went wrong */
      std::cout << "Fuck!" << std::endl;
      std::exit(1);
    }
    std::cout << "Yeh again!" << std::endl;

    // READING
    while(true) {
            // write 
            uint8_t reg = 0x1D;
            if (write(file, &reg, 1) != 1) {
                std::cout << "Fuck" << std::endl;
                std::exit(1);
            }

            uint8_t data;
            if (read(file, &data, 1) != 1) {
                std::cout << "Fuck" << std::endl;
                std::exit(1);
            }
            std::cout << "Yeh! data=0x" << std::hex << (unsigned int)data << std::endl;
            usleep(10000);
    }

}
