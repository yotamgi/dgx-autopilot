
#ifndef ADXL345_ACC_H_ 
#define ADXL345_ACC_H_ 

#include <stream/util/lin_algebra.h>
#include <stream/data_pop_stream.h>
#include "platform/hw/i2c_interface.h"

namespace autopilot {

class Adxl345Acc : public stream::DataPopStream<lin_algebra::vec3f> {
public:

	enum Direction {
		X_DIR = 0, Y_DIR = 1, Z_DIR = 2
	};

	Adxl345Acc(size_t device_i2c_num,
			const Direction sensor_orentation[3] = (Direction[3]){ X_DIR, Y_DIR, Z_DIR },
			const bool axis_heading[3] = (bool[3]){ true, true, true }
	);

	virtual ~Adxl345Acc() {}

	/**
	 * Return the angular velocity
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
	const uint8_t WHO_AM_I;
	const uint8_t POWER_CONTROL;
	const uint8_t DATA_FORMAT;
	uint8_t SENSOR_AXIS_ADDRES[3];

	uint8_t SYSTEM_AXIS_ADDRES[3];
	int16_t SYSTEM_AXIS_HEADING[3];

	/**
	 * the i2c device communication object.
	 */
	I2C_Interface m_i2c;
};

}  // namespace autopilot

#endif /* ITG3200_GYRO_H_ */
