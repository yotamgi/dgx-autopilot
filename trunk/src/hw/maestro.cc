#include "maestro.h"

#include <stdlib.h>
#include <unistd.h>  // UNIX standard function definitions
#include <fcntl.h>   // File control definitions
#include <termios.h> // POSIX terminal control definitions

Maestro::Maestro(std::string port_name):m_port_name(port_name) {

	// open the file
	m_device_fd = open(m_port_name.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);

	if (m_device_fd == -1) {  // Could not open the port
		throw(HwExcetion(std::string("Could not open port ") + m_port_name));
	}
	std::cout << "Opened port " << m_port_name << std::endl;

	configure_port();

	// Create the data channels:
	for (int i=0; i<6; i++) {
		m_channels.push_back(
				boost::shared_ptr<MaestroServoChannel>(new MaestroServoChannel(i, m_device_fd))
		);
	}

	usleep(1000);
}

void Maestro::configure_port() {
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

Maestro::~Maestro() {
	   // Close the serial port
	if (close(m_device_fd) == -1) {
		throw(HwExcetion(std::string("Could not close port ") + m_port_name));
	}
	std::cout << "Closed port " << m_port_name << std::endl;
}

Maestro::MaestroServoChannel::MaestroServoChannel(size_t channel, int device_fd):
	m_channel(channel),
	m_device_fd(device_fd),
	SERVO_MIN(4000),
	SERVO_MAX(8000)
{}

void Maestro::MaestroServoChannel::set_state(float statePercentage) {

	if ((statePercentage < 0.0f) || (statePercentage > 100.0f)) {
		throw ServoException("You asked for illegal servo percentage");
	}

	unsigned char serialBytes[5];
	int target = SERVO_MIN + (statePercentage/100.0f) * float(SERVO_MAX-SERVO_MIN);

	int a;

	serialBytes[0] = 0x40; // Unknown command.
	serialBytes[1] = 0x84; // Command byte: Set Target.
	serialBytes[2] = m_channel; // First data byte holds channel number.
	serialBytes[3] = 0x7f&target; // Second byte holds the lower 7 bits of target.
	serialBytes[4] = 0x7f&(target>>7);   // Third data byte holds the bits 7-13 of target.

	a = write(m_device_fd,serialBytes,5);
	if (a != 5) {
		throw ServoException("Could not write to file");
	}
}
