
#ifndef BMP085_BARO_H_ 
#define BMP085_BARO_H_ 

#include <stream/util/lin_algebra.h>
#include <stream/data_pop_stream.h>
#include <stream/util/time.h>
#include "platform/hw/i2c_interface.h"

namespace autopilot {

class Bmp085baro : public stream::DataPopStream<float> {
public:

	Bmp085baro(size_t device_i2c_num, bool continues_mode = true, float see_level_pressure=100500.);

	virtual ~Bmp085baro() {}

	/**
	 * Return the angular velocity
	 */
	float get_data();

private:

	/**
	 * Working functions
	 * Those are the functions that does all the work. Get data uses them in
	 * changing order, according to the contious mode flag and timers.
	 */
	void update_temperature();
	void start_updating_pressure();
	void update_pressure();
	float calculate_pressure();

	/**
	 * fixed values.
	 */
	const uint8_t I2C_ADDRESS;
	static const float TEMPERATURE_UPDATE_TIME = 5.0;
	static const float PRESSURE_UPDATE_TIME = 0.05;
	const float m_see_level_pressure;
	const bool m_continous_mode;

	/**
	 * Opcodes
	 */
	static const uint8_t CONTROL_REGISTER 	= 0xF4;
	static const uint8_t DATA_READ 			= 0xF6;
	static const uint16_t RESOLUTION 		= 1; // between 0 to 3

	/**
	 * Control Commands
	 */
	static const uint8_t CONVERT_TEMPERATURE 	= 0x2E;
	static const uint8_t CONVERT_PRESSURE 		= 0x34;

	/**
	 * Pressure parameters
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

	int32_t m_current_ut;
	int32_t m_current_up;

	/**
	 * Timers for the pressure calculations
	 */
	Timer m_temp_update_timer;
	Timer m_pressure_update_timer;
};

}  // namespace autopilot

#endif /* BMP085_BARO_H_ */
