#include "stream_connection.h"
#include <iostream>
#include "protocol.h"


namespace stream {


template <typename T>
inline boost::shared_ptr<DataPopStream<T> > StreamConnection::import_pop_stream(std::string name) {
	m_control->write(protocol::control::NEW_STREAM);
	boost::shared_ptr<Connection> conn = m_factory->get_connection();

	// Make sure we are not talking with another import function
	conn->write("IMP");
	while (conn->read() != "RUN") {
		usleep(rand()%30000);
		m_control->write(protocol::control::NEW_STREAM);
		conn = m_factory->get_connection();
		conn->write("IMP");
	}

	conn->write(std::string(&protocol::DATA_CONN, 1) + name);

	return boost::make_shared<PopStreamProxy<T> >(conn);
}

template <typename T>
inline boost::shared_ptr<DataPushStream<T> > StreamConnection::import_push_stream(std::string name) {
	m_control->write(protocol::control::NEW_STREAM);
	boost::shared_ptr<Connection> conn = m_factory->get_connection();

	// Make sure we are not talking with another import function
	conn->write("IMP");
	while (conn->read() != "RUN") {
		usleep(rand()%30000);
		m_control->write(protocol::control::NEW_STREAM);
		conn = m_factory->get_connection();
		conn->write("IMP");
	}

	conn->write(std::string(&protocol::DATA_CONN, 1) + name);

	return boost::make_shared<PushStreamProxy<T> >(conn);
}


template <typename T>
inline void StreamConnection::export_pop_stream(boost::shared_ptr<DataPopStream<T> > stream, std::string name) {
	m_exported_streams[name] = boost::make_shared<SpecificStream<T> >(stream, name);
}

template <typename T>
inline void StreamConnection::export_push_stream(boost::shared_ptr<DataPushStream<T> > stream, std::string name) {
	m_exported_streams[name] = boost::make_shared<SpecificStream<T> >(stream, name);
}


template <typename T>
StreamConnection::PopStreamProxy<T>::PopStreamProxy(boost::shared_ptr<Connection> conn):
			m_conn(conn) {}

template <typename T>
StreamConnection::PopStreamProxy<T>::~PopStreamProxy() {
	try {
		m_conn->write(std::string(&protocol::END_STREAM, 1));
	} catch (ConnectionExceptioin& e) {
		// do nothing - it might already be closed
	}
}

template <typename T>
inline T StreamConnection::PopStreamProxy<T>::get_data()  {

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

template <typename T>
StreamConnection::PushStreamProxy<T>::PushStreamProxy(boost::shared_ptr<Connection> conn):
			m_conn(conn) {}

template <typename T>
StreamConnection::PushStreamProxy<T>::~PushStreamProxy() {
	try {
		m_conn->write(std::string(&protocol::END_STREAM, 1));
	} catch (ConnectionExceptioin& e) {
		// do nothing - it might already be closed
	}
}

template <typename T>
inline void StreamConnection::PushStreamProxy<T>::set_data(const T& data)  {
	// prepare the data and serialize it
	std::stringstream ss;
	ss << data;
	std::string serialized_data = std::string(&protocol::SET_COMMAND, 1) + ss.str();

	// send the data
	m_conn->write(serialized_data);
}

}  // namespace stream
