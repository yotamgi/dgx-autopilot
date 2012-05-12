#include "async_stream_connection.h"
#include <sstream>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>

namespace stream {

const std::string AsyncStreamConnection::ALIVE_MESSAGE = "ALIVE";

AsyncStreamConnection::AsyncStreamConnection(send_streams_t send_streams,
    										 recv_streams_t recv_streams,
    										 boost::shared_ptr<ConnectionFactory> conn_factory,
    										 bool side,
    										 float rate):
		m_send_streams(send_streams),
		m_recv_streams(recv_streams),
		m_running(false),
		m_quality_stream(boost::make_shared<AsyncStreamConnection::QualityStream>(rate))
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
	packet << ALIVE_MESSAGE << std::endl;
	for (size_t i=0; i<m_send_streams.size(); i++) {
		m_send_streams[i]->serialize(packet);
	}
	return packet.str();
}

void AsyncStreamConnection::parse_recv_packet(std::string p) {
	std::stringstream packet(p);
	std::string alive;
	packet >> alive;
	if (alive != ALIVE_MESSAGE) {
		throw ConnectionExceptioin("The other did not return a proper alive message: " + alive);
	}
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
		m_quality_stream->got_sample();
	}
}

boost::shared_ptr<stream::DataPopStream<float> > AsyncStreamConnection::get_quality_stream() {
	return m_quality_stream;
}

AsyncStreamConnection::QualityStream::QualityStream(float expected_rate):
		m_counter(0),
		m_curr_rate(0.0f),
		m_expected_rate(expected_rate)
{}

void AsyncStreamConnection::QualityStream::got_sample() {
	m_counter++;
	update();
}
void AsyncStreamConnection::QualityStream::update() {
	if (m_timer.passed() > INTERVAL) {
		m_curr_rate = m_counter/m_timer.passed();
		m_timer.reset();
		m_counter = 0;
	}
}

float AsyncStreamConnection::QualityStream::get_data() {
	update();
	return m_curr_rate/m_expected_rate;
}


} // namespace stream
