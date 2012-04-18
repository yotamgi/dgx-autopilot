#ifndef ASYNC_STREAM_CONNECTION_H_
#define ASYNC_STREAM_CONNECTION_H_

#include <stream/data_pop_stream.h>
#include <stream/data_push_stream.h>
#include <stream/connection.h>
#include <iostream>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

namespace stream {

class AsyncStreamConnection {
public:

	//
	// Classes for getting reed of the bloody template arguments in the streams
	//

	class SendStream {
	public:
		virtual void serialize(std::ostream&) = 0;
	};

	class RecvStream {
	public:
		virtual void deserialize(std::istream&) = 0;
	};

	typedef std::vector<boost::shared_ptr<SendStream> > send_streams_t;
	typedef std::vector<boost::shared_ptr<RecvStream> > recv_streams_t;

	//
	// Methods
	//

	AsyncStreamConnection(send_streams_t send_streams,
						  recv_streams_t recv_streams,
						  boost::shared_ptr<ConnectionFactory> conn_factory,
						  bool side,
						  float rate); // in Hz

	~AsyncStreamConnection();

	void start();
	void stop();

	//
	// Classes specifications for the specific streams
	//

	template <typename T>
	class SendPopStream : public SendStream {
	public:
		SendPopStream(boost::shared_ptr<DataPopStream<T> > s): m_s(s) {}

		virtual void serialize(std::ostream& os);
	private:
		boost::shared_ptr<stream::DataPopStream<T> > m_s;
	};

	template <typename T>
	class SendPushStream : public SendStream, public DataPushStream<T> {
	public:
		SendPushStream():m_counter(0) {}

		virtual void set_data(const T& data);
		virtual void serialize(std::ostream& os);
	private:
		size_t m_counter;
		size_t m_curr_data;
	};

	template <typename T>
	class RecvPopStream : public RecvStream, public DataPopStream<T> {
	public:
		virtual T get_data();
		virtual void deserialize(std::istream&);
	public:
		T m_curr_data;
	};

	template <typename T>
	class RecvPushStream : public RecvStream {
	public:
		RecvPushStream(boost::shared_ptr<DataPushStream<T> > s): m_s(s), m_counter(0) {}

		virtual void deserialize(std::istream&);
	public:
		boost::shared_ptr<DataPushStream<T> > m_s;
		size_t m_counter;
	};

private:

	std::string create_send_packet();
	void parse_recv_packet(std::string packet);
	void run_exporting();
	void run_importing();

	// the streams to export
	send_streams_t m_send_streams;
	recv_streams_t m_recv_streams;

	// the connections
	boost::shared_ptr<Connection> m_send_conn;
	boost::shared_ptr<Connection> m_recv_conn;

	// threads stuff
	boost::thread m_exporting_thread;
	boost::thread m_importing_thread;
	volatile bool m_running;

	size_t m_wait_time;
};

} // namespace stream

#include "async_stream_connection.inl"


#endif /* ASYNC_STREAM_CONNECTION_H_ */
