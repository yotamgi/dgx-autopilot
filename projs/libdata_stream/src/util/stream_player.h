#ifndef STREAM_PLAYER_H_
#define STREAM_PLAYER_H_

#include <stream/data_pop_stream.h>
#include <stream/util/time.h>
#include <istream>

namespace stream {

/**
 * The StreamReader class.
 * This class knows how to read from an std::stream in a stream format and to return
 * the data.
 * stream format is a format for stream recording - it holds the stream data samples
 * and the time they were taken. Those stream files could be created using the
 * StreamRecorder filter.
 * In every time you will call next_sample, the class will return the next from the
 * std::stream and the time it was taken.
 */
template <typename T>
class StreamReader {
public:
	StreamReader(std::istream& in);

	struct sample {
		T data;
		float time;
	};

	sample next_sample();

private:
	std::istream& m_in;
};

/**
 * The PopStreamPlayer class.
 * This class uses the StreamReader class (above) to play a stream format
 * std::istream.
 */
template <typename T>
class PopStreamPlayer : public DataPopStream<T> {
public:
	/**
	 * Constructor.
	 * @param in - the stream to write to.
	 * @param blocking - tells the stream player to be either blocking and
	 * completely complient with recorded stream, or it can skip samples if their
	 * time passed and to return the data immedietly if not.
	 */
	PopStreamPlayer(std::istream& in, bool blocking = true);

	T get_data();

private:

	StreamReader<T> m_reader;

	bool m_blocking;

	Timer m_timer;

	typename StreamReader<T>::sample m_curr_sample;
};

} // namespace stream

#include "stream_player.inl"

#endif /* STREAM_PLAYER_H_ */
