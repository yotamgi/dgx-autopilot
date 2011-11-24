#ifndef STREAM_CONNECTION_H_
#define STREAM_CONNECTION_H_

#include "connection.h"
#include "generators.h"
#include "protocol.h"
#include <string>
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
	void export_stream(boost::shared_ptr<DataGenerator<T> > stream, std::string name);

	template <typename T>
	boost::shared_ptr<DataGenerator<T> > import_stream(std::string name);

	void run(bool open_thread = true);

	void stop() {m_running = false; m_thread->join(); }

private:

	void add_connection(boost::shared_ptr<Connection> conn);

	/**
	 * The StreamProxy class.
	 * Proxies a stream through the conn object
	 */
	template <typename T>
	class StreamProxy : public DataGenerator<T> {
	public:
		StreamProxy(boost::shared_ptr<Connection> conn);
		~StreamProxy();

		T get_data();

	private:

		boost::shared_ptr<Connection> m_conn;
	};

	/**
	 * Classes for getting reed of the bloody template arguments in the streams
	 */
	class AnyStream {
	public:
		virtual void serialize(std::ostream&) = 0;
	};
	template <typename T>
	class SpecificStream : public AnyStream {
	public:
		SpecificStream(boost::shared_ptr<DataGenerator<T> > gen):m_gen(gen) {}

		void serialize(std::ostream& os) {
			os << m_gen->get_data();
		}
	private:
		boost::shared_ptr<DataGenerator<T> > m_gen;
	};


	typedef std::map<std::string, boost::shared_ptr<AnyStream> >      stream_name_map_t;
	typedef std::map<boost::shared_ptr<Connection>, boost::shared_ptr<AnyStream> >  stream_conn_map_t;

	stream_name_map_t m_exported_streams;
	stream_conn_map_t m_open_streams;

	boost::shared_ptr<ConnectionFactory> m_factory;

	boost::shared_ptr<Connection> m_control;

	volatile bool m_running;
	boost::shared_ptr<boost::thread> m_thread;
};

} // namespace stream

#include "stream_connection.inl"

#endif /* STREAM_CONNECTION_H_ */
