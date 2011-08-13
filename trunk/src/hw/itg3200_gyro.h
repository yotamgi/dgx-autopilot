
#ifndef ITG3200_GYRO_H_
#define ITG3200_GYRO_H_

#include "components/generators.h"
#include "hw/i2c_interface.h"
#include "common/types.h"

class Itg3200Gyro : public DataGenerator<angular_velocity_t> {
public:
	Itg3200Gyro(size_t device_i2c_num);
	virtual ~Itg3200Gyro() {}

	/**
	 * Return the angular velocity
	 */
	boost::shared_ptr<angular_velocity_t> get_data();

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
	const uint8_t SAMPLE_RATE_DIVIDOR;
	const uint8_t SAMPLING_CONF;
	const uint8_t TEMP_READ_ADDRES;
	const uint8_t X_READ_ADDRES;
	const uint8_t Y_READ_ADDRES;
	const uint8_t Z_READ_ADDRES;

	/**
	 * the i2c device communication object.
	 */
	I2C_Interface m_i2c;
};


#endif /* ITG3200_GYRO_H_ */
