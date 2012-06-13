#ifndef STREAM_PLAYER_INL_
#define STREAM_PLAYER_INL_

#include "stream_player.h"
#include <typeinfo>
#include <sstream>
#include <stdint.h>

namespace stream {

template <typename T>
StreamReader<T>::StreamReader(boost::shared_ptr<std::istream> in):m_in(in)
{
	// parse header
	std::string constant;
	(*m_in) >> constant;
	if (constant != std::string("STREAM")) {
		throw PopStreamException("Could not parse the std::stream as a stream format");
	}

	std::string typeid_string;
	(*m_in) >> typeid_string;
	if (typeid_string != typeid(T).name()) {
		std::stringstream error;
		error << "The stream is in not in the correct type (" << typeid_string << "!=" << typeid(T).name() << ")";
		throw PopStreamException(error.str());
	}
}

template <typename T>
boost::optional<typename StreamReader<T>::sample> StreamReader<T>::next_sample() {
	typename StreamReader<T>::sample s;
	char c;
	(*m_in) >> s.time;
	(*m_in) >> c;

	if (m_in->eof()) {
		return boost::optional<sample>();
	}
	else if (c != ',') {
		std::stringstream error;
		error << "Could not parse the std::stream as a stream format - Expected ',' as seperator, but got " << c;
		throw StreamException(error.str());
	}

	(*m_in) >> s.data;

	return boost::optional<sample>(s);
}

template <typename T>
inline float StreamReader<T>::get_stream_length() {

	std::streamsize prev_count = m_in->tellg();

	// go to near eof
	m_in->seekg(-2, std::ios_base::end);

	// extract the sample
	sample last_sample = align_and_extract();

	// go back to the prev file position
	m_in->seekg(prev_count);

	return last_sample.time;
}

template <typename T>
void StreamReader<T>::seek(float seek_t) {

	// find the total length
	m_in->seekg(-2, std::ios_base::end);
	const std::streamsize total_len = m_in->tellg();

	std::streamsize delta = total_len/2;
	std::streamsize curr_pos = delta;


	// binary search
	float prev_t, next_t;
	while (true) {

		// go to the correct place in file
		m_in->seekg(curr_pos);
		delta /= 2;

		// find the prev_t
		prev_t = align_and_extract().time;

		// find the next_t, if it exist
		boost::optional<sample> next_optional = next_sample();
		if (!next_optional) {
			// it means that we got to the eof.
			next_t = std::numeric_limits<float>::max();
		} else {
			next_t = next_optional->time;
		}

		if ((prev_t <= seek_t) && (next_t <= seek_t)) {
			curr_pos += delta;
		}
		else if ((prev_t >= seek_t) && (next_t >= seek_t)) {
			curr_pos -= delta;
		}
		else if ((prev_t <= seek_t) && (next_t >= seek_t)) {
			m_in->seekg(curr_pos);
			align_and_extract();
			return ;
		}

		if (delta == 0) return;

	}
}

template <typename T>
typename StreamReader<T>::sample StreamReader<T>::align_and_extract() {

	m_in->clear();

	long gc = m_in->tellg();

	while (true) {
		// go to the beginning of this line
		do {
			// clear any error flag before we start
			m_in->clear();

			// go back one char
			m_in->seekg(gc--);

		} while (((char)m_in->peek() != '\n') && gc >= 0);
		if (gc < 0) break;

		// try to parse
		boost::optional<sample> s;
		try {
			s = next_sample();
		} catch (...) {
			// OK, didn't succeed. just do nothing
		}

		// if we succeded, return the sample
		if (s) {
			return *s;
		}

		// There might be some errors, so clear all the error flags
		m_in->clear();

		// if we did not find a good sample, go to the former line
		do {
			// clear any error flag before we start
			m_in->clear();

			// go back one char
			m_in->seekg(gc--);

		} while (((char)m_in->peek() != '\n') && gc >= 0);

		if (gc < 0) break;
	}
	sample s;
	s.time = 0.;
	return s;
}

template <typename T>
inline PopStreamPlayer<T>::PopStreamPlayer(boost::shared_ptr<std::istream> in, bool blocking):
		m_reader(in),
		m_blocking(blocking),
		m_seek_offset(0.),
		m_ended(false),
		m_total_stream_length(m_reader.get_stream_length())
{
	if (!blocking) {
		m_curr_sample.time = -1.;
	}

	m_timer.pause();
}

template <typename T>
inline void PopStreamPlayer<T>::start() {
	m_timer.cont();
}

template <typename T>
inline void PopStreamPlayer<T>::pause() {
	m_timer.pause();
}

template <typename T>
inline void PopStreamPlayer<T>::stop() {
	m_timer.reset();
	m_timer.pause();
}


template <typename T>
inline float PopStreamPlayer<T>::get_pos() {
	return m_timer.passed() + m_seek_offset;
}

template <typename T>
inline void PopStreamPlayer<T>::seek(float seek_t) {
	m_reader.seek(seek_t);
	m_seek_offset = seek_t;
}


template <typename T>
inline T PopStreamPlayer<T>::get_data() {

	// if the stream has already ended, just return the old sample
	if (m_ended) {
		return m_curr_sample.data;
	}

	if (m_blocking) {

		// if there is no more to give, set the m_ended bit, and return the old sample
		boost::optional<typename StreamReader<T>::sample> curr_sample_optional = m_reader.next_sample();
		if (!curr_sample_optional) {
			m_ended = true;
			return m_curr_sample.data;
		}

		typename StreamReader<T>::sample curr_sample = *curr_sample_optional;

		// block until the time comes
		float time_diff = curr_sample.time - (m_timer.passed() + m_seek_offset);
		while (time_diff > 0.) {
			usleep((uint32_t)(time_diff * 1000000.));
			time_diff = curr_sample.time - (m_timer.passed() + m_seek_offset);
		}

		m_curr_sample = curr_sample;
		return curr_sample.data;
	}
	else {
		// read from file until we got to the correct time
		while (m_curr_sample.time < (m_timer.passed() + m_seek_offset)) {

			boost::optional<typename StreamReader<T>::sample> curr_sample_optional = m_reader.next_sample();
			if (!curr_sample_optional) {
				m_ended = true;
				return m_curr_sample.data;
			}
			m_curr_sample = *curr_sample_optional;
		}

		// return the data
		return m_curr_sample.data;
	}
}

template <typename T>
PushStreamPlayer<T>::PushStreamPlayer(boost::shared_ptr<std::istream> in):
	m_reader(in),
	m_worker_thread(),
	m_seek_offset(0.),
	m_total_stream_length(m_reader.get_stream_length())
{
	m_timer.pause();
}

template <typename T>
void PushStreamPlayer<T>::register_receiver(boost::shared_ptr<DataPushStream<T> > reciever) {
	m_out_forwarder.register_receiver(reciever);
}
template <typename T>
void PushStreamPlayer<T>::unregister_receiver(boost::shared_ptr<DataPushStream<T> > reciever) {
	m_out_forwarder.unregister_receiver(reciever);
}

template <typename T>
void PushStreamPlayer<T>::run() {
	boost::optional<typename StreamReader<T>::sample > curr_sample_optional = m_reader.next_sample();

	while (curr_sample_optional) {
		typename StreamReader<T>::sample curr_sample = *curr_sample_optional;

		// block until the time comes
		while (curr_sample.time > (m_timer.passed() + m_seek_offset)) {
			usleep((uint32_t)((curr_sample.time - (m_timer.passed() + m_seek_offset)) * 1000000.));
		}

		// set the data, if somone listens
		m_out_forwarder.set_data(curr_sample.data);

		// read the next sample
		curr_sample_optional = m_reader.next_sample();

	}
}

template <typename T>
inline void PushStreamPlayer<T>::start() {
	m_timer.cont();
	if (m_worker_thread == boost::thread()) {
		boost::thread(&PushStreamPlayer<T>::run, this);
	}
}

template <typename T>
inline void PushStreamPlayer<T>::pause() {
	m_timer.pause();
}

template <typename T>
inline void PushStreamPlayer<T>::stop() {
	m_timer.reset();
	m_timer.pause();
}

template <typename T>
inline float PushStreamPlayer<T>::get_pos() {
	return (m_timer.passed() + m_seek_offset);
}

template <typename T>
inline void PushStreamPlayer<T>::seek(float seek_t) {
	m_reader.seek(seek_t);
	m_seek_offset = seek_t;
}

} // namespace stream

#endif /* STREAM_PLAYER_INL_ */
