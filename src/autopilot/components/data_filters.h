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
#include "common/types.h"
#include "components/generators.h"

/*
 * TODO:
 *  - seperate to inl
 */
class IntergralFilter : public DataFilter<vector_t> {
public:
	IntergralFilter(DataGenerator<vector_t>* data_gen,
			float up_limit = std::numeric_limits<float>::max(),
			float down_limit = std::numeric_limits<float>::min()):
		DataFilter(data_gen),
		m_sum(0., 0., 0.),
		m_up_limit(up_limit),
		m_down_limit(down_limit)
	{
		gettimeofday(&m_prev_time, NULL);
	}

	virtual ~IntergralFilter() {}

	boost::shared_ptr<vector_t> get_data() {
		boost::shared_ptr<vector_t> data = m_generator->get_data();

		// integrate the data with time
		timeval curr_time;
		gettimeofday(&curr_time, NULL);
		m_sum += (*data)*(curr_time.tv_usec - m_prev_time.tv_usec);
		m_prev_time = curr_time;

		// apply limits
		if (m_sum.x > m_up_limit) 	m_sum.x = m_down_limit;
		if (m_sum.x < m_down_limit) m_sum.x = m_up_limit;
		if (m_sum.y > m_up_limit) 	m_sum.y = m_down_limit;
		if (m_sum.y < m_down_limit) m_sum.y = m_up_limit;
		if (m_sum.z > m_up_limit) 	m_sum.z = m_down_limit;
		if (m_sum.z < m_down_limit) m_sum.z = m_up_limit;

		boost::shared_ptr<vector_t> ans(new vector_t);
		*ans = m_sum;
		return  ans;
	}

private:
	vector_t m_sum;
	float m_up_limit;
	float m_down_limit;
	timeval m_prev_time;
};

#endif /* DATA_FILTERS_H_ */