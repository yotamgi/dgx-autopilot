#include "udpip_connection.h"
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <boost/make_shared.hpp>

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

namespace stream {

UdpipConnection::UdpipConnection(int sock_fd, sockaddr_in address):m_sock_fd(sock_fd), m_address(address)
{}

static int validate(int ret, int expected, std::string funcname) {
	if (ret  == -1) {
		std::stringstream err;
		err <<  funcname << " failed with erno " << errno;
		throw ConnectionExceptioin(err.str());
	} else if ((ret != expected) && (expected != -1)) {
		std::stringstream err;
		err << funcname << " returned " << ret << " when it should have " << expected;
		throw ConnectionExceptioin(err.str());
	}
	return ret;
}

void UdpipConnection::write(std::string data) {

	if (data.size() > MAX_UDP_PACKET_DATA_SIZE) {
		throw stream::ConnectionExceptioin("Can't send udp this large udp packet");
	}
    validate(
    	::sendto(m_sock_fd, data.c_str(), data.size(), 0, (sockaddr*)&m_address, sizeof(m_address)),
    	 data.size(), "sendto"
    );
}

std::string UdpipConnection::read() {

	// first, read the size of the data
	std::string str_size;
	sockaddr_in from;
	int from_len = sizeof(from);

	char buff[MAX_UDP_PACKET_DATA_SIZE];
	int read_bytes = validate(::recvfrom(m_sock_fd, buff, MAX_UDP_PACKET_DATA_SIZE, 0, (sockaddr*)&from, (socklen_t*)&from_len),
			-1, "recvfrom"
	);
	return std::string(buff, read_bytes);
}

UdpipConnectionFactory::UdpipConnectionFactory(size_t my_port, std::string my_addr, size_t to_port, std::string to_addr) {

	// create the socket address struct
	bzero(&m_sockaddr, sizeof(m_sockaddr));

	m_sockaddr.sin_addr.s_addr = inet_addr(my_addr.c_str());
	m_sockaddr.sin_port = htons(my_port); // htons for network byte order
	m_sockaddr.sin_family = AF_INET;

	m_to_addr.sin_addr.s_addr = inet_addr(to_addr.c_str());
	m_to_addr.sin_port = htons(to_port);
	m_to_addr.sin_family = AF_INET;
}

boost::shared_ptr<Connection> UdpipConnectionFactory::get_connection() {
	int s = socket(AF_INET, SOCK_DGRAM, 0);

	validate(bind(s, (sockaddr*)&m_sockaddr, sizeof(m_sockaddr)), 0, "bind");

	return boost::make_shared<UdpipConnection>(s, m_to_addr);
}

UdpipConnectionFactory::~UdpipConnectionFactory() {}

} // namepsace stream
