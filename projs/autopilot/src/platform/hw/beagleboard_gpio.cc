#include "beagleboard_gpio.h"
#include <iostream>
#include <sstream>
#include <fcntl.h>
#include <stdio.h>

namespace autopilot {

const std::string GPIO_PATH = "/sys/class/gpio/";

GpioPin::GpioPin(unsigned int pin_num):
		m_curr_state(false)
{
	std::stringstream pin_num_ss;
	pin_num_ss << pin_num;

	int master_fd = open((GPIO_PATH + "export").c_str(), O_WRONLY);

	if (master_fd < 0) {
		perror("");
		throw SensorException("Could not open master gpio file");
	}

	int ans = write(master_fd, pin_num_ss.str().c_str(), pin_num_ss.str().size());
	if (ans < 0) {
		//std::cout << "The gpio pin already exists?" << std::endl;
	}
	close(master_fd);

	m_gpio_fd = open((GPIO_PATH + "gpio" + pin_num_ss.str() + "/direction").c_str(), O_WRONLY);

	if (m_gpio_fd < 0) {
		perror("");
		throw SensorException("Could not open specific gpio file");
	}


	*this = m_curr_state;
}

GpioPin::~GpioPin() {
	close(m_gpio_fd);
}

void GpioPin::operator = (bool state) {
	m_curr_state = state;
	int ans = write(m_gpio_fd, m_curr_state? "high":"low", m_curr_state? 4:3);
	if (ans < 0) {
		perror("");
		std::cout << "ANS is " << ans << std::endl;
		throw SensorException("Could not write to specific gpio file");
	}

}

GpioPin::operator bool () {
	return m_curr_state;
}


}  // namespace autopilot

