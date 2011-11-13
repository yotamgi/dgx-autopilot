#include <stream/tcpip_connection.h>
#include <gtest/gtest.h>
#include <boost/thread.hpp>

using namespace stream;

class TestHelper {
public:
	TestHelper() {}
	void connect() {
		TcpipClient client("localhost", 0x6667);
		m_conn = client.connect();
		std::cout << "Client ready!" << std::endl;
	}

	void send(std::string s) { m_conn->write(s); }
	std::string recv() { return m_conn->read(); }
private:
	boost::shared_ptr<Connection> m_conn;
};

TEST(tcpip, functional) {
	TcpipServer server("localhost", 0x6667);
	TestHelper right;

	boost::thread t(&TestHelper::connect, &right);
	boost::shared_ptr<TcpipConnection> left = server.wait_for_connection(0.0);
	std::cout << "Server finished" << std::endl;
	t.join();

	std::cout << "Two connections are ready!" << std::endl;

	left->write("Hello");
	std::string s = right.recv();
	std::cout << s << std::endl;
	ASSERT_EQ(std::string("Hello"), s);
}
