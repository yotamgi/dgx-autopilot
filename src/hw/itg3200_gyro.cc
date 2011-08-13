#include "itg3200_gyro.h
#include <stdint.h>
#include <sstream>


Itg3200Gyro::Itg3200Gyro(size_t device_i2c_num):m_i2c(device_i2c_num, I2C_ADDRESS)
{
	// sanity chack
	uint8_t who_am_i = m_i2c.read_num<uint8_t>(WHO_AM_I);
	if (who_am_i != WHO_AM_I_VALUE) {
		std::stringstream ss;
		ss << "The value of the who am i value of the itg3200 gyro was " <<
				who_am_i << "and not " << WHO_AM_I_VALUE;
		throw HwExcetion(ss.str())
	}

	// configuration
	m_i2c.write_num<uint8_t>(SAMPLING_CONF, 1);
}


boost::shared_ptr<angular_velocity_t> Itg3200Gyro::get_data() {
	boost::shared_ptr<angular_velocity_t> ans(new angular_velocity_t);

	// read the data
	ans->vx = (float)m_i2c.read_buff<int16_t>(0x1d, 2);
	ans->vy = (float)m_i2c.read_buff<int16_t>(0x1f, 2);
	ans->vz = (float)m_i2c.read_buff<int16_t>(0x21, 2);

	return ans;
}
