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
class IntegralFilter : public StreamPopFilter<data_t> {
public:
	IntegralFilter(boost::shared_ptr<DataPopStream<data_t> > data_gen,
			data_t start_val,
			boost::function<data_t(data_t, data_t)> apply =  std::plus<data_t>()):
		StreamPopFilter<data_t>(data_gen),
		m_state(start_val),
		m_apply(apply),
		m_prev_time(get_curr_time())
	{}

	data_t get_data() {

		// integrate the data with time
		double curr_time = get_curr_time();
		double time_delta = curr_time - m_prev_time;
		m_prev_time = curr_time;

		data_t change = StreamPopFilter<data_t>::m_generator->get_data();
		m_state = m_apply(m_state, time_delta * change);
		return m_state;
	}

private:

	data_t m_state;
	boost::function<data_t(data_t, data_t)> m_apply;
	double m_prev_time;

};

} // namespace filters
} // namespace stream

#endif /* DATA_FILTERS_H_ */
