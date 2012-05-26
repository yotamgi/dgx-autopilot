#include <platform/hw/ads1115_adc.h>
#include <iostream>

int main() {
	autopilot::Ads1115_ADC adc(autopilot::Ads1115_ADC::V_4_096, 2);
	while (true) {
		std::cout << adc.read_channel(0) << std::endl;
	}
}
