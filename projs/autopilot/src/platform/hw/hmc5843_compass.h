
#ifndef HMC5843_COMPASS_H
#define HMC5843_COMPASS_H

#include <stream/generators.h>
#include <stream/util/lin_algebra.h>
#include "platform/hw/i2c_interface.h"

class Hmc5843Compass : public stream::DataGenerator<lin_algebra::vec3f> {
public:
	Hmc5843Compass(size_t device_i2c_num);
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
	const uint8_t X_READ_ADDRES;
	const uint8_t Y_READ_ADDRES;
	const uint8_t Z_READ_ADDRES;
	const uint8_t WHO_AM_I_1;

	/**
	 * the i2c device communication object.
	 */
	I2C_Interface m_i2c;
};


#endif /* HMC5843_COMPASS_H */
