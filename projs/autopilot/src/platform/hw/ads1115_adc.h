
#ifndef ADS1115_ADC_H_
#define ADS1115_ADC_H_

#include "platform/hw/i2c_interface.h"

namespace autopilot {

class Ads1115_ADC {
public:

	enum VoltageRange {
		V_6_144, V_4_096, V_2_048, V_1_024, V_0_512, V_0_256
	};

	Ads1115_ADC(size_t device_i2c_num);

	virtual ~Ads1115_ADC() {}

	/**
	 * Reads the data from the analog channel.
	 * @param channel runs from 0 to 3
	 */
	int16_t read_channel(uint8_t channel);

	void set_channel_range(uint8_t channel, VoltageRange v_range);

private:

	/**
	 * fixed values.
	 */
	const uint8_t I2C_ADDRESS;

	/**
	 * Internal registers
	 */
	const uint8_t READ_DATA_REG;
	const uint8_t CONFIG_REG;

	uint8_t m_curr_mux;
	uint16_t m_config_word;

	VoltageRange m_channel_ranges[4];

	/**
	 * the i2c device communication object.
	 */
	I2C_Interface m_i2c;
};

}  // namespace autopilot

#endif /* ADS1115_ADC_H_ */
