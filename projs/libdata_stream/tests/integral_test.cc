#include <iostream>
#include <gtest/gtest.h>
#include <sys/time.h>
#include <cstdlib>
#include "data_pop_stream.h"
#include "filters/integral_filter.h"
#include "util/time.h"
#include "util/lin_algebra.h"


/**
 * Dummy generator for testing the integral.
 * Returns the time since creation in seconds
 */
class DummyGen : public stream::DataPopStream<float> {
public:
	DummyGen() {
		gettimeofday(&m_start_time, NULL);
	}

	float get_data() {
		timeval curr;
		gettimeofday(&curr, NULL);

		return (curr.tv_usec - m_start_time.tv_usec)/1000000. + curr.tv_sec - m_start_time.tv_sec;
	}

private:
	timeval m_start_time;
};

/**
 * Dummy generator for testing the integral.
 * Returns vector with those values:
 * 	- x: constant 1.0
 *  - y: time since creation
 *  - z: constant 1.0
 */
class DummyVecGen : public stream::DataPopStream<lin_algebra::vec3f> {
public:
	DummyVecGen() {
		gettimeofday(&m_start_time, NULL);
	}

	lin_algebra::vec3f get_data() {
		timeval curr;
		gettimeofday(&curr, NULL);

		lin_algebra::vec3f ans;
		ans[0] = 1.f;
		ans[1] = (curr.tv_usec - m_start_time.tv_usec)/1000000. + curr.tv_sec - m_start_time.tv_sec;
		ans[2] = 1.f;

		return ans;
	}

private:
	timeval m_start_time;
};



TEST(IntegralTest, vec_stress_test) {

	boost::shared_ptr<stream::DataPopStream<lin_algebra::vec3f> > a(new DummyVecGen);
	lin_algebra::vec3f zeros;
	zeros << 0. << 0. << 0.;
	stream::filters::IntegralFilter<lin_algebra::vec3f> integ(a, zeros);

	Timer t;

	// for three seconds
	while (t.passed() < 1.0f) {
		integ.get_data();
	}

	// validate that the integral is as expected
	ASSERT_NEAR(integ.get_data()[0], t.passed(), 0.0001);
	ASSERT_NEAR(integ.get_data()[1], t.passed()*t.passed()/2., 0.001);
	ASSERT_NEAR(integ.get_data()[2], t.passed(), 0.001);
}



TEST(IntegralTest, vec_random_test) {

	boost::shared_ptr<DummyVecGen> a(new DummyVecGen);
	lin_algebra::vec3f zeros;
	zeros << 0. << 0. << 0.;
	stream::filters::IntegralFilter<lin_algebra::vec3f> integ(a, zeros);


	Timer t;

	// for three seconds
	while (t.passed() < 1.0f) {
		usleep(rand()%1000);
		integ.get_data();
	}

	// validate that the integral is as expected
	ASSERT_NEAR(integ.get_data()[0], t.passed(), 0.001);
	ASSERT_NEAR(integ.get_data()[1], t.passed()*t.passed()/2., 0.001);
	ASSERT_NEAR(integ.get_data()[2], t.passed(), 0.001);
}


TEST(IntegralTest, stress_test) {

	boost::shared_ptr<DummyGen> a(new DummyGen);
	stream::filters::IntegralFilter<float> integ(a, 0.);

	Timer t;

	// for three seconds
	while (t.passed() < 1.0f) {
		integ.get_data();
	}

	// validate that the integral is as expected
	ASSERT_NEAR(integ.get_data(), t.passed()*t.passed()/2., 0.0001);
}



TEST(IntegralTest, random_test) {

	boost::shared_ptr<DummyGen> a(new DummyGen);
	stream::filters::IntegralFilter<float> integ(a, 0.);

	Timer t;

	// for three seconds
	while (t.passed() < 1.0f) {
		usleep(rand()%1000);
		integ.get_data();
	}

	// validate that the integral is as expected
	ASSERT_NEAR(integ.get_data(), t.passed()*t.passed()/2., 0.001);
}
