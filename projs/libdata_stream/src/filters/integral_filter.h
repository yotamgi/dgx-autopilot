/*
 * data_filters.h
 *
 *  Created on: Aug 28, 2011
 *      Author: yotam
 */

#ifndef DATA_FILTERS_H_
#define DATA_FILTERS_H_

#include <limits>
#include <sys/time.h>

#include <boost/array.hpp>

#include "stream/data_filter.h"
#include "stream/util/time.h"
#include <boost/function.hpp>
#include <functional>

namespace stream {
namespace filters {

template <typename data_t>
class IntegralFilter : public StreamFilter<data_t> {
public:
	IntegralFilter(boost::shared_ptr<DataPopStream<data_t> > data_gen,
			data_t start_val,
			boost::function<data_t(data_t, data_t)> apply =  std::plus<data_t>()):
		StreamFilter<data_t>(data_gen),
		m_state(start_val),
		m_apply(apply),
		m_prev_time(get_curr_time())
	{}

	IntegralFilter(boost::shared_ptr<DataPushStream<data_t> > data_col,
			data_t start_val,
			boost::function<data_t(data_t, data_t)> apply =  std::plus<data_t>()):
		StreamFilter<data_t>(data_col),
		m_state(start_val),
		m_apply(apply),
		m_prev_time(get_curr_time())
	{}


	data_t get_data() {
		return filter(StreamPopFilter<data_t>::m_generator->get_data());
	}
	void set_data(data_t data) {
		StreamPushFilter<data_t>::m_collector->set_data(filter(data));
	}


private:

	data_t filter(data_t data) {
		double curr_time = get_curr_time();
		double time_delta = curr_time - m_prev_time;
		m_prev_time = curr_time;

		m_state = m_apply(m_state, time_delta * data);
		return m_state;
	}

	data_t m_state;
	boost::function<data_t(data_t, data_t)> m_apply;
	double m_prev_time;

};

} // namespace filters
} // namespace stream

#endif /* DATA_FILTERS_H_ */
