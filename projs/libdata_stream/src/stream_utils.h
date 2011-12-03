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

} // namespace stream

#endif /* STREAM_UTILS_H_ */
