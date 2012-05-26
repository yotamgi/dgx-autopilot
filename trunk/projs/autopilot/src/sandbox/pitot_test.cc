#include <platform/hw/ads1115_adc.h>
#include <iostream>

int main() {
	autopilot::Ads1115_ADC adc(2);
	adc.set_channel_range(0, autopilot::Ads1115_ADC::V_4_096);

	while (true) {
		std::cout << adc.read_channel(0) << std::endl;
	}
}
