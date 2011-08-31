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

#include "generators.h"
#include "util/time.h"

/*
 * TODO:
 *  - seperate to inl
 */
template <typename T, size_t N>
class IntergralFilter : public VecFilter<T,N> {
public:

	IntergralFilter(VecGenerator<T,N>* data_gen,
			float up_limit = std::numeric_limits<float>::max(),
			float down_limit = std::numeric_limits<float>::min()):
		VecFilter<T,N>(data_gen),
		m_up_limit(up_limit),
		m_down_limit(down_limit),
		m_prev_time(get_curr_time())
	{
		m_sum[0] = 0.; m_sum[1] =  0.; m_sum[2] =  0.;
	}

	virtual ~IntergralFilter() {}

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
	float m_up_limit;
	float m_down_limit;
	double m_prev_time;
};

#endif /* DATA_FILTERS_H_ */
