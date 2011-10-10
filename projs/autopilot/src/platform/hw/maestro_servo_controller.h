#ifndef __MAESTRO_H__
#define __MAESTRO_H__

#include <stdint.h>
#include <iostream>
#include <vector>
#include <string>
#include <interfaces/servo.h>
#include <common/exceptions.h>
#include <boost/shared_ptr.hpp>

/**
 * Class representing the Pololu MaestroServoController 6 channel USB servo controller, and
 * contains 6 servo channels that implement the Servo interface.
 */
class MaestroServoController {
public:
	/**
	 * Inits everyting.
	 * @throws HwException if anything failes
	 */
	MaestroServoController(std::string _port_name);

	~MaestroServoController();

	/**
	 * The servo channel class, that implements the Servo interface.
	 */
	class MaestroServoChannel : public Servo {
	public:
		MaestroServoChannel(size_t channel, MaestroServoController& father);
		void set_state(float statePercentage);
	private:
		size_t m_channel;
		MaestroServoController& m_father;

		const int SERVO_MIN;
		const int SERVO_MAX;
	};

	/**
	 * Returns the servo channel you asked.
	 * This is the only way to control the servos
	 */
	boost::shared_ptr<MaestroServoChannel> getServoChannel(size_t channel)
					{ return m_channels.at(channel); }

	/**
	 * Checks the error register of the device, and informs if there is a
	 * problem.
	 * @throws ServoException if there is a problem
	 */
	void check_errors() const;

private:
	/**
	 * Takes the fd and configures it with the termios stuff.
	 */
	void configure_port();

	std::vector<boost::shared_ptr<MaestroServoChannel> > m_channels;
	
	/** The main device file descryptor */
	int m_device_fd;

	const std::string m_port_name;

	/**
	 * Opcodes for the communication
	 */
	static const uint8_t MAGIC_OPCODE 		= 0x40;
	static const uint8_t SET_STATE_OPCODE 	= 0x84;
	static const uint8_t GET_ERROR_OPCODE 	= 0xA1;

};

#endif // __MAESTRO_H__