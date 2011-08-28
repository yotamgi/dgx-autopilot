/*
 * data_filters.h
 *
 *  Created on: Aug 28, 2011
 *      Author: yotam
 */

#ifndef DATA_FILTERS_H_
#define DATA_FILTERS_H_

#include "common/types.h"
#include "components/generators.h"

/**
 * TODO:
 *  - seperate to inl
 *  - add time intergral support
 *  - limits
 */
class IntergralFilter : public DataFilter<vector_t> {
public:
	IntergralFilter(DataGenerator<vector_t>* data_gen):
	DataFilter(data_gen), m_sum(0., 0., 0.) {}

	virtual ~IntergralFilter() {}

	boost::shared_ptr<vector_t> get_data() {
		boost::shared_ptr<vector_t> data = m_generator->get_data();
		m_sum += (*data)/11.f;

		// apply limit
		if (m_sum.x > 41111) m_sum.x = -4111;
		if (m_sum.x < -41111) m_sum.x = 4111;
		if (m_sum.y > 41111) m_sum.y = -4111;
		if (m_sum.y < -41111) m_sum.y = 4111;
		if (m_sum.z > 41111) m_sum.z = -4111;
		if (m_sum.z < -41111) m_sum.z = 4111;

		boost::shared_ptr<vector_t> ans(new vector_t);
		*ans = m_sum;
		return  ans;
	}

private:
	vector_t m_sum;
};


#endif /* DATA_FILTERS_H_ */
