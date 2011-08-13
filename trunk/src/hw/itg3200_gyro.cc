#include "itg3200_gyro.h"
#include <stdint.h>
#include <sstream>

Itg3200Gyro::Itg3200Gyro(size_t device_i2c_num):
			m_i2c(device_i2c_num, I2C_ADDRESS),
			I2C_ADDRESS(0x68),
			WHO_AM_I_VALUE(I2C_ADDRESS),
			WHO_AM_I(0x00),
			SAMPLE_RATE_DIVIDOR(0x15),
			SAMPLING_CONF(0x16),
			TEMP_READ_ADDRES(0x1b),
			X_READ_ADDRES(0x1d),
			Y_READ_ADDRES(0x1f),
			Z_READ_ADDRES (0x21)
{
	// sanity chack
	uint8_t who_am_i = m_i2c.read_num<uint8_t>(WHO_AM_I);
	if (who_am_i != WHO_AM_I_VALUE) {
		std::stringstream ss;
		ss << "The value of the who am i value of the itg3200 gyro was " <<
				who_am_i << "and not " << WHO_AM_I_VALUE;
		throw HwExcetion(ss.str());
	}

	// configuration
	m_i2c.write_num<uint8_t>(SAMPLING_CONF, (0x3<<3) | 0x0); // FS_SEL=0x3, DLFP_CFG=0x0
}

boost::shared_ptr<angular_velocity_t> Itg3200Gyro::get_data() {
	boost::shared_ptr<angular_velocity_t> ans(new angular_velocity_t);

	// read the data
	ans->vx = (float)m_i2c.read_num<int16_t>(0x1d);
	ans->vy = (float)m_i2c.read_num<int16_t>(0x1f);
	ans->vz = (float)m_i2c.read_num<int16_t>(0x21);

	return ans;
}
