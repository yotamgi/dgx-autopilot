#include <gtest/gtest.h>
#include <stream/func_filter.h>
#include <stream/util/lin_algebra.h>
#include <stream/stream_utils.h>


int basic_filt(int in) {
	return in * 2;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Func Pop Stream Tests
///////////////////////////////////////////////////////////////////////////////////////////////

TEST(FuncFilter, pop_basic) {
	boost::shared_ptr<stream::PushToPopConv<int> > stream(new stream::PushToPopConv<int>(0));

	stream::FuncPopFilter<int> filtered_stream(stream, basic_filt);

	stream->set_data(0);
	ASSERT_EQ(filtered_stream.get_data(), 0);

	stream->set_data(1);
	ASSERT_EQ(filtered_stream.get_data(), 2);
}


TEST(FuncFilter, pop_multitype) {
	boost::shared_ptr<stream::PushToPopConv<lin_algebra::vec3f> > stream(
			new stream::PushToPopConv<lin_algebra::vec3f>(lin_algebra::vec3f())
	);

	stream::FuncPopFilter<lin_algebra::vec3f, float> filtered_stream(stream, lin_algebra::vec_len<lin_algebra::vec3f>);

	lin_algebra::vec3f v;
	v[0] = 100.; v[1] = 0.; v[2] = 0.;
	stream->set_data(v);
	ASSERT_NEAR(filtered_stream.get_data(), 100., 0.01);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Func Push Stream Tests
///////////////////////////////////////////////////////////////////////////////////////////////

TEST(FuncFilter, push_basic) {
	boost::shared_ptr<stream::PushToPopConv<int> > stream(new stream::PushToPopConv<int>(0));

	stream::FuncPushFilter<int> filtered_stream(stream, basic_filt);

	filtered_stream.set_data(0);
	ASSERT_EQ(stream->get_data(), 0);

	filtered_stream.set_data(1);
	ASSERT_EQ(stream->get_data(), 2);
}


TEST(FuncFilter, push_multitype) {
	boost::shared_ptr<stream::PushToPopConv<float> > stream(
			new stream::PushToPopConv<float>(0.)
	);

	stream::FuncPushFilter<lin_algebra::vec3f, float> filtered_stream(stream, lin_algebra::vec_len<lin_algebra::vec3f>);

	lin_algebra::vec3f v;
	v[0] = 100.; v[1] = 0.; v[2] = 0.;
	filtered_stream.set_data(v);
	ASSERT_NEAR(stream->get_data(), 100., 0.01);
}

