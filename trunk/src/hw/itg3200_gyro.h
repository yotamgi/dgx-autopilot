
#ifndef ITG3200_GYRO_H_
#define ITG3200_GYRO_H_

#include "components/generators.h"
#include "hw/i2c_interface.h"

struct angular_velocity_t {
	float vx;
	float vy;
	float vz;
};

class Itg3200Gyro : public DataGenerator<angular_velocity_t> {
public:
	Itg3200Gyro(size_t device_i2c_num);
	virtual ~Itg3200Gyro() {}

	boost::shared_ptr<angular_velocity_t> get_data();

private:

	I2C_Interface m_i2c;

	const uint8_t I2C_ADDRESS = 0x68;
	const uint8_t WHO_AM_I_VALUE = I2C_ADDRESS;

	const uint8_t WHO_AM_I				= 0x00;
	const uint8_t SAMPLE_RATE_DIVIDOR	= 0x15;
	const uint8_t SAMPLING_CONF			= 0x16;
	const uint8_t TEMP_READ_ADDRES 		= 0x1b;
	const uint8_t X_READ_ADDRES 		= 0x1d;
	const uint8_t Y_READ_ADDRES 		= 0x1f;
	const uint8_t Z_READ_ADDRES 		= 0x21;
};


#endif /* ITG3200_GYRO_H_ */
