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

	/**
	 * @return the stream length in seconds
	 */
	float get_stream_length();

	/**
	 * Jumps to certain time in the stream file.
	 * @param time to seek to in seconds
	 */
	void seek(float time);

private:

	enum SampleState {
		OK, FAILED, END_OF_FILE
	};

	/** Parses one sample and return its state */
	SampleState parse_sample(sample& ans);

	/**
	 * This is a helper function that align the file counter to a coherent
	 * position and extract one sample from it.
	 * If it does not succeed, it returns nothing. A fail means that the stream
	 * in the current position could not be parsed at all.
	 * NOTE - it does affect the file's get counter.
	 */
	boost::optional<sample> align_and_extract();

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

	/**
	 * The start function.
	 * This funcion must be called in order of the class to start playing the
	 * stream.
	 */
	void start();

	/** Stops the stream playing */
	void stop();

	/** Pause the stream playing */
	void pause();

	/**
	 * Jump to a certain point in the stream, so now the play will continue
	 * from there.
	 * @param time to seek to in seconds
	 */
	void seek(float seek_t);

	/** @return the current position of the stream in seconds */
	float get_pos();

	bool ended() { return m_ended; }

	/**
	 * @return the stream length in seconds
	 */
	float get_stream_length() { return m_total_stream_length; }

	T get_data();

private:

	StreamReader<T> m_reader;

	bool m_blocking;

	Timer m_timer;
	float m_seek_offset;

	typename StreamReader<T>::sample m_curr_sample;

	bool m_ended;

	float m_total_stream_length;
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

	/**
	 * The start function.
	 * This funcion must be called in order of the class to start playing the
	 * stream.
	 */
	void start();

	/** Stops the stream playing */
	void stop();

	/** Pause the stream playing */
	void pause();

	/**
	 * Jump to a certain point in the stream, so now the play will continue
	 * from there.
	 * @param time to seek to in seconds
	 */
	void seek(float seek_t);

	/** @return the current position of the stream in seconds */
	float get_pos();

	/**
	 * @return the stream length in seconds
	 */
	float get_stream_length() { return m_total_stream_length; }

	void register_receiver(boost::shared_ptr<DataPushStream<T> > reciever);
	void unregister_receiver(boost::shared_ptr<DataPushStream<T> > reciever);

private:

	void run();

	StreamReader<T> m_reader;
	boost::thread m_worker_thread;
	PushForwarder<T> m_out_forwarder;

	Timer m_timer;
	float m_seek_offset;

	float m_total_stream_length;
};


} // namespace stream

#include "stream_player.inl"

#endif /* STREAM_PLAYER_H_ */
