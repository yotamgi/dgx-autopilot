#include <stream/generators.h>
#include <stream/stream_connection.h>
#include <stream/util/tcpip_connection.h>
#include <gtest/gtest.h>
#include <boost/make_shared.hpp>
#include <boost/thread.hpp>
#include <iostream>

class SimpleStream : public stream::DataGenerator<int> {
public:
	SimpleStream(): m_num(0) {}
	int get_data() {return m_num++;}
private:
	int m_num;
};

void BasicHelper() {
	boost::shared_ptr<stream::ConnectionFactory> client =
				boost::make_shared<stream::TcpipClient>("localhost", 0x6666);
	stream::StreamConnection strcon(client);

	strcon.export_stream<int>(boost::make_shared<SimpleStream>(), "stam1");
	strcon.run();

	boost::shared_ptr<stream::DataGenerator<int> > s = strcon.import_stream<int>("stam2");
	EXPECT_EQ(s->get_data(), 0);
	EXPECT_EQ(s->get_data(), 1);

	usleep(1000000);
	strcon.stop();
}

TEST(stream_conn, basic) {

	// run the helper
	boost::thread t(BasicHelper);

	// create connection
	boost::shared_ptr<stream::ConnectionFactory> server =
			boost::make_shared<stream::TcpipServer>("localhost", 0x6666);
	stream::StreamConnection strcon(server);

	// export stream
	strcon.export_stream<int>(boost::make_shared<SimpleStream>(), "stam2");
	strcon.run();

	// import stream
	boost::shared_ptr<stream::DataGenerator<int> > s = strcon.import_stream<int>("stam1");
	EXPECT_EQ(s->get_data(), 0);
	EXPECT_EQ(s->get_data(), 1);

	usleep(100000);
	t.join();
	strcon.stop();
}
