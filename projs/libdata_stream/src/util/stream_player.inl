#ifndef STREAM_PLAYER_INL_
#define STREAM_PLAYER_INL_

#include "stream_player.h"
#include <typeinfo>
#include <sstream>
#include <stdint.h>

namespace stream {

template <typename T>
StreamReader<T>::StreamReader(std::istream& in):m_in(in)
{
	// parse header
	std::string constant;
	m_in >> constant;
	if (constant != std::string("STREAM")) {
		throw PopStreamException("Could not parse the std::stream as a stream format");
	}

	std::string typeid_string;
	m_in >> typeid_string;
	if (typeid_string != typeid(T).name()) {
		std::stringstream error;
		error << "The stream is in not in the correct type (" << typeid_string << "!=" << typeid(T).name() << ")";
		throw PopStreamException(error.str());
	}
}

template <typename T>
typename StreamReader<T>::sample StreamReader<T>::next_sample() {
	typename StreamReader<T>::sample s;
	char c;
	m_in >> s.time;
	m_in >> c;
	if (c != ',') {
		std::stringstream error;
		error << "Could not parse the std::stream as a stream format - Expected ',' as seperator, but got " << c;
		throw PopStreamException(error.str());
	}
	m_in >> s.data;
	return s;
}

template <typename T>
inline PopStreamPlayer<T>::PopStreamPlayer(std::istream& in, bool blocking):
		m_reader(in), m_blocking(blocking)
{
	if (!blocking) {
		m_curr_sample.time = -1.;
	}
}

template <typename T>
inline T PopStreamPlayer<T>::get_data() {
	if (m_blocking) {
		typename StreamReader<T>::sample curr_sample = m_reader.next_sample();

		// block until the time comes
		while (curr_sample.time > m_timer.passed()) {
			usleep((uint32_t)((curr_sample.time - m_timer.passed()) * 1000000.));
		}

		return curr_sample.data;
	}
	else {
		// read from file untill we got to the correct time
		while (m_curr_sample.time < m_timer.passed()) {
			m_curr_sample = m_reader.next_sample();
		}

		// return the data
		return m_curr_sample.data;
	}
}

} // namespace stream

#endif /* STREAM_PLAYER_INL_ */