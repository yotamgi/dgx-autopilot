#include "stream_exporter.h"
#include <stdexcept>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

StreamExporter::StreamExporter()
{}

void StreamExporter::run() {
    int serversock, clientsock;
    struct sockaddr_in echoserver, echoclient;

    // Create the TCP socket
    if ((serversock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    	throw std::runtime_error("Failed to create socket");
    }

    // Construct the server sockaddr_in structure
    memset(&echoserver, 0, sizeof(echoserver));       // Clear struct
    echoserver.sin_family = AF_INET;                  // Internet/IP
    echoserver.sin_addr.s_addr = htonl(INADDR_ANY);   // Incoming addr
    echoserver.sin_port = htons(protocol::PORT);      // server port

    // Bind the server socket
    if (bind(serversock, (struct sockaddr *) &echoserver,
                                 sizeof(echoserver)) < 0) {
    	throw std::runtime_error("Failed to bind the server socket");
    }

    // Listen on the server socket
    if (listen(serversock, 5) < 0) {
    	throw std::runtime_error("Failed to listen on server socket");
    }

    // Run until cancelled
    while (1) {
		unsigned int clientlen = sizeof(echoclient);
		// Wait for client connection
		if ((clientsock =
		   accept(serversock, (struct sockaddr *) &echoclient,
				  &clientlen)) < 0)
		{
		continue;
		}
		std::cout << "Client connected: " << inet_ntoa(echoclient.sin_addr) << std::endl;
		handle_client(clientsock);
    }
}

void StreamExporter::handle_client(int sock) {
	const size_t BUFF_SIZE = 100;
	while (true) {
		char buff[BUFF_SIZE];
		size_t read_size = read(sock, buff, BUFF_SIZE);

		buff[read_size] = '\0';
		if (buff[0] == protocol::GET_COMMAND) {
			std::string name(&buff[1]);

			if (m_exported_streams.count(name) != 1) {
				std::cout << "Asked for a non-existing stream" << std::endl;
				if (write(sock, protocol::NOT_EXIST_COMMAND.c_str(), 9) != 9) {
					break;
				}
			}
			else {
				std::stringstream ss;
				m_exported_streams[name]->serialize(ss);
				ss << protocol::SEPERATOR;
				size_t written_size = write(sock, ss.str().c_str(), ss.str().size());
				if (written_size != ss.str().size()) {
					std::cout << "It seems like the client closed" << std::endl;
					break;
				}
			}
		}
	}
	close(sock);
}
