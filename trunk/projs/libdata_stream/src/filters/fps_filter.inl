#include "fps_filter.h"


namespace stream {
namespace filters {


template <typename data_t>
FpsFilter<data_t>::FpsFilter(boost::shared_ptr<DataPopStream<data_t> > source):
		StreamPopFilter<data_t>(source),
		m_counter(0),
		m_fps_gen(new typename FpsFilter<data_t>::FpsGen(this))
{}

template <typename data_t>
FpsFilter<data_t>::~FpsFilter() {
	m_fps_gen->invalidate();
}

template <typename data_t>
data_t FpsFilter<data_t>::get_data() {
	m_counter++;
	return StreamPopFilter<data_t>::m_generator->get_data();
}

template <typename data_t>
boost::shared_ptr<DataPopStream<float> > FpsFilter<data_t>::get_fps_stream() {
	return m_fps_gen;
}

template <typename data_t>
float FpsFilter<data_t>::calc_fps() {
	float fps = (m_counter/m_timer.passed());
	m_counter = 0;
	m_timer.reset();
	return fps;
}

template <typename data_t>
FpsFilter<data_t>::FpsGen::FpsGen(FpsFilter<data_t>* father):m_father(father)
{};

template <typename data_t>
float FpsFilter<data_t>::FpsGen::get_data() {
	if (!m_father) {
		throw std::runtime_error("Tried to ask FPS from an non existing stream");
	}
	return m_father->calc_fps();
}

template <typename data_t>
void FpsFilter<data_t>::FpsGen::invalidate() {
	m_father = NULL;
}


} // namespace filters
} // namespace stream

