#ifndef EXCEPTIONS_H_
#define EXCEPTIONS_H_

#include <stream/data_pop_stream.h>
#include <string>

class SensorException : public stream::StreamException {
public:
	SensorException(std::string what): stream::StreamException(what) {}
	~SensorException() throw() {}
};

#endif /* EXCEPTIONS_H_ */
