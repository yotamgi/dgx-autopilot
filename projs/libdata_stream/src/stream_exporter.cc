#include "stream_exporter.h"
#include <stdexcept>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <iostream>

namespace stream {

StreamExporter::StreamExporter():m_is_running(false), m_client_sock(0), m_server_sock(0)
{}

StreamExporter::~StreamExporter()
{
	if (m_client_sock) close(m_client_sock);
	if (m_server_sock) close(m_server_sock);
}

void StreamExporter::run() {
    struct sockaddr_in server_add, client_add;

    // Create the TCP socket
    if ((m_server_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    	throw std::runtime_error("Failed to create socket");
    }

    // Construct the server sockaddr_in structure
    memset(&server_add, 0, sizeof(server_add));       // Clear struct
    server_add.sin_family = AF_INET;                  // Internet/IP
    server_add.sin_addr.s_addr = htonl(INADDR_ANY);   // Incoming addr
    server_add.sin_port = htons(protocol::PORT);      // server port

    // Bind the server socket
    if (bind(m_server_sock, (struct sockaddr *) &server_add,
                                 sizeof(server_add)) < 0) {
    	throw std::runtime_error("Failed to bind the server socket");
    }

    // Listen on the server socket
    if (listen(m_server_sock, 5) < 0) {
    	throw std::runtime_error("Failed to listen on server socket");
    }

    // Run until cancelled
    m_is_running = true;
    while (m_is_running) {
		unsigned int clientlen = sizeof(client_add);
		// Wait for client connection
		if ((m_client_sock =
		   accept(m_server_sock, (struct sockaddr *) &client_add,
				  &clientlen)) < 0)
		{
			continue;
		}
		std::cout << "Client connected: " << inet_ntoa(client_add.sin_addr) << std::endl;
		handle_client();
    }
    close(m_server_sock);
}

void StreamExporter::handle_client() {
	const size_t BUFF_SIZE = 100;

	// make the socket nonblock
	timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 100000;

	setsockopt(m_client_sock, SOL_SOCKET, SO_RCVTIMEO, (void*)&tv, sizeof(tv));
	while (m_is_running) {
		char buff[BUFF_SIZE];
		ssize_t read_size;
		if ( (read_size = read(m_client_sock, buff, BUFF_SIZE)) <= 0) {
			continue;
		}
		buff[read_size] = '\0';

		if (buff[0] == protocol::GET_COMMAND) {
			std::string name(&buff[1]);

			if (m_exported_streams.count(name) != 1) {
				std::cout << "Asked for a non-existing stream" << std::endl;
				if (write(m_client_sock, protocol::NOT_EXIST_COMMAND.c_str(), 9) != 9) {
					break;
				}
			}
			else {
				std::stringstream ss;
				m_exported_streams[name]->serialize(ss);
				ss << protocol::SEPERATOR;
				size_t written_size = write(m_client_sock, ss.str().c_str(), ss.str().size());
				if (written_size != ss.str().size()) {
					std::cout << "It seems like the client closed" << std::endl;
					break;
				}
			}
		}
		else if (buff[0] == protocol::LIST_COMMAND) {
			std::string data;
			for(stream_map_t::iterator i=m_exported_streams.begin(); i != m_exported_streams.end(); i++) {
				data += std::string(i->first);
				data += protocol::SEPERATOR;
			}
			if (write(m_client_sock, data.c_str(), data.size()) != (signed)data.size()) {
				std::cout << "It seems like the client closed" << std::endl;
				break;
			}
		}
	}
	write(m_client_sock, &protocol::END, 1);
	close(m_client_sock);
	m_client_sock = 0;
}

void StreamExporter::stop() {
	m_is_running = false;
}

} // namespace stream
