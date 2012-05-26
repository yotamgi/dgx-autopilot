#include <platform/hw/ads1115_adc.h>
#include <platform/hw/mpxv7002_pitot.h>
#include <iostream>
#include <boost/make_shared.hpp>

int main() {
	boost::shared_ptr<autopilot::Ads1115_ADC> adc = boost::make_shared<autopilot::Ads1115_ADC>(2);
	autopilot::Mpxv7002Pitot p(adc, 0);

	while (true) {
		std::cout << p.get_data() << std::endl;
	}
}
