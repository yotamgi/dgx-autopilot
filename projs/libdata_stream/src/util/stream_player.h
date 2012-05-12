#ifndef STREAM_PLAYER_H_
#define STREAM_PLAYER_H_

#include <stream/data_pop_stream.h>
#include <stream/data_push_stream.h>
#include <stream/util/time.h>
#include <stream/stream_utils.h>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>
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
	StreamReader(boost::shared_ptr<std::istream> in);

	struct sample {
		T data;
		float time;
	};

	/**
	 * The next sample function.
	 * @returns the next sample if exists, or NULL if the stream ended.
	 * @throws StreamException if the file does not meets the format requirments
	 */
	boost::optional<sample> next_sample();

private:
	boost::shared_ptr<std::istream> m_in;
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
	 * @param in - the stream to play.
	 * @param blocking - tells the stream player to be either blocking and
	 * completely complient with recorded stream, or it can skip samples if their
	 * time passed and to return the data immedietly if not.
	 */
	PopStreamPlayer(boost::shared_ptr<std::istream> in, bool blocking = true);

	bool ended() { return m_ended; }

	T get_data();

private:

	StreamReader<T> m_reader;

	bool m_blocking;

	Timer m_timer;

	typename StreamReader<T>::sample m_curr_sample;

	bool m_ended;
};


/**
 * The PushStreamPlayer class.
 * This class is a PushGenerator that uses StreamReader class (above) to play a
 * stream format std::istream.
 * In order for it to work it opens a thread.
 */
template <typename T>
class PushStreamPlayer : public PushGenerator<T> {
public:
	/**
	 * Constructor.
	 * @param in - the stream to play.
	 */
	PushStreamPlayer(boost::shared_ptr<std::istream> in);

	void register_receiver(boost::shared_ptr<DataPushStream<T> > reciever);
	void unregister_receiver(boost::shared_ptr<DataPushStream<T> > reciever);

private:

	void run();

	StreamReader<T> m_reader;
	boost::thread m_worker_thread;
	PushForwarder<T> m_out_forwarder;

	Timer m_timer;
};


} // namespace stream

#include "stream_player.inl"

#endif /* STREAM_PLAYER_H_ */
