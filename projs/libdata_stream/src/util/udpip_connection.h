#ifndef UPDIP_CONNECTION_H_
#define UPDIP_CONNECTION_H_

#include <boost/shared_ptr.hpp>
#include <string>
#include <stream/connection.h>
#include <netinet/in.h>

namespace stream {

class UdpipConnection : public Connection {
public:
	UdpipConnection(int sock_fd, sockaddr_in address);
	virtual ~UdpipConnection() {}

	void write(std::string data);
	std::string read();

	int fd() { return m_sock_fd; }

private:
	int m_sock_fd;
	sockaddr_in m_address;

	static const size_t MAX_UDP_PACKET_DATA_SIZE = 65507;
};

/**
 * Udpip connection factory.
 * Gets the source and dest addresses and can create as many udp connection to
 * the dest as you want.
 * This class assures you that the other side will get the connected sockets at
 * the same order you got them, so your first socket will talk to his socket
 * and etc.
 */
class UdpipConnectionFactory : public ConnectionFactory {
public:
	UdpipConnectionFactory(size_t to_port_begin,
						   std::string to_addr,
						   size_t my_port_begin,
						   std::string my_addr = "");

	virtual ~UdpipConnectionFactory();

	boost::shared_ptr<Connection> get_connection();

private:
	size_t m_port_offset;
	const size_t m_my_port_begin;
	const size_t m_to_port_begin;

	struct sockaddr_in m_sockaddr;
	struct sockaddr_in m_to_addr;
};

} // namepsace stream

#endif /* UPDIP_CONNECTION_H_ */
