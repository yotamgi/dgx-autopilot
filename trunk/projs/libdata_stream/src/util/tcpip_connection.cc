#include "tcpip_connection.h"
#include <sstream>
#include <iostream>
#include <boost/make_shared.hpp>
#include <boost/lexical_cast.hpp>

// socket includes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>


namespace stream {

TcpipConnection::TcpipConnection(int sock_fd):m_sock_fd(sock_fd)
{}

int TcpipConnection::fd() {
	return m_sock_fd;
}

std::string TcpipConnection::read() {

	// first, read the size of the data
	std::string str_size;
	char n;
	do {
		if (::read(m_sock_fd, &n, 1) != 1) {
			throw ConnectionExceptioin("Could not read from socket");
		}
		str_size += std::string(&n,1);
	} while (n != ';');

	// understand the size
	size_t size = boost::lexical_cast<size_t>(str_size.substr(0, str_size.size()-1));

	// read the data
	char buff[size];
	if (::read(m_sock_fd, buff, size) != (int)size) {
		throw ConnectionExceptioin("Could not read from socket");
	}
	return std::string(buff, size);
}

void TcpipConnection::write(std::string data) {
	std::stringstream packet;
	packet << data.size() << ";" << data;
	if (::write(m_sock_fd, packet.str().c_str(), packet.str().size()) != (int)packet.str().size()) {
		close(m_sock_fd);
		throw ConnectionExceptioin("Could not write to socket");
	}
}

TcpipConnection::~TcpipConnection() {
	std::cout << "Closing connection" << std::endl;
	if (close(m_sock_fd) != 0)
		std::cout << "Couldn't close socket" << std::endl;
}

TcpipServer::TcpipServer(std::string listen_address, size_t port) {
	struct sockaddr_in server_add;

    // Create the TCP socket
    if ((m_server_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    	throw std::runtime_error("Failed to create socket");
    }

	// set the reuse address option
    int opt = 1;
    setsockopt(m_server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));


    // Construct the server sockaddr_in structure
    memset(&server_add, 0, sizeof(server_add));       // Clear struct
    server_add.sin_family = AF_INET;                  // Internet/IP
    server_add.sin_addr.s_addr = htonl(INADDR_ANY);   // Incoming addr
    server_add.sin_port = htons(port);      		  // server port

    // Bind the server socket
    if (bind(m_server_sock, (struct sockaddr *) &server_add,
                                 sizeof(server_add)) < 0) {
    	throw std::runtime_error("Failed to bind the server socket");
    }

    // Listen on the server socket
    if (listen(m_server_sock, 5) < 0) {
    	throw std::runtime_error("Failed to listen on server socket");
    }
}

boost::shared_ptr<Connection> TcpipServer::get_connection(){
	struct sockaddr_in client_add;

	unsigned int clientlen = sizeof(client_add);
	int client_sock;

	// Wait for client connection
	if ((client_sock = accept(m_server_sock, (struct sockaddr *) &client_add, &clientlen)) < 0) {
		throw ConnectionExceptioin("Server's accept function failed");
	}

	std::cout << "Client connected: " << inet_ntoa(client_add.sin_addr) << std::endl;

	boost::shared_ptr<Connection> conn = boost::make_shared<TcpipConnection>(client_sock);

	// some handshake
	conn->read();
	conn->write("Hello");

	return conn;
}

TcpipServer::~TcpipServer() {
	std::cout << "Closing server socket" << std::endl;
	if (close(m_server_sock) != 0)
		std::cout << "Couldn't close socket" << std::endl;
}


TcpipClient::TcpipClient(std::string host_address, size_t host_port):
		m_host_address(host_address),
		m_host_port(host_port) {}

boost::shared_ptr<Connection> TcpipClient::get_connection() {
	// init the connection
    struct sockaddr_in serv_addr;
    struct hostent *server;
	int sock_fd;

    sock_fd  = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd  < 0) {
    	throw ConnectionExceptioin("Could not create socket!");
    }

    server = gethostbyname(m_host_address.c_str());
    if (server == NULL) {
    	throw ConnectionExceptioin("No such host");
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
	serv_addr.sin_port = htons(m_host_port);

    while (::connect(sock_fd,(struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    	std::cout << "Could not connect to " << m_host_address << ". Trying again in 1 second" << std::endl;
		sleep(1);
    }

    boost::shared_ptr<Connection> conn = boost::make_shared<TcpipConnection>(sock_fd);

    // some handshake
    conn->write("Hello");
    std::string ans = conn->read();

    return conn;
}

}  // namespace stream
