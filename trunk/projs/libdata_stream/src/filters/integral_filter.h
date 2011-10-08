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
class IntegralFilter : public DataFilter<data_t> {
public:
	IntegralFilter(DataGenerator<data_t>* data_gen,
			data_t start_val,
			boost::function<data_t(data_t, data_t)> apply =  std::plus<float>()):
		DataFilter<data_t>(data_gen),
		m_state(start_val),
		m_apply(apply),
		m_prev_time(get_curr_time())
	{}

	data_t get_data() {

		// integrate the data with time
		double curr_time = get_curr_time();
		double time_delta = curr_time - m_prev_time;
		m_prev_time = curr_time;

		data_t change = DataFilter<data_t>::m_generator->get_data();
		m_state = m_apply(m_state, time_delta * change);
		return m_state;
	}

private:

	data_t m_state;
	boost::function<data_t(data_t, data_t)> m_apply;
	double m_prev_time;

};


template <typename T, size_t N>
class VecIntegralFilter : public VecFilter<T,N> {
public:

	VecIntegralFilter(VecGenerator<T,N>* data_gen,
			T up_limit = std::numeric_limits<T>::max(),
			T down_limit = std::numeric_limits<T>::min()):
		VecFilter<T,N>(data_gen),
		m_up_limit(up_limit),
		m_down_limit(down_limit),
		m_prev_time(get_curr_time())
	{
		for (size_t i=0; i<N; i++) m_sum[i] = 0;
	}

	virtual ~VecIntegralFilter() {}

	typename VecFilter<T,N>::vector_t get_data() {
		typename VecFilter<T,N>::vector_t data =
				VecFilter<T,N>::m_generator->get_data();

		// integrate the data with time
		double curr_time = get_curr_time();
		double time_delta = curr_time - m_prev_time;
		m_prev_time = curr_time;

		for (size_t i=0; i != data.size(); i++) {

			// integrate
			m_sum[i] += data[i] * time_delta;

			// limit
			if (m_sum[i] > m_up_limit) {
				m_sum[i] = m_down_limit;
			} else if (m_sum[i] < m_down_limit) {
				m_sum[i] = m_up_limit;
			}
		}

		return  m_sum;
	}

private:
	typename VecFilter<T,N>::vector_t m_sum;
	T m_up_limit;
	T m_down_limit;
	double m_prev_time;
};

} // namespace filters
} // namespace stream

#endif /* DATA_FILTERS_H_ */
