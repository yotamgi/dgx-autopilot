#ifndef EXCEPTIONS_H_
#define EXCEPTIONS_H_

#include <stdexcept>
#include <string>

class HwExcetion : public std::runtime_error {
public:
	HwExcetion(std::string _what): runtime_error(_what) {}
};

/**
 * Thrown when a ground station or any other units does not wait for
 * connection.
 */
class HostException : public std::runtime_error {
public:
	HostException(std::string _what): runtime_error(_what) {}
};


#endif /* EXCEPTIONS_H_ */
