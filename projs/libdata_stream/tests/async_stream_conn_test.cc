#include <stream/async_stream_connection.h>
#include <stream/util/udpip_connection.h>
#include <stream/stream_utils.h>

#include <gtest/gtest.h>
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/make_shared.hpp>
#include <boost/assign/list_of.hpp>

void echo_stream_server() {

	using namespace stream;
	typedef boost::shared_ptr<AsyncStreamConnection::RecvStream> recv_stream_ptr;
	typedef boost::shared_ptr<AsyncStreamConnection::SendStream> send_stream_ptr;

	//
	// The Recv Streams
	//

	boost::shared_ptr<AsyncStreamConnection::RecvPopStream<int> > recv_int_pop =
			boost::make_shared<AsyncStreamConnection::RecvPopStream<int> >();

	boost::shared_ptr<PushToPopConv<int> > recv_int_push =
			boost::make_shared<PushToPopConv<int> >(0);

	boost::shared_ptr<AsyncStreamConnection::RecvPopStream<double> > recv_double_pop =
			boost::make_shared<AsyncStreamConnection::RecvPopStream<double> >();

	boost::shared_ptr<PushToPopConv<bool> > recv_control =
			boost::make_shared<PushToPopConv<bool> >(true);

	AsyncStreamConnection::recv_streams_t recv_streams = boost::assign::list_of
		((recv_stream_ptr)recv_int_pop)
		((recv_stream_ptr)boost::make_shared<AsyncStreamConnection::RecvPushStream<int> >(recv_int_push))
		((recv_stream_ptr)recv_double_pop)
		((recv_stream_ptr)boost::make_shared<AsyncStreamConnection::RecvPushStream<bool> >(recv_control));


	//
	// The Send Streams
	//

	boost::shared_ptr<PushToPopConv<int> > send_int_pop =
			boost::make_shared<PushToPopConv<int> >(0);

	boost::shared_ptr<AsyncStreamConnection::SendPushStream<int> > send_int_push =
			boost::make_shared<AsyncStreamConnection::SendPushStream<int> >();
	send_int_push->set_data(4);

	boost::shared_ptr<PushToPopConv<double> > send_double_pop =
			boost::make_shared<PushToPopConv<double> >(0.0);

	AsyncStreamConnection::send_streams_t send_streams = boost::assign::list_of
		((send_stream_ptr)boost::make_shared<AsyncStreamConnection::SendPopStream<int> >(send_int_pop))
		((send_stream_ptr)send_int_push)
		((send_stream_ptr)boost::make_shared<AsyncStreamConnection::SendPopStream<double> >(send_double_pop));

	//
	// The Connection
	//

	// creating the udp connection stuff
	boost::shared_ptr<UdpipConnectionFactory> conn_factory =
			boost::make_shared<UdpipConnectionFactory>(5555, "127.0.0.1", 4444, "127.0.0.1");

	// create the async stream connection
	AsyncStreamConnection c(send_streams,
									recv_streams,
									conn_factory,
									false,
									1000.);
	c.start();

	//
	// The logic
	//

	while (recv_control->get_data()) {
		send_int_pop->set_data(recv_int_pop->get_data());
		send_int_push->set_data(recv_int_push->get_data());
		send_double_pop->set_data(recv_double_pop->get_data());
	}
}

TEST(async_conn, basic) {
	// run the helper
	boost::thread helper(echo_stream_server);

	using namespace stream;
	typedef boost::shared_ptr<AsyncStreamConnection::RecvStream> recv_stream_ptr;
	typedef boost::shared_ptr<AsyncStreamConnection::SendStream> send_stream_ptr;

	//
	// The recv streams
	//

	boost::shared_ptr<AsyncStreamConnection::RecvPopStream<int> > recv_int_pop =
			boost::make_shared<AsyncStreamConnection::RecvPopStream<int> >();

	boost::shared_ptr<PushToPopConv<int> > recv_int_push =
			boost::make_shared<PushToPopConv<int> >(0);

	boost::shared_ptr<AsyncStreamConnection::RecvPopStream<double> > recv_double_pop =
			boost::make_shared<AsyncStreamConnection::RecvPopStream<double> >();

	AsyncStreamConnection::recv_streams_t recv_streams = boost::assign::list_of
		((recv_stream_ptr)recv_int_pop)
		((recv_stream_ptr)boost::make_shared<AsyncStreamConnection::RecvPushStream<int> >(recv_int_push))
		((recv_stream_ptr)recv_double_pop);


	//
	// The send streams
	//

	boost::shared_ptr<PushToPopConv<int> > send_int_pop =
			boost::make_shared<PushToPopConv<int> >(0);

	boost::shared_ptr<AsyncStreamConnection::SendPushStream<int> > send_int_push =
			boost::make_shared<AsyncStreamConnection::SendPushStream<int> >();
	send_int_push->set_data(4);

	boost::shared_ptr<PushToPopConv<double> > send_double_pop =
			boost::make_shared<PushToPopConv<double> >(0.0);

	boost::shared_ptr<AsyncStreamConnection::SendPushStream<bool> > send_control =
			boost::make_shared<AsyncStreamConnection::SendPushStream<bool> >();
	send_control->set_data(true);

	AsyncStreamConnection::send_streams_t send_streams = boost::assign::list_of
		((send_stream_ptr)boost::make_shared<AsyncStreamConnection::SendPopStream<int> >(send_int_pop))
		((send_stream_ptr)send_int_push)
		((send_stream_ptr)boost::make_shared<AsyncStreamConnection::SendPopStream<double> >(send_double_pop))
		((send_stream_ptr)send_control);

	//
	// The connection
	//

	// creating the udp connection stuff
	boost::shared_ptr<stream::UdpipConnectionFactory> conn_factory =
			boost::make_shared<stream::UdpipConnectionFactory>(4444, "127.0.0.1", 5555, "127.0.0.1");

	// create the async stream connection
	stream::AsyncStreamConnection c(send_streams,
									recv_streams,
									conn_factory,
									true,
									1000.);
	c.start();

	//
	// The logic
	//

	for (size_t i=0; i<50; i++) {
		send_int_pop->set_data(i);
		send_int_push->set_data(i*2);
		send_double_pop->set_data((double)i*3);
		usleep(30000);
		ASSERT_EQ(recv_int_pop->get_data(), i);
		ASSERT_EQ(recv_int_push->get_data(), i*2);
		ASSERT_EQ(recv_double_pop->get_data(), (double)i*3);
	}


	// stop the helper
	send_control->set_data(false);
	helper.join();
}



