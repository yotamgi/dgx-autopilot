#ifndef STREAM_UTILS_H_
#define STREAM_UTILS_H_

#include "data_pop_stream.h"
#include "data_push_stream.h"
#include <vector>
#include <algorithm>
#include <boost/thread/mutex.hpp>

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

	void set_data(const T& data) {
		m_lock.lock();
		for (size_t i=0; i<m_recievers.size(); i++)
			m_recievers[i]->set_data(data);
		m_lock.unlock();
	}

	void register_receiver(boost::shared_ptr<DataPushStream<T> > stream) {
		m_lock.lock();
		if (std::find(m_recievers.begin(), m_recievers.end(), stream) == m_recievers.end()) {
			m_recievers.push_back(stream);
		}
		m_lock.unlock();
	}

	void unregister_receiver(boost::shared_ptr<DataPushStream<T> > stream) {
		m_lock.lock();
		if (std::find(m_recievers.begin(), m_recievers.end(), stream) != m_recievers.end()) {
			m_recievers.erase(std::find(m_recievers.begin(), m_recievers.end(), stream));
		}
		m_lock.unlock();
	}

private:

	std::vector<boost::shared_ptr<DataPushStream<T> > > m_recievers;
	boost::mutex m_lock;
};

} // namespace stream

#endif /* STREAM_UTILS_H_ */
