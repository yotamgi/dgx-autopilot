
#ifndef BMP085_BARO_H_ 
#define BMP085_BARO_H_ 

#include <stream/util/lin_algebra.h>
#include <stream/data_pop_stream.h>
#include <stream/util/time.h>
#include "platform/hw/i2c_interface.h"

namespace autopilot {

class Bmp085baro : public stream::DataPopStream<float> {
public:

	Bmp085baro(size_t device_i2c_num, bool continious_mode);

	virtual ~Bmp085baro() {}

	/**
	 * Return the angular velocity
	 */
	float get_data();

private:

	/**
	 * Constants
	 */
	static const uint16_t RESOLUTION = 1;
	static const float TEMP_UPDATE_TIME = 1.5;
	static const float PRES_UPDATE_TIME = 0.05;

	void update_temperature();
	void prepare_pressure();
	void read_pressure();
	void calc_alt();

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

	int32_t m_curr_ut;
	int32_t m_curr_up;
	float m_curr_alt;

	/**
	 * Timers for the calculations
	 */
	 Timer m_update_temp_timer;
	 Timer m_update_pres_timer;

	 const bool m_continious_mode;

	/**
	 * the i2c device communication object.
	 */
	I2C_Interface m_i2c;
};

}  // namespace autopilot

#endif /* ITG3200_GYRO_H_ */
