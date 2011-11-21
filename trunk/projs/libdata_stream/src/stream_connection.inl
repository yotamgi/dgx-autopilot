#include "stream_connection.h"
#include <iostream>


namespace stream {

inline StreamConnection::StreamConnection(boost::shared_ptr<ConnectionFactory> factory):
		m_factory(factory),
		m_running(false)
{}

template <typename T>
inline boost::shared_ptr<DataGenerator<T> > StreamConnection::import_stream(std::string name) {
	boost::shared_ptr<Connection> conn = m_factory->get_connection();
	std::string header = conn->read();
	while (header != "OK") {
		std::cout << "Got header" << header << std::endl;
		add_connection(conn, header);
		boost::shared_ptr<Connection> conn = m_factory->get_connection();
		header = conn->read();
	}
	conn->write("SABABA");
	conn->write(std::string(&protocol::DATA_CONN, 1) + name);
	return boost::make_shared<StreamProxy<T> >(conn);
}

template <typename T>
inline void StreamConnection::export_stream(boost::shared_ptr<DataGenerator<T> > stream, std::string name) {
	m_opened_stream_mutex.lock();
	m_exported_streams[name] = boost::make_shared<SpecificStream<T> >(stream);
	m_opened_stream_mutex.unlock();
}

template <typename T>
StreamConnection::StreamProxy<T>::StreamProxy(boost::shared_ptr<Connection> conn):
			m_conn(conn) {}

template <typename T>
StreamConnection::StreamProxy<T>::~StreamProxy() {
	m_conn->write(std::string(&protocol::END_STREAM, 1));
}

template <typename T>
inline T StreamConnection::StreamProxy<T>::get_data()  {

	// ask for the data
	m_conn->write(std::string(&protocol::GET_COMMAND, 1));

	// get the serialized data
	std::string serialized_data = m_conn->read();

	// deserialize
	std::stringstream ss(serialized_data);
	T data;
	ss >> data;
	if (ss.fail()) {
		throw std::runtime_error("The exported stream does not match: couldn't parse: " + ss.str());
	}
	return data;
}


}  // namespace stream
