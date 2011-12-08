#include "gps_filter.h"
#include <boost/make_shared.hpp>
#include <stream/util/time.h>

SimpleGpsFilter::SimpleGpsFilter(size_t avarge_len):
			m_avarge_len(avarge_len),
			m_samples(avarge_len),
			m_position_stream(
				boost::make_shared<stream::PushToPopConv<lin_algebra::vec3f> >(
					lin_algebra::vec3f()
				)
			),
			m_speed_stream(
				boost::make_shared<stream::PushToPopConv<lin_algebra::vec3f> >(
					lin_algebra::vec3f()
				)
			)
{}

void SimpleGpsFilter::set_data(const lin_algebra::vec3f& data) {
	m_samples.push_back(data);

	m_position_stream->set_data(data);
	m_speed_stream->set_data(data - m_samples.front());

//	// fill the data into a matrix
//	lin_algebra::Mat<float> m(3, m_samples.size());
//	for (size_t i=0; i<m_samples.size(); i++) {
//		m.col(i) = m_samples.at(i);
//	}
//	m = lin_algebra::trans(m);
//
//	lin_algebra::Mat<float> ce;
//	lin_algebra::princomp(ce, m);
//	std::cout << ce << std::endl;
//
//	lin_algebra::vec3f mean = lin_algebra::trans(lin_algebra::mean(m));

}

boost::shared_ptr<stream::DataPopStream<lin_algebra::vec3f> > SimpleGpsFilter::get_position_stream() {
	return m_position_stream;
}

boost::shared_ptr<stream::DataPopStream<lin_algebra::vec3f> > SimpleGpsFilter::get_speed_stream() {
	return m_speed_stream;
}
