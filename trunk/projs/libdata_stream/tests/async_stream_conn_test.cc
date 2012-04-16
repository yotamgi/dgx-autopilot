#include <stream/async_stream_connection.h>
#include <stream/util/udpip_connection.h>
#include <stream/stream_utils.h>

#include <gtest/gtest.h>
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/make_shared.hpp>
#include <boost/assign/list_of.hpp>

void basic_helper() {

	// creating the streams list
	boost::shared_ptr<stream::AsyncStreamConnection::RecvPopStream<int> > int_pop =
			boost::make_shared<stream::AsyncStreamConnection::RecvPopStream<int> >();

	boost::shared_ptr<stream::AsyncStreamConnection::RecvPushStream<int> > int_push =
			boost::make_shared<stream::AsyncStreamConnection::RecvPushStream<int> >(
					boost::make_shared<stream::PushToPopConv<int> >(0)
			);
	boost::shared_ptr<stream::AsyncStreamConnection::RecvPopStream<double> > double_pop =
			boost::make_shared<stream::AsyncStreamConnection::RecvPopStream<double> >();

	stream::AsyncStreamConnection::recv_streams_t streams = boost::assign::list_of
			((boost::shared_ptr<stream::AsyncStreamConnection::RecvStream>)int_pop)
			((boost::shared_ptr<stream::AsyncStreamConnection::RecvStream>)int_push)
			((boost::shared_ptr<stream::AsyncStreamConnection::RecvStream>)double_pop);

	// creating the udp connection stuff
	boost::shared_ptr<stream::UdpipConnectionFactory> conn_factory =
			boost::make_shared<stream::UdpipConnectionFactory>(5555, "127.0.0.1", 4444, "127.0.0.1");

	// create the async stream connection
	stream::AsyncStreamConnection c(stream::AsyncStreamConnection::send_streams_t(),
									streams,
									conn_factory,
									false,
									1.0);
	c.start();
	while (true);
}

TEST(async_conn, basic) {
	// run the helper
	boost::thread helper(basic_helper);

	// creating the streams list
	boost::shared_ptr<stream::AsyncStreamConnection::SendPopStream<int>  > int_pop =
			boost::make_shared<stream::AsyncStreamConnection::SendPopStream<int> >(
					boost::make_shared<stream::PushToPopConv<int> >(0)
			);

	boost::shared_ptr<stream::AsyncStreamConnection::SendPushStream<int> > int_push =
			boost::make_shared<stream::AsyncStreamConnection::SendPushStream<int> >();
	int_push->set_data(4);

	boost::shared_ptr<stream::AsyncStreamConnection::SendPopStream<double> > double_pop =
			boost::make_shared<stream::AsyncStreamConnection::SendPopStream<double> >(
					boost::make_shared<stream::PushToPopConv<double> >(0.0)
			);

	stream::AsyncStreamConnection::send_streams_t streams = boost::assign::list_of
			((boost::shared_ptr<stream::AsyncStreamConnection::SendStream>)int_pop)
			((boost::shared_ptr<stream::AsyncStreamConnection::SendStream>)int_push)
			((boost::shared_ptr<stream::AsyncStreamConnection::SendStream>)double_pop);


	// creating the udp connection stuff
	boost::shared_ptr<stream::UdpipConnectionFactory> conn_factory =
			boost::make_shared<stream::UdpipConnectionFactory>(4444, "127.0.0.1", 5555, "127.0.0.1");

	// create the async stream connection
	stream::AsyncStreamConnection c(streams,
									stream::AsyncStreamConnection::recv_streams_t(),
									conn_factory,
									true,
									1.0);
	c.start();
	while (true);
}



