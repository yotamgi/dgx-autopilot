#include "async_stream_connection.h"
#include <sstream>
#include <boost/bind.hpp>

namespace stream {

AsyncStreamConnection::AsyncStreamConnection(send_streams_t send_streams,
    										 recv_streams_t recv_streams,
    										 boost::shared_ptr<ConnectionFactory> conn_factory,
    										 bool side,
    										 float rate):
		m_send_streams(send_streams),
		m_recv_streams(recv_streams),
		m_running(false)
{
	if (side) {
		m_send_conn = conn_factory->get_connection();
		m_recv_conn = conn_factory->get_connection();
	} else {
		m_recv_conn = conn_factory->get_connection();
		m_send_conn = conn_factory->get_connection();
	}

	m_wait_time = (int)((1. / rate) * 1000000.);
}

AsyncStreamConnection::~AsyncStreamConnection() {
	stop();
}

std::string AsyncStreamConnection::create_send_packet() {
	std::stringstream packet;
	for (size_t i=0; i<m_send_streams.size(); i++) {
		m_send_streams[i]->serialize(packet);
	}
	return packet.str();
}

void AsyncStreamConnection::parse_recv_packet(std::string p) {
	std::stringstream packet(p);
	for (size_t i=0; i<m_recv_streams.size(); i++) {
		m_recv_streams[i]->deserialize(packet);
	}
}


void AsyncStreamConnection::start() {
	m_exporting_thread = boost::thread(boost::bind(&AsyncStreamConnection::run_exporting, this));
	m_importing_thread = boost::thread(boost::bind(&AsyncStreamConnection::run_importing, this));

}
void AsyncStreamConnection::stop() {
	if (m_running) {
		m_running = false;
		m_exporting_thread.join();
		m_importing_thread.detach();
	}
}

void AsyncStreamConnection::run_exporting() {
	m_running = true;
	while (m_running) {
		std::string send_packet = create_send_packet();
		m_send_conn->write(send_packet);
		//std::cout << "Sending: " << send_packet << std::endl;
		usleep(m_wait_time);
	}
}
void AsyncStreamConnection::run_importing() {
	m_running = true;
	while (m_running) {
		std::string recv_packet = m_recv_conn->read();
		//std::cout << "Got " << recv_packet << std::endl;
		parse_recv_packet(recv_packet);
		usleep(m_wait_time);
	}
}


} // namespace stream
