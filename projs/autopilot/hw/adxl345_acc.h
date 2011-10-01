
#ifndef ADXL345_ACC_H_ 
#define ADXL345_ACC_H_ 

#include "stream/generators.h"
#include "hw/i2c_interface.h"

class Adxl345Acc : public stream::VecGenerator<float,3> {
public:
	Adxl345Acc(size_t device_i2c_num);
	virtual ~Adxl345Acc() {}

	/**
	 * Return the angular velocity
	 */
	vector_t get_data();
	
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
	const uint8_t X_READ_ADDRES;
	const uint8_t Y_READ_ADDRES;
	const uint8_t Z_READ_ADDRES;

	/**
	 * the i2c device communication object.
	 */
	I2C_Interface m_i2c;
};


#endif /* ITG3200_GYRO_H_ */
