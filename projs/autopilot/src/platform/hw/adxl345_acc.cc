#include "adxl345_acc.h"
#include <stdint.h>
#include <sstream>

namespace autopilot {

Adxl345Acc::Adxl345Acc(size_t device_i2c_num):
			I2C_ADDRESS(0x53),
			WHO_AM_I_VALUE(0xe5),
			WHO_AM_I(0x00),
			POWER_CONTROL(0x2d),
			DATA_FORMAT(0x31),
			X_READ_ADDRES(0x32),
			Y_READ_ADDRES(0x34),
			Z_READ_ADDRES(0x36),
			m_i2c(device_i2c_num, I2C_ADDRESS)
{
	// sanity check
	uint8_t who_am_i = m_i2c.read_num<uint8_t>(WHO_AM_I);
	if (who_am_i != WHO_AM_I_VALUE) {
		std::stringstream ss;
		ss << "The value of the who am i value of the adxl345 accelerometer was " <<
				std::hex << (int)who_am_i << "and not " << (int)WHO_AM_I_VALUE;
		throw SensorException(ss.str());
	}

	// configuration
	m_i2c.write_num<uint8_t>(POWER_CONTROL, 1<<3);
	m_i2c.write_num<uint8_t>(DATA_FORMAT, 1); // +-4G
}

lin_algebra::vec3f Adxl345Acc::get_data() {

	// read the data
	lin_algebra::vec3f fans;
	uint16_t ans[3];
	ans[0] = m_i2c.read_num<uint16_t>(X_READ_ADDRES);
	ans[1] = m_i2c.read_num<uint16_t>(Z_READ_ADDRES);
	ans[2] = m_i2c.read_num<uint16_t>(Y_READ_ADDRES);
	
	fans[0] =  float((int16_t)((ans[0]<<8)&0xff00) + (ans[0]>>8))*0.008;
	fans[1] = -float((int16_t)((ans[1]<<8)&0xff00) + (ans[1]>>8))*0.008;
	fans[2] =  float((int16_t)((ans[2]<<8)&0xff00) + (ans[2]>>8))*0.008;
	return fans;
}

}  // namespace autopilot
