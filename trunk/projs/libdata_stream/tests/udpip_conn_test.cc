#include <gtest/gtest.h>
#include <stream/util/udpip_connection.h>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

typedef boost::shared_ptr<stream::Connection> conn_ptr;

void other() {
	stream::UdpipConnectionFactory f(4444, "127.0.0.1", 5555, "127.0.0.1");
	conn_ptr conn_a = f.get_connection();
	conn_ptr conn_b = f.get_connection();

	conn_a->write("Hello");
	std::string data = conn_a->read();
	ASSERT_EQ(data, "Hi");

	data = conn_b->read();
	ASSERT_EQ(data, "Bye");
	conn_b->write("ByeBye");

}

TEST(udpip, basic) {
	stream::UdpipConnectionFactory f(5555, "127.0.0.1", 4444, "127.0.0.1");
	conn_ptr conn_a = f.get_connection();
	conn_ptr conn_b = f.get_connection();
	boost::thread t(other);
	std::string data = conn_a->read();
	ASSERT_EQ(data, "Hello");
	conn_a->write("Hi");

	conn_b->write("Bye");
	data = conn_b->read();
	ASSERT_EQ(data, "ByeBye");
	t.join();
}

TEST(udpip, uniqueness) {
	stream::UdpipConnectionFactory f_a(2000, "127.0.0.1", 3000);
	stream::UdpipConnectionFactory f_b(3000, "127.0.0.1", 2000);
	std::vector<std::pair<conn_ptr, conn_ptr> > conn_pairs;

	for (size_t i=0; i<100; i++) {
		conn_ptr conn_a = f_a.get_connection();
		conn_ptr conn_b = f_b.get_connection();
		conn_pairs.push_back(std::make_pair(conn_a, conn_b));
	}

	for (size_t i=0; i<100; i++) {
		std::stringstream ss;
		ss << i << " is my number";
		conn_pairs[i].second->write(ss.str());
		ASSERT_EQ(conn_pairs[i].first->read(), ss.str());
	}

}


