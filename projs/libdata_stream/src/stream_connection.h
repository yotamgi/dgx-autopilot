#ifndef STREAM_CONNECTION_H_
#define STREAM_CONNECTION_H_

#include "connection.h"
#include "data_pop_stream.h"
#include "data_push_stream.h"
#include "protocol.h"
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

namespace stream {


/**
 * Stream Connection class
 * Allows exporting and importing streams over a Connection object it gets.
 */
class StreamConnection {
public:
	StreamConnection(boost::shared_ptr<ConnectionFactory> factory);
	~StreamConnection();

	template <typename T>
	void export_pop_stream(boost::shared_ptr<DataPopStream<T> > stream, std::string name);

	template <typename T>
	void export_push_stream(boost::shared_ptr<DataPushStream<T> > stream, std::string name);


	template <typename T>
	boost::shared_ptr<DataPopStream<T> > import_pop_stream(std::string name);

	template <typename T>
	boost::shared_ptr<DataPushStream<T> > import_push_stream(std::string name);

	std::vector<std::string> list_avail();

	void run(bool open_thread = true);

	void stop() {m_running = false; m_thread->join(); }

private:

	void add_connection(boost::shared_ptr<Connection> conn);

	/**
	 * The PopStreamProxy class.
	 * Proxies a pop stream through the conn object
	 */
	template <typename T>
	class PopStreamProxy : public DataPopStream<T> {
	public:
		PopStreamProxy(boost::shared_ptr<Connection> conn);
		~PopStreamProxy();

		T get_data();

	private:

		boost::shared_ptr<Connection> m_conn;
	};


	/**
	 * The PushStreamProxy class.
	 * Proxies a push stream through the conn object
	 */
	template <typename T>
	class PushStreamProxy : public DataPushStream<T> {
	public:
		PushStreamProxy(boost::shared_ptr<Connection> conn);
		~PushStreamProxy();

		void set_data(const T&);

	private:

		boost::shared_ptr<Connection> m_conn;
	};


	/**
	 * Classes for getting reed of the bloody template arguments in the streams
	 */
	class AnyStream {
	public:
		virtual void serialize(std::ostream&) = 0;
		virtual void deserialize(std::istream&) = 0;
		virtual std::string get_name() = 0;
	};
	template <typename T>
	class SpecificStream : public AnyStream {
	public:
		SpecificStream(boost::shared_ptr<DataPopStream<T> > gen, std::string name):m_gen(gen), m_name(name) {}
		SpecificStream(boost::shared_ptr<DataPushStream<T> > col, std::string name):m_col(col), m_name(name) {}
		SpecificStream(boost::shared_ptr<DataPopStream<T> > gen, boost::shared_ptr<DataPushStream<T> > col, std::string name):
			m_gen(gen), m_col(col), m_name(name) {}

		void serialize(std::ostream& os) {
			os << m_gen->get_data();
		}
		void deserialize(std::istream& is) {
			T data; is >> data;
			m_col->set_data(data);
		}

		std::string get_name() { return m_name; }
	private:
		boost::shared_ptr<DataPopStream<T> > m_gen;
		boost::shared_ptr<DataPushStream<T> > m_col;
		std::string m_name;
	};


	typedef std::map<std::string, boost::shared_ptr<AnyStream> >      stream_name_map_t;
	typedef std::map<boost::shared_ptr<Connection>, boost::shared_ptr<AnyStream> >  stream_conn_map_t;

	stream_name_map_t m_exported_streams;
	stream_conn_map_t m_open_streams;

	boost::shared_ptr<ConnectionFactory> m_factory;

	boost::shared_ptr<Connection> m_control;

	volatile bool m_running;
	boost::shared_ptr<boost::thread> m_thread;

	volatile bool m_list;
	std::vector<std::string> m_list_data;
};

} // namespace stream

#include "stream_connection.inl"

#endif /* STREAM_CONNECTION_H_ */
