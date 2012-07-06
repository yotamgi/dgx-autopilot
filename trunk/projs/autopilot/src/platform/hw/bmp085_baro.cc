#include "bmp085_baro.h"
#include <stdint.h>
#include <sstream>

namespace autopilot {

Bmp085baro::Bmp085baro(size_t device_i2c_num, bool continious_mode):
			I2C_ADDRESS(0x77),
			m_continious_mode(continious_mode),
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
}

float Bmp085baro::get_data() {
	

	if (m_continious_mode) {
		if (m_update_temp_timer.passed() > TEMP_UPDATE_TIME) { 
			update_temperature();
			m_update_temp_timer.reset();
			prepare_pressure();
			m_update_pres_timer.reset();
		}

		if (m_update_pres_timer.passed() > PRES_UPDATE_TIME) {
			read_pressure();
			calc_alt();
			prepare_pressure();
			m_update_pres_timer.reset();
		}
	}

	else {
		update_temperature();
		prepare_pressure();
		usleep(25500);
		read_pressure();
		calc_alt();
	}

	return m_curr_alt;
}


void Bmp085baro::update_temperature() {
	//read temperature value
	//datasheet start reading temp 
	m_i2c.write_num<uint8_t>(0xF4, 0x2E);

	usleep(7000);
	m_curr_ut = m_i2c.read_num<uint16_t>(0xf6);
}

void Bmp085baro::prepare_pressure() {
	// tell the sensor to start reading pressure
	m_i2c.write_num<uint8_t>(0xF4, 0x34+(RESOLUTION<<6));
}


void Bmp085baro::read_pressure() {
	std::string raw =  m_i2c.read_buff(0xF6, 3);
	m_curr_up = (raw[2] + (raw[1] << 8) + (raw[0] << 16))>>(8-RESOLUTION);
}

void Bmp085baro::calc_alt() {
	int32_t x1;
	int32_t x2;
	int32_t x3;
	int32_t b3;
	uint32_t b4;
	int16_t b5;
	int32_t b6;
	uint32_t b7;
//	int32_t t;
	int32_t p;

  	x1 = ((m_curr_ut-m_ac6)*m_ac5)>>15;
  	x2 = (m_mc<<11)/(x1+m_md);
  	b5 = x1+x2;
//	t  = (b5+8)>>4;

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
	b7 = ((uint32_t)m_curr_up-b3)*(50000>>RESOLUTION);
	if (b7<0x80000000) 	p  = (b7*2)/b4;
	else				p  = (b7/b4)*2;
	x1 = (p>>8)*(p>>8);
	x1 = (x1*3038)>>16;
	x2 = (-7357*p)>>16;

	p  = p + ((x1+x2+3791)>>4);	

	m_curr_alt = (100500. - p)/10.;
}


}  // namespace autopilot
