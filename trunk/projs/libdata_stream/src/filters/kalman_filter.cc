#include "kalman_filter.h"


namespace stream {
namespace filters {

template <typename data_t>
KalmanFilter<data_t>::KalmanFilter(DataGenerator<data_t>* predict,
			 DataGenerator<data_t>* obserbed,
			 DataGenerator<float>* reliable):
	m_predict(predict),
	m_obserbed(obserbed),
	m_reliable(reliable)
{}

template <typename data_t>
data_t KalmanFilter<data_t>::get_data() {
	data_t predicted = m_predict->get_data();
	data_t observed  = m_obserbed->get_data();
	float  reliable  = m_reliable->get_data();

	m_state = (1.-reliable)*predicted + reliable*observed;

	return m_state;
}



} // namespace filters
} // namespace stream

