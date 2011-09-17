#include "adxl345_acc.h"
#include <stdint.h>
#include <sstream>

Adxl345Acc::Adxl345Acc(size_t device_i2c_num):
			I2C_ADDRESS(0x53),
			WHO_AM_I_VALUE(0xe5),
			WHO_AM_I(0x00),
			POWER_CONTROL(0x2d),
			X_READ_ADDRES(0x32),
			Y_READ_ADDRES(0x36),
			Z_READ_ADDRES(0x36),
			m_i2c(device_i2c_num, I2C_ADDRESS)
{
	// sanity check
	uint8_t who_am_i = m_i2c.read_num<uint8_t>(WHO_AM_I);
	if (who_am_i != WHO_AM_I_VALUE+1) {
		std::stringstream ss;
		ss << "The value of the who am i value of the adxl345 accelerometer was " <<
				std::hex << (int)who_am_i << "and not " << (int)WHO_AM_I_VALUE;
		throw HwExcetion(ss.str());
	}

	// configuration
	m_i2c.write_num<uint8_t>(POWER_CONTROL, 1<<3);
}

Adxl345Acc::vector_t Adxl345Acc::get_data() {

	// read the data
	vector_t ans;
	ans[0] = (float)m_i2c.read_num<int16_t>(X_READ_ADDRES);
	ans[1] = (float)m_i2c.read_num<int16_t>(Z_READ_ADDRES);
	ans[2] = (float)m_i2c.read_num<int16_t>(Y_READ_ADDRES);
	return ans;
}
