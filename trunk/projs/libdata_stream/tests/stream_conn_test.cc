#include <stream/data_pop_stream.h>
#include <stream/data_push_stream.h>
#include <stream/stream_connection.h>
#include <stream/util/tcpip_connection.h>
#include <gtest/gtest.h>
#include <boost/make_shared.hpp>
#include <boost/thread.hpp>
#include <boost/format.hpp>
#include <iostream>
#include <vector>

class SimplePopStream : public stream::DataPopStream<int> {
public:
	SimplePopStream(): m_num(0) {}
	int get_data() {return m_num++;}
private:
	int m_num;
};

class SimplePushStream : public stream::DataPushStream<int> {
public:
	SimplePushStream(): m_num(0) {}
	void set_data(const int& data) {
		EXPECT_EQ(m_num, data);
		m_num++;
	}
private:
	int m_num;
};


void BasicPopHelper() {
	boost::shared_ptr<stream::ConnectionFactory> client =
				boost::make_shared<stream::TcpipClient>("localhost", 0x6666);
	stream::StreamConnection strcon(client);

	strcon.export_pop_stream<int>(boost::make_shared<SimplePopStream>(), "stam1");
	strcon.run();

	boost::shared_ptr<stream::DataPopStream<int> > s = strcon.import_pop_stream<int>("stam2");
	EXPECT_EQ(s->get_data(), 0);
	EXPECT_EQ(s->get_data(), 1);

	usleep(1000000);
	strcon.stop();
}

TEST(stream_conn, pop_basic) {

	// run the helper
	boost::thread t(BasicPopHelper);

	// create connection
	boost::shared_ptr<stream::ConnectionFactory> server =
			boost::make_shared<stream::TcpipServer>("localhost", 0x6666);
	stream::StreamConnection strcon(server);

	// export stream
	strcon.export_pop_stream<int>(boost::make_shared<SimplePopStream>(), "stam2");
	strcon.run();

	// import stream
	boost::shared_ptr<stream::DataPopStream<int> > s = strcon.import_pop_stream<int>("stam1");
	EXPECT_EQ(s->get_data(), 0);
	EXPECT_EQ(s->get_data(), 1);

	usleep(100000);
	t.join();
	strcon.stop();
}

void BasicPushHelper() {
	boost::shared_ptr<stream::ConnectionFactory> client =
				boost::make_shared<stream::TcpipClient>("localhost", 0x6666);
	stream::StreamConnection strcon(client);

	strcon.export_push_stream<int>(boost::make_shared<SimplePushStream>(), "stam");
	strcon.run();

	boost::shared_ptr<stream::DataPushStream<int> > s = strcon.import_push_stream<int>("stam");
	s->set_data(0);
	s->set_data(1);

	usleep(1000000);
	strcon.stop();
}

TEST(stream_conn, push_basic) {

	// run the helper
	boost::thread t(BasicPushHelper);

	// create connection
	boost::shared_ptr<stream::ConnectionFactory> server =
			boost::make_shared<stream::TcpipServer>("localhost", 0x6666);
	stream::StreamConnection strcon(server);

	// export stream
	strcon.export_push_stream<int>(boost::make_shared<SimplePushStream>(), "stam");
	strcon.run();

	// import stream
	boost::shared_ptr<stream::DataPushStream<int> > s = strcon.import_push_stream<int>("stam");
	s->set_data(0);
	s->set_data(1);

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
		strcon.export_pop_stream<int>(boost::make_shared<SimplePopStream>(), (boost::format("pop-stream%d") % i).str());
		strcon.export_push_stream<int>(boost::make_shared<SimplePushStream>(), (boost::format("push-stream%d") % i).str());
	}
	strcon.run();
	sleep(1);

	// import all streams
	std::vector<boost::shared_ptr<stream::DataPopStream<int> > > pop_streams;
	std::vector<boost::shared_ptr<stream::DataPushStream<int> > > push_streams;
	for (size_t i=0; i<howmany; i++) {
		pop_streams.push_back(
				strcon.import_pop_stream<int>((boost::format("pop-stream%d") % i).str())
		);
		push_streams.push_back(
				strcon.import_push_stream<int>((boost::format("push-stream%d") % i).str())
		);
	}

	// Read from them and validate output
	for (int i=0; i<1000; i++) {
		for (size_t stream=0; stream < howmany; stream++) {
			push_streams[stream]->set_data(pop_streams[stream]->get_data());
		}
	}

	usleep(1000000);
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
		strcon.export_pop_stream<int>(boost::make_shared<SimplePopStream>(), (boost::format("pop-stream%d") % i).str());
		strcon.export_push_stream<int>(boost::make_shared<SimplePushStream>(), (boost::format("push-stream%d") % i).str());
	}
	strcon.run();
	sleep(1);

	// import all streams
	std::vector<boost::shared_ptr<stream::DataPopStream<int> > > pop_streams;
	std::vector<boost::shared_ptr<stream::DataPushStream<int> > > push_streams;
	for (size_t i=0; i<NUM_OF_STREAMS; i++) {

		pop_streams.push_back(
				strcon.import_pop_stream<int>((boost::format("pop-stream%d") % i).str())
		);
		push_streams.push_back(
				strcon.import_push_stream<int>((boost::format("push-stream%d") % i).str())
		);
	}

	// Read from them and validate output
	for (int i=0; i<1000; i++) {
		for (size_t stream=0; stream < NUM_OF_STREAMS; stream++) {
			push_streams[stream]->set_data(pop_streams[stream]->get_data());
		}
	}

	usleep(1000000);
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
		strcon.export_pop_stream<int>(boost::make_shared<SimplePopStream>(), (boost::format("stream%d") % i).str());
	}
	strcon.run();

	usleep(1000000);
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

