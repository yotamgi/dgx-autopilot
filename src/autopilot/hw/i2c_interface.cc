#include "i2c_interface.h"
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>


I2C_Interface::I2C_Interface(size_t device_num, uint8_t device_i2c_add) {
	std::stringstream ss;
	ss << "/dev/i2c-" << device_num;
	init(ss.str(), device_i2c_add);
}

I2C_Interface::I2C_Interface(std::string device_filename, uint8_t device_i2c_add) {
	init(device_filename, device_i2c_add);
}


void I2C_Interface::init(std::string device_name, uint8_t device_i2c_add) {
	m_i2c_device_fd = open(device_name.c_str(), O_RDWR);

	if (m_i2c_device_fd < 0) {
		throw HwExcetion("Could not open i2c device " + device_name);
	}

	if (ioctl(m_i2c_device_fd, I2C_SLAVE, device_i2c_add) < 0) {
		throw HwExcetion("Could not ioctl i2c device " + device_name);
	}
}
