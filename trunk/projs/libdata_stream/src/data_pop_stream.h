#ifndef GANERATOR_H_
#define GANERATOR_H_

#include <ostream>
#include <istream>
#include <iterator>

namespace stream {

/**
 * Represents a any type of data generator. It may be a sensor, or a combination
 * of some.
 */
template <typename T>
class DataPopStream {
public:

	typedef T type;

	virtual ~DataPopStream() {}

	/**
	 * Suppose to return the current value/sample of the data.
	 */
	virtual T get_data() = 0;

};

} // namespace stream

#endif /* GANERATOR_H_ */
