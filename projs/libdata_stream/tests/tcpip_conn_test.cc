#include <stream/tcpip_connection.h>
#include <gtest/gtest.h>
#include <boost/thread.hpp>

using namespace stream;

class TestHelper {
public:
	TestHelper() {}
	void connect() {
		TcpipClient client("localhost", 0x6666);
		m_conn = client.connect();
	}

	void send(std::string s) { m_conn->write(s); }
	std::string recv() { return m_conn->read(); }
private:
	boost::shared_ptr<Connection> m_conn;
};

TEST(tcpip, functional) {
	TcpipServer server("localhost", 0x6666);
	TestHelper right;

	boost::thread t(&TestHelper::connect, &right);
	boost::shared_ptr<TcpipConnection> left = server.wait_for_connection(0.0);
	t.join();

	std::cout << "Two connections are ready!" << std::endl;

	std::string test_str = "Hello";
	left->write(test_str);
	ASSERT_EQ(test_str, right.recv());

	test_str = "Hello to you too!";
	right.send(test_str);
	ASSERT_EQ(test_str, left->read());

	right.send(test_str);
	right.send(test_str);
	ASSERT_EQ(test_str, left->read());
	ASSERT_EQ(test_str, left->read());
}

TEST(tcpip, reconnect) {
	TcpipServer server("localhost", 0x6666);
	TestHelper right;

	boost::thread t(&TestHelper::connect, &right);
	boost::shared_ptr<TcpipConnection> left = server.wait_for_connection(0.0);
	t.join();

	std::cout << "Two connections are ready!" << std::endl;
}

