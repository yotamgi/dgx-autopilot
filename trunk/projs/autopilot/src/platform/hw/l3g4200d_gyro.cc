#include "l3g4200d_gyro.h"
#include <stdint.h>
#include <sstream>

namespace autopilot {

L3g4200dGyro::L3g4200dGyro(size_t device_i2c_num,
						   const L3g4200dGyro::Direction sensor_orentation[3],
						   const bool axis_heading[3]):
			I2C_ADDRESS(0x69),
			WHO_AM_I_VALUE(0xD3),
			WHO_AM_I(0x0f),
			SETUP1(0x20),
			SETUP2(0x23),
			TEMP_READ_ADDRES(0x26), //only 1 byte
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

	// configure the system axis members
	SENSOR_AXIS_ADDRES[0] = 0x28;
	SENSOR_AXIS_ADDRES[1] = 0x2a;
	SENSOR_AXIS_ADDRES[2] = 0x2c;

	SYSTEM_AXIS_ADDRES[0] = SENSOR_AXIS_ADDRES[sensor_orentation[0]];
	SYSTEM_AXIS_ADDRES[1] = SENSOR_AXIS_ADDRES[sensor_orentation[1]];
	SYSTEM_AXIS_ADDRES[2] = SENSOR_AXIS_ADDRES[sensor_orentation[2]];

	SYSTEM_AXIS_HEADING[0] = axis_heading[0]? 1:-1;
	SYSTEM_AXIS_HEADING[1] = axis_heading[1]? 1:-1;
	SYSTEM_AXIS_HEADING[2] = axis_heading[2]? 1:-1;
}

lin_algebra::vec3f L3g4200dGyro::get_data() {

	// read the data
	lin_algebra::vec3f ans;
	ans[0] = SYSTEM_AXIS_HEADING[0]*m_i2c.read_num<int16_t>(SYSTEM_AXIS_ADDRES[0])*0.0175;
	ans[1] = SYSTEM_AXIS_HEADING[1]*m_i2c.read_num<int16_t>(SYSTEM_AXIS_ADDRES[1])*0.0175;
	ans[2] = SYSTEM_AXIS_HEADING[2]*m_i2c.read_num<int16_t>(SYSTEM_AXIS_ADDRES[2])*0.0175;
	return ans;
}


}  // namespace autopilot
