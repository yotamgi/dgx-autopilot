
#ifndef BMP085_BARO_H_ 
#define BMP085_BARO_H_ 

#include <stream/util/lin_algebra.h>
#include <stream/data_pop_stream.h>
#include "platform/hw/i2c_interface.h"

namespace autopilot {

class Bmp085baro : public stream::DataPopStream<float> {
public:

	Bmp085baro(size_t device_i2c_num);

	virtual ~Bmp085baro() {}

	/**
	 * Return the angular velocity
	 */
	float get_data();

private:

	/**
	 * fixed values.
	 */
	const uint8_t I2C_ADDRESS;


	/**
	 * barometer to alt params
	 */
	int16_t m_ac1;
	int16_t m_ac2;
	int16_t m_ac3;
	uint16_t m_ac4;
	uint16_t m_ac5;
	uint16_t m_ac6;
	int16_t m_b1;
	int16_t m_b2;
	int16_t m_mb;
	int16_t m_mc;
	int16_t m_md;

	/**
	 * the i2c device communication object.
	 */
	I2C_Interface m_i2c;
};

}  // namespace autopilot

#endif /* ITG3200_GYRO_H_ */
