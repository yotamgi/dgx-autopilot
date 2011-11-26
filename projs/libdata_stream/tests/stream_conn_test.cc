#include <stream/data_pop_stream.h>
#include <stream/stream_connection.h>
#include <stream/util/tcpip_connection.h>
#include <gtest/gtest.h>
#include <boost/make_shared.hpp>
#include <boost/thread.hpp>
#include <boost/format.hpp>
#include <iostream>
#include <vector>

class SimpleStream : public stream::DataPopStream<int> {
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

	boost::shared_ptr<stream::DataPopStream<int> > s = strcon.import_stream<int>("stam2");
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
	boost::shared_ptr<stream::DataPopStream<int> > s = strcon.import_stream<int>("stam1");
	EXPECT_EQ(s->get_data(), 0);
	EXPECT_EQ(s->get_data(), 1);

	usleep(100000);
	t.join();
	strcon.stop();
}

void StressHelper(size_t howmany) {
	// create connection
	boost::shared_ptr<stream::ConnectionFactory> client =
				boost::make_shared<stream::TcpipClient>("localhost", 0x6666);
	stream::StreamConnection strcon(client);

	// export 100 streams
	for (size_t i=0; i<howmany; i++) {
		strcon.export_stream<int>(boost::make_shared<SimpleStream>(), (boost::format("stream%d") % i).str());
	}
	strcon.run();

	// import all streams
	std::vector<boost::shared_ptr<stream::DataPopStream<int> > > streams;
	for (size_t i=0; i<howmany; i++) {
		streams.push_back(
				strcon.import_stream<int>((boost::format("stream%d") % i).str())
		);
	}

	// Read from them and validate output
	for (int i=0; i<1000; i++) {
		for (size_t stream=0; stream < howmany; stream++) {
			EXPECT_EQ(streams[stream]->get_data(), (int)i);
		}
	}

	usleep(300000);
	strcon.stop();
	usleep(100000);
}


TEST(stream_conn, stress) {

	const size_t NUM_OF_STREAMS = 10;

	// run the helper
	boost::thread t(StressHelper, NUM_OF_STREAMS);

	// create connection
	boost::shared_ptr<stream::ConnectionFactory> server =
			boost::make_shared<stream::TcpipServer>("localhost", 0x6666);
	stream::StreamConnection strcon(server);

	// export 100 streams
	for (size_t i=0; i<NUM_OF_STREAMS; i++) {
		strcon.export_stream<int>(boost::make_shared<SimpleStream>(), (boost::format("stream%d") % i).str());
	}
	strcon.run();

	// import all streams
	std::vector<boost::shared_ptr<stream::DataPopStream<int> > > streams;
	for (size_t i=0; i<NUM_OF_STREAMS; i++) {

		streams.push_back(
				strcon.import_stream<int>((boost::format("stream%d") % i).str())
		);
	}

	// Read from them and validate output
	for (int i=0; i<1000; i++) {
		for (size_t stream=0; stream < NUM_OF_STREAMS; stream++) {
			EXPECT_EQ(streams[stream]->get_data(), (int)i);
		}
	}

	usleep(300000);
	t.join();
	strcon.stop();

}

void ListHelper(size_t howmany) {
	// create connection
	boost::shared_ptr<stream::ConnectionFactory> client =
				boost::make_shared<stream::TcpipClient>("localhost", 0x6666);
	stream::StreamConnection strcon(client);

	// export some streams
	for (size_t i=0; i<howmany; i++) {
		strcon.export_stream<int>(boost::make_shared<SimpleStream>(), (boost::format("stream%d") % i).str());
	}
	strcon.run();

	usleep(300000);
	strcon.stop();
	usleep(100000);
}


TEST(stream_conn, list) {

	const size_t NUM_STREAMS = 100;

	// run the helper
	boost::thread t(ListHelper, NUM_STREAMS);

	// create connection
	boost::shared_ptr<stream::ConnectionFactory> server =
			boost::make_shared<stream::TcpipServer>("localhost", 0x6666);
	stream::StreamConnection strcon(server);

	strcon.run();

	usleep(100000);

	std::vector<std::string> avail = strcon.list_avail();

	for (size_t i=0; i<NUM_STREAMS; i++) {
		bool flag = false;
		for (size_t i=0; i<NUM_STREAMS; i++) {
			flag |= (avail[i] == (boost::format("stream%d") % i).str());
		}
		ASSERT_EQ(flag, true);
	}
	strcon.stop();
	t.join();
}

