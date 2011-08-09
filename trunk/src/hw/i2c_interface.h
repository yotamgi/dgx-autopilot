#ifndef I2C_H_
#define I2C_H_

#include "common/exceptions.h"
#include <stdint.h>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


/**
 * Helpful class for dealing with i2c devices.
 */
class I2C_Interface {
public:

	I2C_Interface(size_t device_num, uint8_t device_i2c_add);
	I2C_Interface(std::string device_filename, uint8_t device_i2c_add);

	void write_buff(const std::string& data, uint8_t reg) const {
		std::string packet = std::string(1, char(reg)) + data;
		raw_write(packet.c_str(), packet.size());
	}

	template <typename T>
	void write_num(const T& num, uint8_t reg) {
		char packet[sizeof(T)+1];
		char* pnum = reinterpret_cast<char*>(&num);

		packet[0] = (char)reg;

		// simple memcpy
		for (int i=0; i<sizeof(T); i++) {
			packet[i+1] = pnum[i];
		}
		raw_write(packet, sizeof(T)+1);
	}

	std::string read_buff(uint8_t reg, const size_t& length) const {
		// read from it
		char data[length];
		raw_read(data, length, reg);
		return std::string(data, length);
	}

	template <typename T>
	T read_num(uint8_t reg) const {
		T data;
		raw_read(reinterpret_cast<char*>(&data), sizeof(T), reg);
		return data;
	}

private:

	/**
	 * Writes raw data to the device.
	 * NOTE: Packet array should contains already the reg as first byte.
	 * @throws HwException if fails to write.
	 */
	void raw_write(const char* packet, const size_t& size) const {

		size_t count = write(m_i2c_device_fd, packet, size);

		if (count != size) {
			throw HwExcetion("Could not write to i2c device");
		}
	}


	/**
	 * Reads raw data from the device.
	 * @throws HwException if fails to write.
	 */
	void raw_read(char* data, const size_t& size, uint8_t reg) const {
		// peak the register
		raw_write((char*)&reg, 1);

		// read from it
		size_t count = read(m_i2c_device_fd, data, size);
		if (count != size) {
			throw HwExcetion("Could not read from i2c device");
		}
	}

	void init(std::string device_name, uint8_t device_i2c_add);

	int m_i2c_device_fd;
};

#endif /* I2C_H_ */
