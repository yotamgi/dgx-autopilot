#ifndef EXCEPTIONS_H_
#define EXCEPTIONS_H_

#include <stdexcept>
#include <string>

class HwExcetion : public std::runtime_error {
public:
	HwExcetion(std::string _what): runtime_error(_what) {}
};


#endif /* EXCEPTIONS_H_ */
