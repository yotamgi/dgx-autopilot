#include "low_pass_filter.h"
#include <algorithm>
#include <numeric>

namespace stream {
namespace filters {

template <typename data_t>
LowPassFilter<data_t>::LowPassFilter(
		boost::shared_ptr<DataPopStream<data_t> > data_gen,
		size_t filt):
	StreamFilter<data_t>(data_gen),
	m_samples(filt),
	m_size(filt),
	m_max(1000),
	m_left_to_reset(m_max)
{
	init();
}

template <typename data_t>
LowPassFilter<data_t>::LowPassFilter(
		boost::shared_ptr<DataPushStream<data_t> > data_col,
		size_t filt):
	StreamFilter<data_t>(data_col),
	m_samples(filt),
	m_size(filt),
	m_max(1000),
	m_left_to_reset(m_max)
{
	init();
}


template <typename data_t>
void LowPassFilter<data_t>::init() {
	// start everythig with initial value
	data_t dat = StreamPopFilter<data_t>::m_generator->get_data();
	m_samples.push_back(dat);
	m_sum = dat;

	for (size_t i=0; i<m_size-1; i++) {
		data_t dat = StreamPopFilter<data_t>::m_generator->get_data();
		m_sum += dat;
		m_samples.push_back(dat);
	}
}

template <typename data_t>
data_t LowPassFilter<data_t>::get_data() {
	data_t dat = StreamPopFilter<data_t>::m_generator->get_data();
	return filter(dat);
}

template <typename data_t>
void LowPassFilter<data_t>::set_data(data_t data) {
	StreamPushFilter<data_t>::m_collector->set_data(filter(data));
}

template <typename data_t>
data_t LowPassFilter<data_t>::filter(data_t dat) {
	m_sum += dat;
	m_sum -= m_samples.front();
	m_samples.push_back(dat);

	// update sum every m_max times
	if (m_left_to_reset == 0) {
		m_sum = *m_samples.begin();
		m_sum = std::accumulate(m_samples.begin()+1, m_samples.end(), m_sum);
		m_left_to_reset = m_max;
	} else {
		m_left_to_reset--;
	}

	return m_sum/m_size;

}


}  // namespace filter
}  // namespace stream
