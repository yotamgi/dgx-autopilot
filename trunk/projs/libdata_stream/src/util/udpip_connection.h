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

class UdpipConnectionFactory : public ConnectionFactory {
public:
	UdpipConnectionFactory(size_t my_port, std::string my_addr, size_t to_port, std::string to_addr);
	virtual ~UdpipConnectionFactory();

	boost::shared_ptr<Connection> get_connection();

private:
	struct sockaddr_in m_sockaddr;
	struct sockaddr_in m_to_addr;
};

} // namepsace stream

#endif /* UPDIP_CONNECTION_H_ */
