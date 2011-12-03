#ifndef STREAM_EXCEPTION_H_
#define STREAM_EXCEPTION_H_

#include <stdexcept>
#include <string>

namespace stream {

class StreamException: public std::runtime_error {
public:
	StreamException(std::string what): runtime_error(what) {}
	virtual ~StreamException() throw() {}
};

}  // namespace stream

#endif /* STREAM_EXCEPTION_H_ */
