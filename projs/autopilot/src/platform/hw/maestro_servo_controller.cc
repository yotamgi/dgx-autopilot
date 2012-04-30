#include "maestro_servo_controller.h"

#include <sstream>
#include <stdlib.h>
#include <unistd.h>  // UNIX standard function definitions
#include <fcntl.h>   // File control definitions
#include <termios.h> // POSIX terminal control definitions

namespace autopilot {


MaestroServoController::MaestroServoController(std::string port_name):m_port_name(port_name) {

	// open the file
	m_device_fd = open(m_port_name.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);

	if (m_device_fd == -1) {  // Could not open the port
		throw(SensorException("Could not open port " + m_port_name + ". is device connected?"));
	}
	std::cout << "Opened port " << m_port_name << std::endl;

	configure_port();

	// Create the data channels:
	for (int i=0; i<6; i++) {
		m_channels.push_back(
				boost::shared_ptr<MaestroServoChannel>(new MaestroServoChannel(i, *this))
		);
	}

	usleep(1000);
}

void MaestroServoController::configure_port() {
   struct termios options;

   // Get the current settings of the serial port.
   tcgetattr (m_device_fd, &options);

   // Set the read and write speed to 19200 BAUD.
   // All speeds can be prefixed with B as a settings.
   cfsetispeed (&options, B9600);
   cfsetospeed (&options, B9600);

   // Now to set the other settings. Here we use the no parity example. Both will assumme 8-bit words.

   // PARENB is enabled parity bit. This disables the parity bit.
   options.c_cflag &= ~PARENB;

   // CSTOPB means 2 stop bits, otherwise (in this case) only one stop bit.
   options.c_cflag &= ~CSTOPB;

   // CSIZE is a mask for all the data size bits, so anding with the negation clears out the current data size setting.
   options.c_cflag &= ~CSIZE;

   // CS8 means 8-bits per work
   options.c_cflag |= CS8;
}

MaestroServoController::~MaestroServoController() {
	   // Close the serial port
	if (close(m_device_fd) == -1) {
		throw(SensorException(std::string("Could not close port ") + m_port_name));
	}
	std::cout << "Closed port " << m_port_name << std::endl;
}

void MaestroServoController::check_errors() const {

	// ask for error state
//	uint8_t packet = MaestroServoController::GET_ERROR_OPCODE;
//	size_t count = write(m_device_fd,&packet,1);
//	if (count != 1) {
//		throw ServoException("Could not write to device file");
//	}
//	
//	// check it
//	uint16_t error_reg;
//	count = read(m_device_fd, (uint8_t*)&error_reg, 2);
//	if (count != 2) {
//		std::cout << count << std::endl;
//		throw ServoException("Could not read from device file");
//	}
//	
//	if (error_reg != 0) {
//		std::stringstream ss;
//		ss << "There was an error in the device. The error register is " <<
//				std::hex << error_reg;
//		throw ServoException(ss.str());
//	}
}

MaestroServoController::MaestroServoChannel::MaestroServoChannel(size_t channel, MaestroServoController& father):
	m_channel(channel),
	m_father(father),
	SERVO_MIN(4000),
	SERVO_MAX(8000)
{}

void MaestroServoController::MaestroServoChannel::set_data(const float& state_percentage) {

	// apply the trims
	float trimmed_state_percentage = trim(state_percentage);

	// convert it to servo units
	int target = SERVO_MIN + (trimmed_state_percentage/100.0f) * float(SERVO_MAX-SERVO_MIN);

	// build the send packet
	unsigned char serialBytes[5];
	serialBytes[0] = MaestroServoController::MAGIC_OPCODE; // Unknown command.
	serialBytes[1] = MaestroServoController::SET_STATE_OPCODE; // Command byte: Set Target.
	serialBytes[2] = m_channel; // First data byte holds channel number.
	serialBytes[3] = 0x7f&target; // Second byte holds the lower 7 bits of target.
	serialBytes[4] = 0x7f&(target>>7);   // Third data byte holds the bits 7-13 of target.

	// send the packet
	int a;
	a = write(m_father.m_device_fd, serialBytes,5);
	if (a != 5) {
		throw ServoException("Could not write to device file");
	}

	m_father.check_errors();
}

}  // namespace autopilot
