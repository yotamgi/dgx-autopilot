#ifndef GENERATE_FILTER_H_
#define GENERATE_FILTER_H_

#include <stream/data_filter.h>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

namespace stream {

template <typename data_source_t, typename data_t = data_source_t>
class FuncPopFilter : public StreamPopFilter<data_source_t, data_t> {
public:
	FuncPopFilter(boost::shared_ptr<DataPopStream<data_source_t> > generator,
					   boost::function<data_t(data_source_t)> filter_func):
		StreamPopFilter<data_source_t, data_t>(generator),
		m_filter_func(filter_func)
	{}

	data_t get_data() {
		return m_filter_func(StreamPopFilter<data_source_t, data_t>::m_generator->get_data());
	}

private:

	boost::function<data_t(data_source_t)> m_filter_func;
};

template <typename data_source_t, typename data_t>
boost::shared_ptr<DataPopStream<data_t> > create_func_pop_filter(
							boost::shared_ptr<DataPopStream<data_source_t> > generator,
							boost::function<data_t(data_source_t)> filter_func)
{
	return boost::make_shared<FuncPopFilter<data_source_t, data_t> >(generator, filter_func);
}


} // namespace stream


#endif /* GENERATE_FILTER_H_ */
