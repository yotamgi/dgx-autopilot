#include "low_pass_filter.h"
#include <algorithm>
#include <numeric>

namespace stream {
namespace filters {

template <typename data_t>
LowPassFilter<data_t>::LowPassFilter(
		boost::shared_ptr<DataGenerator<data_t> > data_gen,
		size_t filt):
	DataFilter<data_t>(data_gen),
	m_samples(filt),
	m_size(filt),
	m_max(1000),
	m_left_to_reset(m_max)
{
	// start everythig with initial value
	data_t dat = DataFilter<data_t>::m_generator->get_data();
	m_samples.push_back(dat);
	m_sum = dat;

	for (size_t i=0; i<m_size-1; i++) {
		data_t dat = DataFilter<data_t>::m_generator->get_data();
		m_sum += dat;
		m_samples.push_back(dat);
	}
}

template <typename data_t>
data_t LowPassFilter<data_t>::get_data() {
	data_t dat = DataFilter<data_t>::m_generator->get_data();
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

template <typename T, size_t N>
LowPassVecFilter<T,N>::LowPassVecFilter(
		boost::shared_ptr<VecGenerator<T,N> > vec_gen,
		size_t filt):
	VecFilter<T,N>(vec_gen),
	m_samples(filt),
	m_size(filt),
	m_max(1000),
	m_left_to_reset(m_max)
{
	// start everythig with initial value
	typename VecFilter<T,N>::vector_t vec = VecFilter<T,N>::m_generator->get_data();
	m_samples.push_back(vec);
	m_sum = vec;

	for (size_t i=0; i<m_size-1; i++) {
		typename VecFilter<T,N>::vector_t vec = VecFilter<T,N>::m_generator->get_data();

		for (size_t i=0; i<N; i++ )
			m_sum[i] += vec[i];

		m_samples.push_back(vec);
	}

}

template <typename T, size_t N>
typename VecFilter<T,N>::vector_t LowPassVecFilter<T,N>::get_data() {
	typename VecFilter<T,N>::vector_t vec = VecFilter<T,N>::m_generator->get_data();

	for (size_t i=0; i<N; i++ ) {
		m_sum[i] += vec[i];
		m_sum[i] -= m_samples.front()[i];
	}
	m_samples.push_back(vec);

	// update sum every m_max times
	if (m_left_to_reset == 0) {
		m_sum = *m_samples.begin();

		m_sum = m_samples[0];
		for (size_t i=1; i<m_samples.size(); i++) {
			for (size_t j=0; j<N; j++) {
				m_sum[j] += m_samples[i][j];
			}
		}
		m_left_to_reset = m_max;
	} else {
		m_left_to_reset--;
	}

	typename VecFilter<T,N>::vector_t ans;
	for (size_t i=0; i<N; i++ ) {
		ans[i] = m_sum[i]/m_size;
	}
	return ans;
}


}  // namespace filter
}  // namespace stream
