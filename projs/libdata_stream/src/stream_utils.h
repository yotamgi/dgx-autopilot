#ifndef STREAM_UTILS_H_
#define STREAM_UTILS_H_

#include "data_pop_stream.h"
#include "data_push_stream.h"

namespace stream {

/**
 * Converts a push stream to a pop stream
 * Once you set data to be something, you can read the data
 */
template <typename T>
class PushToPopConv : public DataPushStream<T>,
					  public DataPopStream<T>
{
public:
	PushToPopConv(const T& initial): m_data(initial) {}

	void set_data(const T& data) { m_data = data; }
	T get_data() { return m_data; }

private:
	T m_data;
};

/**
 * A bypass PushStream forwarder class.
 * It only forwards the calls if there is a receiver stream. if not, it will do
 * nothing.
 */
template <typename T>
class PushForwarder : public DataPushStream<T>, public PushGenerator<T> {
public:

	void set_data(const T& data) { if (m_stream) m_stream->set_data(data); }

	void set_receiver(boost::shared_ptr<DataPushStream<T> > stream) { m_stream = stream; }

private:

	boost::shared_ptr<DataPushStream<T> > m_stream;
};

} // namespace stream

#endif /* STREAM_UTILS_H_ */
