#ifndef BEAGLEBOARD_GPIO_H_
#define BEAGLEBOARD_GPIO_H_

#include "common/exceptions.h"

namespace autopilot {

class GpioPin {
public:

	GpioPin(unsigned int pin_num);
	~GpioPin();

	void operator = (bool state);

	operator bool ();

private:
	int m_gpio_fd;
	bool m_curr_state;
};

}  // namespace autopilot

#endif /* BEAGLEBOARD_GPIO_H_ */
