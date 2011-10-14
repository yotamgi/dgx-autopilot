#include "kalman_filter.h"


namespace stream {
namespace filters {

template <typename data_t>
inline KalmanFilter<data_t>::KalmanFilter(boost::shared_ptr<DataGenerator<data_t> > predict,
		boost::shared_ptr<DataGenerator<data_t> > obserbed,
		boost::shared_ptr<DataGenerator<float> > reliable,
		data_t inital_val,
		boost::function<data_t(data_t, data_t)> apply):
	m_predict(predict),
	m_obserbed(obserbed),
	m_reliable(reliable),
	m_apply(apply),
	m_state(inital_val)
{}

template <typename data_t>
inline data_t KalmanFilter<data_t>::get_data() {
	data_t predicted = m_predict->get_data();
	data_t observed  = m_obserbed->get_data();
	float  reliable  = m_reliable->get_data();

	// calculate time delta
	float time_delta = m_timer.passed();
	m_timer.reset();

	m_state = m_state + reliable * (observed - m_state);
	m_state = (1-reliable)*m_apply(m_state, time_delta * predicted);

	return m_state;
}



} // namespace filters
} // namespace stream

