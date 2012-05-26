#include "ads1115_adc.h"
#include <stdint.h>
#include <sstream>
#include <iostream>

namespace autopilot {

Ads1115_ADC::Ads1115_ADC(size_t device_i2c_num):
			I2C_ADDRESS(0x48),
	        READ_DATA_REG(0x00),
	        CONFIG_REG(0x01),
			m_i2c(device_i2c_num, I2C_ADDRESS)
{

	for (size_t i=0; i<4; i++) {
		m_channel_ranges[i] = V_6_144;
	}

	// configuration - set the default channel to 0
	m_config_word = 0x4083;
	m_curr_mux = 0xff;
}

void Ads1115_ADC::set_channel_range(uint8_t channel, Ads1115_ADC::VoltageRange v_range) {
	m_channel_ranges[channel] = v_range;	
}

int16_t Ads1115_ADC::read_channel(uint8_t channel) {

	// pick the channel if needed
	if (m_curr_mux != channel) {
		std::cout << "SETING TO CHANNEL " << channel << " WITH RANGE " << m_channel_ranges[channel];
		m_i2c.write_num<uint16_t>(CONFIG_REG, m_config_word | (channel << 12) | (m_channel_ranges[channel] << 9)); 
		std::cout << std::hex << " CONF " << (m_config_word | (channel << 12) | (m_channel_ranges[channel] << 9)) << std::endl;
		m_curr_mux = channel;
	}


	// read the data
	return m_i2c.read_num<int16_t>(READ_DATA_REG);
}

}  // namespace autopilot
