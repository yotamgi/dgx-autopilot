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
#include "util/time.h"

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
		m_down_limit(down_limit),
		m_prev_time(get_curr_time())
	{
	}

	virtual ~IntergralFilter() {}

	boost::shared_ptr<vector_t> get_data() {
		boost::shared_ptr<vector_t> data = m_generator->get_data();

		// integrate the data with time
		double curr_time = get_curr_time();
		double time_delta = curr_time - m_prev_time;
		m_prev_time = curr_time;

		m_sum += (*data) * time_delta;

		// apply limits
		if (m_sum.x > m_up_limit) 	m_sum.x = m_down_limit;
		if (m_sum.x < m_down_limit) m_sum.x = m_up_limit;
		if (m_sum.y > m_up_limit) 	m_sum.y = m_down_limit;
		if (m_sum.y < m_down_limit) m_sum.y = m_up_limit;
		if (m_sum.z > m_up_limit) 	m_sum.z = m_down_limit;
		if (m_sum.z < m_down_limit) m_sum.z = m_up_limit;

		*data = m_sum;

		return  data;
	}

private:
	vector_t m_sum;
	float m_up_limit;
	float m_down_limit;
	double m_prev_time;
};

#endif /* DATA_FILTERS_H_ */
