
#ifndef L3G4200D_GYRO_H_
#define L3G4200D_GYRO_H_

#include <stream/util/lin_algebra.h>
#include <stream/data_pop_stream.h>
#include "platform/hw/i2c_interface.h"

namespace autopilot {

class L3g4200dGyro : public stream::DataPopStream<lin_algebra::vec3f> {
public:
	L3g4200dGyro(size_t device_i2c_num);
	virtual ~L3g4200dGyro() {}

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
	const uint8_t SETUP1;
	const uint8_t SETUP2;
	const uint8_t TEMP_READ_ADDRES;
	const uint8_t X_READ_ADDRES;
	const uint8_t Y_READ_ADDRES;
	const uint8_t Z_READ_ADDRES;

	/**
	 * the i2c device communication object.
	 */
	I2C_Interface m_i2c;
};

}  // namespace autopilot

#endif /* L3G4200D_GYRO_H_ */
