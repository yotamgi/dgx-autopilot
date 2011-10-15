#include "hmc5843_compass.h"
#include <stdint.h>
#include <sstream>

Hmc5843Compass::Hmc5843Compass(size_t device_i2c_num):
			I2C_ADDRESS(0x1e),
			WHO_AM_I_VALUE(0x48),
			CONFIGURATION_REGISTER_A(0x00),
			CONFIGURATION_REGISTER_B(0x01),
			MODE_REGISTER(0x02),
			X_READ_ADDRES(0x03),
			Y_READ_ADDRES(0x05),
			Z_READ_ADDRES(0x07),
			WHO_AM_I_1(0x0a),
			m_i2c(device_i2c_num, I2C_ADDRESS)
{
	// sanity check
	uint8_t who_am_i = m_i2c.read_num<uint8_t>(WHO_AM_I_1);
	if (who_am_i != WHO_AM_I_VALUE) {
		std::stringstream ss;
		ss << "The value of the who am i value of the hmc5843 compass was " <<
				std::hex << (int)who_am_i << "and not " << (int)WHO_AM_I_VALUE;
		throw HwExcetion(ss.str());
	}

	// configuration
	m_i2c.write_num<uint8_t>(CONFIGURATION_REGISTER_A, 0x18); // 50 tims a second
	m_i2c.write_num<uint8_t>(MODE_REGISTER, 0);
}

Hmc5843Compass::vector_t Hmc5843Compass::get_data() {

	// read the data
	vector_t fans;
	fans[0] = (float)m_i2c.read_num<int16_t>(X_READ_ADDRES)*(100./1620.);
	fans[1] = (float)m_i2c.read_num<int16_t>(Y_READ_ADDRES)*(100./1620.);
	fans[2] = (float)m_i2c.read_num<int16_t>(Z_READ_ADDRES)*(100./1620.);
	
	//std::cout << ans[0] << std::endl;
	//std::cout << (ans[0] << 8) + (ans[0] >> 8) << std::endl;

	std::cout << fans << std::endl;
	return fans;
}
