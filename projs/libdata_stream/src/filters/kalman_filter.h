/*
 * kalman_filter.h
 *
 *  Created on: Oct 8, 2011
 *      Author: yotam
 */

#ifndef KALMAN_FILTER_H_
#define KALMAN_FILTER_H_

#include <stream/generators.h>
#include <boost/shared_ptr.hpp>

namespace stream {
namespace filters {

template <typename data_t>
class KalmanFilter : public DataGenerator<data_t> {
public:
	KalmanFilter(boost::shared_ptr<DataGenerator<data_t> > predict,
			boost::shared_ptr<DataGenerator<data_t> > obserbed,
			boost::shared_ptr<DataGenerator<float> > reliable);

	data_t get_data();

private:
	boost::shared_ptr<DataGenerator<data_t> > m_predict;
	boost::shared_ptr<DataGenerator<data_t> > m_obserbed;
	boost::shared_ptr<DataGenerator<float> >  m_reliable;

	data_t m_state;
};

} // namespace filters
} // namespace stream


#endif /* KALMAN_FILTER_H_ */
