#ifndef DATA_COLLECTOR_H_
#define DATA_COLLECTOR_H_

#include "stream_exception.h"
#include <boost/shared_ptr.hpp>

namespace stream {

class PushStreamException: public StreamException {
public:
	PushStreamException(std::string what): StreamException(what) {}
	virtual ~PushStreamException() throw() {}
};

/**
 * An interface for push kind streams
 */
template <typename T>
class DataPushStream {
public:
	virtual ~DataPushStream() {}

	typedef T data_t;

	/**
	 * The method for pushing the data to the stream
	 */
	virtual void set_data(const T& data) = 0;
};

/**
 * An interface for those who push data into a push stream
 */
template <typename T>
class PushGenerator {
public:
	virtual ~PushGenerator() {}

	virtual void register_receiver(boost::shared_ptr<DataPushStream<T> >) = 0;

	virtual void unregister_receiver(boost::shared_ptr<DataPushStream<T> >) = 0;

};

}  // namespace stream

#endif /* DATA_COLLECTOR_H_ */
