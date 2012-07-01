#include "bmp085_baro.h"
#include <stdint.h>
#include <sstream>

namespace autopilot {

Bmp085baro::Bmp085baro(size_t device_i2c_num, bool continues_mode, float see_level_pressure):
			I2C_ADDRESS(0x77),
			m_see_level_pressure(see_level_pressure),
			m_continous_mode(continues_mode),
			m_i2c(device_i2c_num, I2C_ADDRESS)
{

	m_ac1 = m_i2c.read_num<int16_t>(0xaa);   // 408;   
	m_ac2 = m_i2c.read_num<int16_t>(0xac);   // -72;   
	m_ac3 = m_i2c.read_num<int16_t>(0xae);   // -14383;
	m_ac4 = m_i2c.read_num<uint16_t>(0xb0);  // 32741; 
	m_ac5 = m_i2c.read_num<uint16_t>(0xb2);  // 32757; 
	m_ac6 = m_i2c.read_num<uint16_t>(0xb4);  // 23153; 
	m_b1  = m_i2c.read_num<int16_t>(0xb6);   // 6190;  
	m_b2  = m_i2c.read_num<int16_t>(0xb8);   // 4;     
	m_mb  = m_i2c.read_num<int16_t>(0xba);   // -32768;
	m_mc  = m_i2c.read_num<int16_t>(0xbc);   // -8711; 
	m_md  = m_i2c.read_num<int16_t>(0xbe);   // 2868;

	update_temperature();
	start_updating_pressure();
	usleep(25500);
	update_pressure();
}

void Bmp085baro::update_temperature() {

	// start temperature conversion
	m_i2c.write_num<uint8_t>(CONTROL_REGISTER, CONVERT_TEMPERATURE);

	// sleep 4.5ms
	usleep(4500);
	m_current_ut = m_i2c.read_num<uint16_t>(DATA_READ);
}

void Bmp085baro::start_updating_pressure() {
	// start pressure conversion
	m_i2c.write_num<uint8_t>(CONTROL_REGISTER, CONVERT_TEMPERATURE+(RESOLUTION<<6));
}

void Bmp085baro::update_pressure() {

	std::string raw =  m_i2c.read_buff(DATA_READ, 3);

	m_current_up = (raw[2] + (raw[1] << 8) + (raw[0] << 16))>>(8-RESOLUTION);
}

float Bmp085baro::calculate_pressure() {
	int32_t x1;
	int32_t x2;
	int32_t x3;
	int32_t b3;
	uint32_t b4;
	int16_t b5;
	int32_t b6;
	uint32_t b7;
	int32_t p;

  	x1 = ((m_current_ut-m_ac6)*m_ac5)>>15;
  	x2 = (m_mc<<11)/(x1+m_md);
  	b5 = x1+x2;

	// calc true pressure
	b6 = b5 - 4000;
	x1 = (m_b2*((b6*b6)>>12))>>11;
	x2 = m_ac2*b6>>11;
	x3 = x1 + x2;
	b3 = (((m_ac1*4+x3)<<RESOLUTION)+2)>>2;
	x1 = m_ac3*b6>>13;
	x2 = (m_b1*(b6*b6>>12))>>16;
	x3 = ((x1 +x2)+2)>>2;
	b4 = m_ac4*(uint32_t)(x3+32768)>>15;
	b7 = ((uint32_t)m_current_up-b3)*(50000>>RESOLUTION);
	if (b7<0x80000000) 	p  = (b7*2)/b4;
	else				p  = (b7/b4)*2;
	x1 = (p>>8)*(p>>8);
	x1 = (x1*3038)>>16;
	x2 = (-7357*p)>>16;

	p  = p + ((x1+x2+3791)>>4);	

	return (m_see_level_pressure - p)/10.;
}

float Bmp085baro::get_data() {

	// update the temperature if needed
	if (m_temp_update_timer.passed() > TEMPERATURE_UPDATE_TIME) {
		update_temperature();
		m_temp_update_timer.reset();
	}

	// continious mode
	if (m_continous_mode) {

		if (m_pressure_update_timer.passed() > PRESSURE_UPDATE_TIME) {
			update_pressure();
			start_updating_pressure();
			m_pressure_update_timer.reset();
		}

		return calculate_pressure();
	}

	// non-continious mode
	else {

		start_updating_pressure();
		usleep(25500);
		update_pressure();

		return calculate_pressure();
	}

}

}  // namespace autopilot
