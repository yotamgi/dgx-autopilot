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
	virtual ~UdpipConnection();

	void write(std::string data);
	std::string read();

	int fd();

private:
	int m_sock_fd;
	sockaddr_in m_address;

	static const size_t MAX_UDP_PACKET_DATA_SIZE = 65507;
};

class UdpipConnectionFactory : public ConnectionFactory {
public:
	UdpipConnectionFactory(std::string address, size_t port);
	virtual ~UdpipConnectionFactory();

	boost::shared_ptr<Connection> get_connection();
private:
	struct sockaddr_in m_sockaddr;
};

} // namepsace stream

#endif /* UPDIP_CONNECTION_H_ */
