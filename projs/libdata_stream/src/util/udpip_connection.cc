#include "udpip_connection.h"
#include <sstream>

#include <arpa/inet.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

namespace stream {

UdpipConnection::UdpipConnection(int sock_fd, sockaddr_in address):m_sock_fd(sock_fd), m_address(address)
{}

void UdpipConnection::write(std::string data) {
	std::stringstream ss;
	ss << data.size() << ";" << data;

	if (ss.str().size() > MAX_UDP_PACKET_DATA_SIZE) {
		throw stream::ConnectionExceptioin("Can't send udp this large udp packet");
	}
    if (::sendto(m_sock_fd, ss.str().c_str(), ss.str().size(), 0, &m_address, sizeof(m_address))==-1) {
    	throw stream::ConnectionExceptioin("udp sendto failed");
    }
}

std::string UdpipConnection::read() {
	// first, read the size of the data
	std::string str_size;
	char n;
	do {
		if (::recvfrom(m_sock_fd, &n, 1, 0, m_address, sizeof(m_address)) != 1) {
			throw ConnectionExceptioin("Could not recvfrom from udp socket");
		}
		str_size += std::string(&n,1);
	} while (n != ';');

	// understand the size
	size_t size = boost::lexical_cast<size_t>(str_size.substr(0, str_size.size()-1));

	// read the data
	char buff[size];
	if (::recvfrom(m_sock_fd, buff, size, 0, m_address, sizeof(m_address)) != (int)size) {
		throw ConnectionExceptioin("Could not recvfrom from socket");
	}
	return std::string(buff, size);
}

UdpipConnectionFactory::UdpipConnectionFactory(std::string address, size_t port) {

	// create the socket address struct
	m_sockaddr.sin_family = AF_INET;
	m_sockaddr.sin_port = htons(port); // htons for network byte order
	m_sockaddr.sin_addr.s_addr = inet_addr(address.c_str());
}

boost::shared_ptr<Connection> UdpipConnectionFactory::get_connection() {
	int socket = socket(AF_INET, SOCK_STREAM, 0);
	bind(socket, m_sockaddr, sizeof(m_sockaddr));
	return boost::make_shared<UdpipConnection>(socket, m_sockaddr);
}

} // namepsace stream
