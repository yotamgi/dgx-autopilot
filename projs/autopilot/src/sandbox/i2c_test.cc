#include <iostream>
#include <hw/i2c_interface.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <stdio.h>

void test_write_num() {
	int fd = open("/tmp/stamfile", O_RDWR);
	I2C_Interface i2c(fd);
	i2c.write_num<uint8_t>(0xaa, 0xbb);

	char data[4];

	fd = open("/tmp/stamfile", O_RDWR);
	assert(read(fd, data, 4) == 2);
	assert(std::string(data, 2) == std::string("\xbb\xaa"));

	close(fd);
	std::cout << "WRITE TEST PASSED" << std::endl;
}

void test_read_num() {
	// prepare the file
	int fd = open("/tmp/stamfile", O_RDWR);
	char data = '\xcc';
	write(fd, &data, 1);

	// read data
	fd = open("/tmp/stamfile", O_RDWR);
	I2C_Interface i2c(fd);
	char ans = i2c.read_num<char>(0xdd);
	std::cout << std::hex << (unsigned int)(ans) << std::endl;
	assert(ans == '\xcc');

	// verify read
	char filedata[4];
	fd = open("/tmp/stamfile", O_RDWR);
	assert(read(fd, filedata, 4) == 2);
	assert(std::string(data, 2) == std::string("\xcc\xd"));

	close(fd);
	std::cout << "READ TEST PASSED" << std::endl;
}

int main() {
	test_read_num();
	test_write_num();
}
