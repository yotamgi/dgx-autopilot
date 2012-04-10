#include <gtest/gtest.h>
#include <stream/util/udpip_connection.h>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

void other() {
	stream::UdpipConnectionFactory f(4444, "127.0.0.1", 5555, "127.0.0.1");
	boost::shared_ptr<stream::Connection> conn = f.get_connection();

	conn->write("Hello");
	std::string data = conn->read();
	ASSERT_EQ(data, "Bye");
}

TEST(udpip, basic) {
	stream::UdpipConnectionFactory f(5555, "127.0.0.1", 4444, "127.0.0.1");
	boost::shared_ptr<stream::Connection> conn = f.get_connection();
	boost::thread t(other);
	std::string data = conn->read();
	ASSERT_EQ(data, "Hello");
	conn->write("Bye");
	t.join();
}

