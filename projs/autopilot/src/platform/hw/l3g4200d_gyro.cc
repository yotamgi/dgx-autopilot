#include "l3g4200d_gyro.h"
#include <stdint.h>
#include <sstream>

namespace autopilot {

L3g4200dGyro::L3g4200dGyro(size_t device_i2c_num):
			I2C_ADDRESS(0x69),
			WHO_AM_I_VALUE(0xD3),
			WHO_AM_I(0x0f),
			SETUP1(0x20),
			SETUP2(0x23),
			TEMP_READ_ADDRES(0x26), //only 1 byte
			X_READ_ADDRES(0x28),
			Y_READ_ADDRES(0x2A),
			Z_READ_ADDRES (0x2C),
			m_i2c(device_i2c_num, I2C_ADDRESS)
{
	// sanity chack
	uint8_t who_am_i = m_i2c.read_num<uint8_t>(WHO_AM_I);
	if (who_am_i != WHO_AM_I_VALUE) {
		std::stringstream ss;
		ss << "The value of the who am i value of the l3d4200d gyro was " <<
				std::hex << (int)who_am_i << "and not " << (int)WHO_AM_I_VALUE;
		throw SensorException(ss.str());
	}

	// configuration
	m_i2c.write_num<uint8_t>(SETUP1, 0x8F); // 
	m_i2c.write_num<uint8_t>(SETUP2, 0x50);
}

lin_algebra::vec3f L3g4200dGyro::get_data() {

	// read the data
	lin_algebra::vec3f ans;
	ans[0] =  (float)m_i2c.read_num<int16_t>(Y_READ_ADDRES)*0.0175;
	ans[1] = -(float)m_i2c.read_num<int16_t>(Z_READ_ADDRES)*0.0175;
	ans[2] = -(float)m_i2c.read_num<int16_t>(X_READ_ADDRES)*0.0175;
	return ans;
}


}  // namespace autopilot
