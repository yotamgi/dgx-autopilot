#include "stream_connection.h"
#include <iostream>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <sys/time.h>

#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

namespace stream {

StreamConnection::StreamConnection(boost::shared_ptr<ConnectionFactory> factory):
		m_factory(factory),
		m_running(false),
		m_list(false)
{
	m_control = m_factory->get_connection();

	// srand
	timeval tv;
	gettimeofday(&tv, NULL);
	std::srand(tv.tv_usec);
}

StreamConnection::~StreamConnection() {
	m_control->write(protocol::control::END_COMMAND);
}

void StreamConnection::run(bool open_thread) {
	if (m_running) {
		return;
	}
	if (open_thread) {
		m_thread.reset(new boost::thread(&StreamConnection::run, this, false));
	} else {
		m_running = true;
		bool first_to_close = true;

		while (m_running) {

			if (m_list) {
				m_control->write(protocol::control::LIST_COMMAND);
				size_t num = boost::lexical_cast<size_t>(m_control->read());
				for (size_t i=0; i<num; i++) {
					m_list_data.push_back(m_control->read());
				}
				m_list = false;
			}

			// add all the streams to the fd_set
			fd_set fds;
			FD_ZERO(&fds);
			FD_SET(m_control->fd(), &fds);
			int max_fds = m_control->fd();
			for (stream_conn_map_t::iterator iter = m_open_streams.begin(); iter != m_open_streams.end(); iter++) {
				FD_SET(iter->first->fd(), &fds);
				if (iter->first->fd() > max_fds) {
					max_fds = iter->first->fd();
				}
			}

			// define the timeout to be 0.1 secs
			timeval tv; tv.tv_sec = 0; tv.tv_usec = 100000;

			// select
			int ret = select(max_fds+1, &fds, NULL, NULL, &tv);
			if (ret < 0) {
				std::cout << "Select failed: " << errno << std::endl;
				throw ConnectionExceptioin("Select failed");
			} else if (ret == 0) {
				continue;
			}

			// check who returned and write back
			if (FD_ISSET(m_control->fd(), &fds)) {
				std::string command = m_control->read();
				if (command == protocol::control::NEW_STREAM) {
					boost::shared_ptr<Connection> conn = m_factory->get_connection();

					// negotiate a bit
					conn->write("RUN");
					if (conn->read() != "RUN") {
						add_connection(conn);
					}
				} else if (command == protocol::control::END_COMMAND) {
					std::cout << "Closing Connection" << std::endl;
					m_running = false;
					first_to_close = false;
					break;
				} else if (command == protocol::control::LIST_COMMAND) {
					std::cout << "Got LIST_COMMAND" << std::endl;
					m_control->write(boost::lexical_cast<std::string>(m_exported_streams.size()));
					for (stream_name_map_t::iterator iter = m_exported_streams.begin(); iter != m_exported_streams.end(); iter++) {
						m_control->write(iter->second->get_name());
					}
				} else {
					std::cout << "Got " << command << std::endl;
					throw ConnectionExceptioin("unknown protocol");
				}
			}

			std::vector<typename stream_conn_map_t::key_type> to_be_cleaned;
			for (stream_conn_map_t::iterator iter = m_open_streams.begin(); iter != m_open_streams.end(); iter++) {
				if (FD_ISSET(iter->first->fd(), &fds)) {
					std::stringstream ss;
					std::string command = iter->first->read();
					if (command.at(0) == protocol::GET_COMMAND) {
						iter->second->serialize(ss);
						iter->first->write(ss.str());
					} else if (command.at(0) == protocol::SET_COMMAND) {
						ss << command;
						char c;
						ss >> c; // remove the command
						iter->second->deserialize(ss);
					} else if (command.at(0) == protocol::END_STREAM) {
						to_be_cleaned.push_back(iter->first);
					} else {
						throw ConnectionExceptioin("unknown protocol");
					}
				}
			}

			for (size_t i=0; i<to_be_cleaned.size(); i++) {
				m_open_streams.erase(to_be_cleaned[i]);
			}
		}
		if (first_to_close) {
			m_control->write(protocol::control::END_COMMAND);
		}

		// free all the opened connections
		m_open_streams.clear();
	}
}

std::vector<std::string> StreamConnection::list_avail() {
	m_list = true;
	while (m_list != false);

	return m_list_data;
}

void StreamConnection::add_connection(boost::shared_ptr<Connection> conn) {
	std::string header = conn->read();
	if (header[0] == protocol::DATA_CONN) {
		std::string stream_name = header.substr(1, header.size()-1);

		m_open_streams[conn] = m_exported_streams.at(stream_name);
	} else {
		std::cout << "This is a wierd connection... (header: '" << header << "')" << std::endl;
	}
}

}  // namespace stream
