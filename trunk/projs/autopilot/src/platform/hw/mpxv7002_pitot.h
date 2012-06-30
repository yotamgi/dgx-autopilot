
#ifndef BMP085_BARO_H_ 
#define BMP085_BARO_H_ 

#include "ads1115_adc.h"
#include <stream/util/lin_algebra.h>
#include <stream/data_pop_stream.h>
#include <boost/shared_ptr.hpp>
#include <math.h>

namespace autopilot {

class Mpxv7002Pitot : public stream::DataPopStream<float> {
public:

	Mpxv7002Pitot(boost::shared_ptr<Ads1115_ADC> adc, uint8_t channel):
		m_adc(adc), m_channel(channel)
	{
		m_adc->set_channel_range(m_channel, Ads1115_ADC::V_4_096);

		// assume that we start at airspeed 0, so check the zero voltage
		m_zero_calibration_voltage = (float(m_adc->read_channel(m_channel))/float(0x7fff))*4.096;
		std::cout << "Calibrated pitot's zero calibration voltage to " << m_zero_calibration_voltage << std::endl;
	}

	virtual ~Mpxv7002Pitot() {}

	float get_data() { 
		float data_volts = (float(m_adc->read_channel(m_channel))/float(0x7fff))*4.096;
		float pressure_diff_pa = 100*(data_volts - m_zero_calibration_voltage);
		if (pressure_diff_pa < 0.001) pressure_diff_pa = 0.001;
		return sqrt(2*pressure_diff_pa/1.2041);
	}

private:

	boost::shared_ptr<Ads1115_ADC> m_adc;
	uint8_t m_channel;

	float m_zero_calibration_voltage;
};

}  // namespace autopilot

#endif /* BMP085_BARO_H_ */
