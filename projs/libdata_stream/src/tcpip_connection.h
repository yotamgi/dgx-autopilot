#ifndef TCPIP_CONNECTION_H_
#define TCPIP_CONNECTION_H_

#include <boost/shared_ptr.hpp>
#include "connection.h"

namespace stream {

class TcpipConnection : public Connection {
public:
	TcpipConnection(int sock_fd);
	~TcpipConnection();

	void write(std::string data);
	std::string read();

private:
	int m_sock_fd;
};

class TcpipServer {
public:
	TcpipServer(std::string listen_address, size_t port);
	~TcpipServer();

	boost::shared_ptr<TcpipConnection> wait_for_connection(float max_time);

private:
	int m_server_sock;
};

class TcpipClient {
public:
	TcpipClient(std::string host_address, size_t host_port);

	boost::shared_ptr<TcpipConnection> connect();
private:
	std::string m_host_address;
	size_t m_host_port;
};


}  // namespace stream


#endif /* TCPIP_CONNECTION_H_ */
