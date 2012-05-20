#include "hmc5843_compass.h"
#include <stdint.h>
#include <sstream>

namespace autopilot {

Hmc5843Compass::Hmc5843Compass(size_t device_i2c_num,
				   const Hmc5843Compass::Direction sensor_orentation[3],
				   const bool axis_heading[3]):
			I2C_ADDRESS(0x1e),
			WHO_AM_I_VALUE(0x48),
			CONFIGURATION_REGISTER_A(0x00),
			CONFIGURATION_REGISTER_B(0x01),
			MODE_REGISTER(0x02),
			WHO_AM_I_1(0x0a),
			m_i2c(device_i2c_num, I2C_ADDRESS)
{
	// sanity check
	uint8_t who_am_i = m_i2c.read_num<uint8_t>(WHO_AM_I_1);
	if (who_am_i != WHO_AM_I_VALUE) {
		std::stringstream ss;
		ss << "The value of the who am i value of the hmc5843 compass was " <<
				std::hex << (int)who_am_i << "and not " << (int)WHO_AM_I_VALUE;
		throw SensorException(ss.str());
	}

	// configuration
	m_i2c.write_num<uint8_t>(CONFIGURATION_REGISTER_A, 0x18); // 50 tims a second
	m_i2c.write_num<uint8_t>(MODE_REGISTER, 0);

	// configure the system axis members
	SENSOR_AXIS_ADDRES[0] = 0x03;
	SENSOR_AXIS_ADDRES[1] = 0x05;
	SENSOR_AXIS_ADDRES[2] = 0x07;

	SYSTEM_AXIS_ADDRES[0] = SENSOR_AXIS_ADDRES[sensor_orentation[0]];
	SYSTEM_AXIS_ADDRES[1] = SENSOR_AXIS_ADDRES[sensor_orentation[1]];
	SYSTEM_AXIS_ADDRES[2] = SENSOR_AXIS_ADDRES[sensor_orentation[2]];

	SYSTEM_AXIS_HEADING[0] = axis_heading[0]? 1:-1;
	SYSTEM_AXIS_HEADING[1] = axis_heading[1]? 1:-1;
	SYSTEM_AXIS_HEADING[2] = axis_heading[2]? 1:-1;
}

lin_algebra::vec3f Hmc5843Compass::get_data() {

	// read the data
	lin_algebra::vec3f fans;
	fans[0] = SYSTEM_AXIS_HEADING[0]*m_i2c.read_num<int16_t>(SYSTEM_AXIS_ADDRES[0])*(100./1620.);
	fans[1] = SYSTEM_AXIS_HEADING[1]*m_i2c.read_num<int16_t>(SYSTEM_AXIS_ADDRES[1])*(100./1620.);
	fans[2] = SYSTEM_AXIS_HEADING[2]*m_i2c.read_num<int16_t>(SYSTEM_AXIS_ADDRES[2])*(100./1620.);
	
	return fans;
}

}  // namespace autopilot

