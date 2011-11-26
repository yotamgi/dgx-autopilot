#ifndef WATCH_TEST_CC_
#define WATCH_TEST_CC_

#include <stream/util/lin_algebra.h>
#include <stream/filters/watch_filter.h>
#include <gtest/gtest.h>

/**
 * Dummy generator for testing the integral.
 * Returns the time since creation in seconds
 */
class DummyWatchedGen : public stream::DataPopStream<int> {
public:
	DummyWatchedGen():m_counter(0) {}

	int get_data() {
		return m_counter++;
	}

private:
	size_t m_counter;
};

/**
 * Dummy generator for testing the integral.
 * Returns vector with those values:
 * 	- x: constant 1.0
 *  - y: time since creation
 *  - z: constant 1.0
 */
class DummyWatchedVecGen : public stream::DataPopStream<lin_algebra::vec3f> {
public:
	DummyWatchedVecGen():m_counter(0) {}

	lin_algebra::vec3f get_data() {
		lin_algebra::vec3f ans;
		ans[0] = (float) m_counter;
		ans[1] = (float) m_counter+1;
		ans[2] = (float) m_counter+2;
		return ans;
	}

private:
	size_t m_counter;
};

TEST(WatchTest, int_test) {

	boost::shared_ptr< stream::DataPopStream<int> > a(new DummyWatchedGen);
	stream::filters::WatchFilter<int> watch(a);
	boost::shared_ptr<stream::DataPopStream<int> > w = watch.get_watch_stream();

	for (size_t i=0; i<100; i++) {
		int ans = watch.get_data();

		for (size_t j=0; j<5; j++){
			ASSERT_EQ(ans, w->get_data());
		}
	}
}

TEST(WatchTest, vec_test) {

	boost::shared_ptr<stream::DataPopStream<lin_algebra::vec3f> > a(new DummyWatchedVecGen);
	stream::filters::WatchFilter<lin_algebra::vec3f> watch(a);
	boost::shared_ptr<stream::DataPopStream<lin_algebra::vec3f> > w = watch.get_watch_stream();

	for (size_t i=0; i<100; i++) {
		lin_algebra::vec3f ans = watch.get_data();
		ASSERT_NEAR(ans[0], w->get_data()[0], 0.001);
		ASSERT_NEAR(ans[1], w->get_data()[1], 0.001);
		ASSERT_NEAR(ans[2], w->get_data()[2], 0.001);
	}
}

#endif /* WATCH_TEST_CC_ */
