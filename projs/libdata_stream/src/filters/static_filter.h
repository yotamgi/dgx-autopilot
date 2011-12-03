#ifndef STATIC_FILTER_H_
#define STATIC_FILTER_H_

#include "stream/data_filter.h"
#include "stream/util/lin_algebra.h"

namespace stream {
namespace filters {

template <typename T>
class StaticFilter : public StreamFilter<T> {
public:

	StaticFilter(boost::shared_ptr<DataPopStream<T> > gen,
			const T& add):
		StreamFilter<T>(gen),
		m_add(add)
	{}

	StaticFilter(boost::shared_ptr<DataPushStream<T> > col,
			const T& add):
		StreamFilter<T>(col),
		m_add(add)
	{}


	T get_data(){
		return filter(StreamPopFilter<T>::m_generator->get_data());
	}

	void set_data(const T& data) {
		StreamPushFilter<T>::m_collector->set_data(filter(data));
	}

private:

	T filter(T data) {
		return data + m_add;
	}

	T m_add;
};

} // namespace filters
} // namespace stream

#endif /* STATIC_FILTER_H_ */

