
#ifndef HMC5843_COMPASS_H
#define HMC5843_COMPASS_H

#include <stream/data_pop_stream.h>
#include <stream/util/lin_algebra.h>
#include "platform/hw/i2c_interface.h"

namespace autopilot {

class Hmc5843Compass : public stream::DataPopStream<lin_algebra::vec3f> {
public:

	enum Direction {
		X_DIR = 0, Y_DIR = 1, Z_DIR = 2
	};

	Hmc5843Compass(size_t device_i2c_num,
			const Direction sensor_orentation[3] = (Direction[3]){ X_DIR, Y_DIR, Z_DIR },
			const bool axis_heading[3] = (bool[3]){ true, true, true }
	);
	virtual ~Hmc5843Compass() {}

	/**
	 * Returns the north vector in Mili-Tesla units.
	 */
	lin_algebra::vec3f get_data();
	
	virtual bool is_data_losed() { return false; };

private:

	/**
	 * fixed values.
	 */
	const uint8_t I2C_ADDRESS;
	const uint8_t WHO_AM_I_VALUE;

	/**
	 * Internal registers
	 */
	const uint8_t CONFIGURATION_REGISTER_A;
	const uint8_t CONFIGURATION_REGISTER_B;
	const uint8_t MODE_REGISTER;
	const uint8_t WHO_AM_I_1;
	uint8_t SENSOR_AXIS_ADDRES[3];

	uint8_t SYSTEM_AXIS_ADDRES[3];
	int16_t SYSTEM_AXIS_HEADING[3];

	/**
	 * the i2c device communication object.
	 */
	I2C_Interface m_i2c;
};

}  // namespace autopilot

#endif /* HMC5843_COMPASS_H */
