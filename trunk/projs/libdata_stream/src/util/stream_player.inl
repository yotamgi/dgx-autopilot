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
typename StreamReader<T>::SampleState StreamReader<T>::parse_sample(typename StreamReader<T>::sample& ans) {
	char c;
	(*m_in) >> ans.time;
	(*m_in) >> c;

	if (m_in->eof()) {
		return END_OF_FILE;
	}
	else if (c != ',') {
		return FAILED;
	}

	try {
		(*m_in) >> ans.data;
	} catch (...) {
		return FAILED;
	}

	return OK;
}

template <typename T>
boost::optional<typename StreamReader<T>::sample> StreamReader<T>::next_sample() {
	typename StreamReader<T>::sample s;
	SampleState state = parse_sample(s);

	if (state == END_OF_FILE) {
		return boost::optional<sample>();
	} else if (state == FAILED) {
		throw StreamException("Could not parse the std::stream as a stream format - Could not find ','");
	} else {
		return boost::optional<sample>(s);
	}
}

template <typename T>
inline float StreamReader<T>::get_stream_length() {

	std::streamsize prev_count = m_in->tellg();

	// go to near eof
	m_in->seekg(-2, std::ios_base::end);

	// extract the sample
	boost::optional<sample> last_sample = align_and_extract();

	// go back to the prev file position
	m_in->seekg(prev_count);

	if (last_sample) {
		return last_sample->time;
	} else {
		return 0.;
	}
}

template <typename T>
void StreamReader<T>::seek(float seek_t) {

	m_in->clear();
	std::streamsize prev_count = m_in->tellg();

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
		boost::optional<sample> prev_sample = align_and_extract();
		if (prev_sample) {
			prev_t = prev_sample->time;
		} else {
			// if align_and_extract failed, it means that the file has no samples at all
			m_in->seekg(prev_count);
			return;
		}

		// find the next_t, if it exist
		sample next_sample;
		SampleState state = parse_sample(next_sample);
		if (state != OK) {
			next_t = std::numeric_limits<float>::max();
		} else {
			next_t = next_sample.time;
		}

		// advance the binary search according to the prev_t and next_t
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

		// delta == zero means that we didn't find a correct place. just give up seeking
		if (delta == 0) {
			m_in->seekg(prev_count);
			return;
		}

	}
}

template <typename T>
boost::optional<typename StreamReader<T>::sample> StreamReader<T>::align_and_extract() {

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
		sample s;
		if (parse_sample(s) == OK) {
			// if we succeded, return the sample
			return boost::optional<sample>(s);
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
	std::cout << "Align and extract failed." << std::endl;
	return boost::optional<sample>();
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
