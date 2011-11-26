
#ifndef LOW_PASS_FILTER_H_
#define LOW_PASS_FILTER_H_

#include <stream/data_filter.h>
#include <boost/circular_buffer.hpp>

namespace stream {
namespace filters {

template <typename data_t>
class LowPassFilter : public StreamPopFilter<data_t> {
public:
	LowPassFilter(boost::shared_ptr<DataPopStream<data_t> > data_gen, size_t filt);

	data_t get_data();

private:

	data_t m_sum;
	boost::circular_buffer<data_t> m_samples;
	const size_t m_size;

	/* Variables for the sum resetting */
	const size_t m_max;
	size_t m_left_to_reset;
};

}  // namespace filter
}  // namespace stream


#include "low_pass_filter.inl"

#endif /* LOW_PASS_FILTER_H_ */
