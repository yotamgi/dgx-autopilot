
#include <iostream>
#include <gtest/gtest.h>
#include <sys/time.h>
#include <cstdlib>
#include "generators.h"
#include "filters/integral_filter.h"
#include "util/time.h"


/**
 * Dummy generator for testing the integral.
 * Returns the time since creation in seconds
 */
class DummyGen : public stream::DataGenerator<float> {
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
class DummyVecGen : public stream::VecGenerator<float,3> {
public:
	DummyVecGen() {
		gettimeofday(&m_start_time, NULL);
	}

	vector_t get_data() {
		timeval curr;
		gettimeofday(&curr, NULL);

		vector_t ans;
		ans[0] = 1.f;
		ans[1] = (curr.tv_usec - m_start_time.tv_usec)/1000000. + curr.tv_sec - m_start_time.tv_sec;
		ans[2] = 1.f;

		return ans;
	}

private:
	timeval m_start_time;
};



TEST(IntegralTest, vec_stress_test) {

	DummyVecGen a;
	stream::filters::VecIntegralFilter<float,3> integ(&a);

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

	DummyVecGen a;
	stream::filters::VecIntegralFilter<float,3> integ(&a);

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

	DummyGen a;
	stream::filters::IntegralFilter<float> integ(&a, 0.);

	Timer t;

	// for three seconds
	while (t.passed() < 1.0f) {
		integ.get_data();
	}

	// validate that the integral is as expected
	ASSERT_NEAR(integ.get_data(), t.passed()*t.passed()/2., 0.0001);
}



TEST(IntegralTest, random_test) {

	DummyGen a;
	stream::filters::IntegralFilter<float> integ(&a, 0.);

	Timer t;

	// for three seconds
	while (t.passed() < 1.0f) {
		usleep(rand()%1000);
		integ.get_data();
	}

	// validate that the integral is as expected
	ASSERT_NEAR(integ.get_data(), t.passed()*t.passed()/2., 0.001);
}
