#include <stream/filters/fps_filter.h>
#include <gtest/gtest.h>
#include <stream/stream_utils.h>
#include <boost/shared_ptr.hpp>

TEST(FpsFilter, fps_basic) {
	const int DATA = 33;
	boost::shared_ptr<stream::PushToPopConv<int> > s(new stream::PushToPopConv<int>(0.));
	s->set_data(DATA);

	stream::filters::FpsFilter<int> filtered(s);

	int data;
	for (size_t i=0; i<100; i++) {
		usleep(10000); // 1/100 sec
		data = filtered.get_data();
	}
	ASSERT_EQ(data, DATA);
	ASSERT_NEAR(filtered.get_fps_stream()->get_data(), 100., 1.);
}

