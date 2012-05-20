#include "itg3200_gyro.h"
#include <stdint.h>
#include <sstream>

namespace autopilot {

Itg3200Gyro::Itg3200Gyro(size_t device_i2c_num,
					     const Itg3200Gyro::Direction sensor_orentation[3],
					     const bool axis_heading[3]):
			I2C_ADDRESS(0x68),
			WHO_AM_I_VALUE(I2C_ADDRESS),
			WHO_AM_I(0x00),
			SAMPLE_RATE_DIVIDOR(0x15),
			SAMPLING_CONF(0x16),
			TEMP_READ_ADDRES(0x1b),
			m_i2c(device_i2c_num, I2C_ADDRESS)
{
	// sanity chack
	uint8_t who_am_i = m_i2c.read_num<uint8_t>(WHO_AM_I);
	if (who_am_i != WHO_AM_I_VALUE+1) {
		std::stringstream ss;
		ss << "The value of the who am i value of the itg3200 gyro was " <<
				std::hex << (int)who_am_i << "and not " << (int)WHO_AM_I_VALUE;
		throw SensorException(ss.str());
	}

	// configuration
	m_i2c.write_num<uint8_t>(SAMPLING_CONF, (0x3<<3) | 0x0); // FS_SEL=0x3, DLFP_CFG=0x0

	// configure the system axis members
	SENSOR_AXIS_ADDRES[0] = 0x1d;
	SENSOR_AXIS_ADDRES[1] = 0x1f;
	SENSOR_AXIS_ADDRES[2] = 0x21;

	SYSTEM_AXIS_ADDRES[0] = SENSOR_AXIS_ADDRES[sensor_orentation[0]];
	SYSTEM_AXIS_ADDRES[1] = SENSOR_AXIS_ADDRES[sensor_orentation[1]];
	SYSTEM_AXIS_ADDRES[2] = SENSOR_AXIS_ADDRES[sensor_orentation[2]];

	SYSTEM_AXIS_HEADING[0] = axis_heading[0]? 1:-1;
	SYSTEM_AXIS_HEADING[1] = axis_heading[1]? 1:-1;
	SYSTEM_AXIS_HEADING[2] = axis_heading[2]? 1:-1;

}

// X, -Z, Y

lin_algebra::vec3f Itg3200Gyro::get_data() {

	// read the data
	lin_algebra::vec3f ans;
	ans[0] = SYSTEM_AXIS_HEADING[0]*m_i2c.read_num<int16_t>(SYSTEM_AXIS_ADDRES[0])/14.375;
	ans[1] = SYSTEM_AXIS_HEADING[1]*m_i2c.read_num<int16_t>(SYSTEM_AXIS_ADDRES[1])/14.375;
	ans[2] = SYSTEM_AXIS_HEADING[2]*m_i2c.read_num<int16_t>(SYSTEM_AXIS_ADDRES[2])/14.375;
	return ans;
}

}  // namespace autopilot
