/*
 * kalman_filter.h
 *
 *  Created on: Oct 8, 2011
 *      Author: yotam
 */

#ifndef KALMAN_FILTER_H_
#define KALMAN_FILTER_H_

#include <stream/generators.h>

namespace stream {
namespace filters {

template <typename data_t>
class KalmanFilter : public DataGenerator<data_t> {
public:
	KalmanFilter(DataGenerator<data_t>* predict,
				 DataGenerator<data_t>* obserbed,
				 DataGenerator<float>* reliable);

	data_t get_data();

private:
	DataGenerator<data_t>* m_predict;
	DataGenerator<data_t>* m_obserbed;
	DataGenerator<float>*  m_reliable;

	data_t m_state;
};

} // namespace filters
} // namespace stream


#endif /* KALMAN_FILTER_H_ */
