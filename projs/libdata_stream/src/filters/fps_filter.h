#ifndef FPS_FILTER_H_
#define FPS_FILTER_H_

#include <stream/data_filter.h>
#include <stream/util/time.h>
#include <stdexcept>

namespace stream {
namespace filters {


/**
 * Fps pop stream filter
 * Calcs the rate of access to a given stream, without affecting it at all.
 */
template <typename data_t>
class FpsFilter : public StreamPopFilter<data_t> {
public:
	FpsFilter(boost::shared_ptr<DataPopStream<data_t> > source, float min_calc_time = 0.05);
	~FpsFilter();

	data_t get_data();

	boost::shared_ptr<DataPopStream<float> > get_fps_stream();

private:

	/**
	 * Fps generator.
	 * The class that will return the FPS. It will be returned from get_fps_stream,
	 */
	class FpsGen : public DataPopStream<float> {
	public:
		FpsGen(FpsFilter<data_t>* father);

		float get_data();

		void invalidate();

	private:
		FpsFilter<data_t>* m_father;
	};

	float calc_fps();

	size_t m_counter;
	Timer m_timer;
	boost::shared_ptr<FpsGen> m_fps_gen;

	const float m_min_calc_time;
	float m_last_value;

	friend class FpsGen;
};


} // namespace filters
} // namespace stream

#include "fps_filter.inl"

#endif /* FPS_FILTER_H_ */
