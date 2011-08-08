#ifndef __MAESTRO_H__
#define __MAESTRO_H__

#include <iostream>
#include <vector>
#include <string>
#include <components/servo.h>
#include <common/exceptions.h>
#include <boost/shared_ptr.hpp>

/**
 * Class representing the Maestro servo controller, and contains 6 servo
 * channels that implement the Servo interface.
 */
class Maestro {
public:
	/**
	 * Inits everyting.
	 * @throws HwException if anything failes
	 */
	Maestro(std::string _port_name);

	~Maestro();

	// DATA TYPES
	class MaestroServoChannel : public Servo {
	public:
		MaestroServoChannel(size_t channel, int device_fd);
		void set_state(float statePercentage);
	private:
		int m_device_fd;
		size_t m_channel;

		const int SERVO_MIN;
		const int SERVO_MAX;
	};

	/**
	 * Returns the servo channel you asked.
	 * This is the only way to control the servos
	 */
	boost::shared_ptr<MaestroServoChannel> getServoChannel(size_t channel)
					{ return m_channels.at(channel); }

private:

	/**
	 * Takes the fd and configures it with the termios stuff.
	 */
	void configure_port();

	std::vector<boost::shared_ptr<MaestroServoChannel> > m_channels;
	
	/** The main device file descryptor */
	int m_device_fd;

	const std::string m_port_name;

};

#endif // __MAESTRO_H__
