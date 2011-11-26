#ifndef STATIC_FILTER_H_
#define STATIC_FILTER_H_

#include "stream/data_filter.h"
#include "stream/util/lin_algebra.h"

namespace stream {
namespace filters {

template <typename T>
class StaticFilter : public StreamPopFilter<T> {
public:

	StaticFilter(boost::shared_ptr<DataPopStream<T> > gen):
		StreamPopFilter<T>(gen)
	{
		m_add[0] = 0.; m_add[1] = 0.; m_add[2]=0.;
	}

	StaticFilter(boost::shared_ptr<DataPopStream<T> > gen,
			const typename lin_algebra::vec3f& add):
		StreamPopFilter<T>(gen),
		m_add(add)
	{}

	lin_algebra::vec3f get_data(){
		lin_algebra::vec3f data = StreamPopFilter<T>::m_generator->get_data();

		data = data + m_add;

		return data;
	}

private:

	lin_algebra::vec3f m_add;
};

} // namespace filters
} // namespace stream

#endif /* STATIC_FILTER_H_ */

