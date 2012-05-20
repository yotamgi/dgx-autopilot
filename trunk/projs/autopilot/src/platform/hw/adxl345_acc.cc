#include "adxl345_acc.h"
#include <stdint.h>
#include <sstream>

namespace autopilot {

Adxl345Acc::Adxl345Acc(size_t device_i2c_num,
					   const Adxl345Acc::Direction sensor_orentation[3],
					   const bool axis_heading[3]):
			I2C_ADDRESS(0x53),
			WHO_AM_I_VALUE(0xe5),
			WHO_AM_I(0x00),
			POWER_CONTROL(0x2d),
			DATA_FORMAT(0x31),
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

	// configure the system axis members
	SENSOR_AXIS_ADDRES[0] = 0x32;
	SENSOR_AXIS_ADDRES[1] = 0x34;
	SENSOR_AXIS_ADDRES[2] = 0x36;

	SYSTEM_AXIS_ADDRES[0] = SENSOR_AXIS_ADDRES[sensor_orentation[0]];
	SYSTEM_AXIS_ADDRES[1] = SENSOR_AXIS_ADDRES[sensor_orentation[1]];
	SYSTEM_AXIS_ADDRES[2] = SENSOR_AXIS_ADDRES[sensor_orentation[2]];

	SYSTEM_AXIS_HEADING[0] = axis_heading[0]? 1:-1;
	SYSTEM_AXIS_HEADING[1] = axis_heading[1]? 1:-1;
	SYSTEM_AXIS_HEADING[2] = axis_heading[2]? 1:-1;

}
// y, -z, -x
lin_algebra::vec3f Adxl345Acc::get_data() {

	// read the data
	lin_algebra::vec3f fans;
	uint16_t ans[3];
	ans[0] = m_i2c.read_num<uint16_t>(SYSTEM_AXIS_ADDRES[0]);
	ans[1] = m_i2c.read_num<uint16_t>(SYSTEM_AXIS_ADDRES[1]);
	ans[2] = m_i2c.read_num<uint16_t>(SYSTEM_AXIS_ADDRES[2]);
	
	fans[0] = float(SYSTEM_AXIS_HEADING[0] * ((int16_t)((ans[0]<<8)&0xff00) + (ans[0]>>8)))*0.008;
	fans[1] = float(SYSTEM_AXIS_HEADING[1] * ((int16_t)((ans[1]<<8)&0xff00) + (ans[1]>>8)))*0.008;
	fans[2] = float(SYSTEM_AXIS_HEADING[2] * ((int16_t)((ans[2]<<8)&0xff00) + (ans[2]>>8)))*0.008;
	return fans;
}

}  // namespace autopilot
