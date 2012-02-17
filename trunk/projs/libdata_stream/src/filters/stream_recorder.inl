#include "stream_recorder.h"
#include <boost/shared_ptr.hpp>

namespace stream {
namespace filters {

template <typename T>
inline RecorderFunctor<T>::RecorderFunctor(std::ostream& out):m_out(out)
{
	// create a little header
	out << "STREAM" << std::endl;
	out << typeid(T).name() << std::endl;
}

template <typename T>
inline T RecorderFunctor<T>::operator() (const T& data) {
	m_out << m_timer.passed() << "," << data << std::endl;
	return data;
}

template <typename data_t>
RecorderPopFilter<data_t>::RecorderPopFilter(std::ostream& out,
									 boost::shared_ptr<DataPopStream<data_t> > generator):
				FuncPopFilter<data_t>(generator, RecorderFunctor<data_t>(out))
{}


}  // namespace filter
}  // namespace stream

