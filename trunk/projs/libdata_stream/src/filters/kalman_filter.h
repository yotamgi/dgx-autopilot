/*
 * kalman_filter.h
 *
 *  Created on: Oct 8, 2011
 *      Author: yotam
 */

#ifndef KALMAN_FILTER_H_
#define KALMAN_FILTER_H_

#include <stream/generators.h>
#include <stream/util/time.h>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

namespace stream {
namespace filters {

template <typename data_t>
class KalmanFilter : public DataGenerator<data_t> {
public:
	KalmanFilter(boost::shared_ptr<DataGenerator<data_t> > predict,
			boost::shared_ptr<DataGenerator<data_t> > obserbed,
			boost::shared_ptr<DataGenerator<float> > reliable,
			data_t inital_val,
			boost::function<data_t(data_t, data_t)> apply =  std::plus<data_t>());

	data_t get_data();

private:
	boost::shared_ptr<DataGenerator<data_t> > m_predict;
	boost::shared_ptr<DataGenerator<data_t> > m_obserbed;
	boost::shared_ptr<DataGenerator<float> >  m_reliable;

	boost::function<data_t(data_t, data_t)> m_apply;

	data_t m_state;

	//float m_prev_time;
	Timer m_timer;
	Timer m_fps_timer;
	
	size_t m_fps;
};

} // namespace filters
} // namespace stream


#include "kalman_filter.inl"

#endif /* KALMAN_FILTER_H_ */
