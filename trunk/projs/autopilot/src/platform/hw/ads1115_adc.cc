#include "ads1115_adc.h"
#include <stdint.h>
#include <sstream>

namespace autopilot {

Ads1115_ADC::Ads1115_ADC(Ads1115_ADC::VoltageRange v_range, size_t device_i2c_num):
			I2C_ADDRESS(0x48),
	        READ_DATA_REG(0x00),
	        CONFIG_REG(0x01),
			m_i2c(device_i2c_num, I2C_ADDRESS)
{
	// configuration
	m_config_word = 0x4083 + (v_range << 9);
	m_curr_mux = 0;
	m_i2c.write_num<uint16_t>(CONFIG_REG, m_config_word | (m_curr_mux << 12)); 
}

int16_t Ads1115_ADC::read_channel(uint8_t channel) {

	// pick the channel if needed
	if (m_curr_mux != channel) {
		m_i2c.write_num<uint16_t>(CONFIG_REG, m_config_word | (channel << 12)); 
		m_curr_mux = channel;
	}

	// read the data
	return m_i2c.read_num<int16_t>(READ_DATA_REG);
}

}  // namespace autopilot
