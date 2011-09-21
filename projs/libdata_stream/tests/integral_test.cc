
#include <iostream>
#include <gtest/gtest.h>
#include <sys/time.h>
#include <cstdlib>
#include "generators.h"
#include "filters/integral_filter.h"
#include "util/time.h"

/**
 * Dummy generator for testing the integral.
 * Returns vector with those values:
 * 	- x: constant 1.0
 *  - y: time since creation
 *  - z: constant 1.0
 */
class DummyGen : public stream::VecGenerator<float,3> {
public:
	DummyGen() {
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

	bool is_data_losed() {return false;}

private:
	timeval m_start_time;
};



TEST(IntegralTest, stress_test) {

	DummyGen a;
	stream::filters::IntergralFilter<float,3> integ(&a);

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



TEST(IntegralTest, random_test) {

	DummyGen a;
	stream::filters::IntergralFilter<float,3> integ(&a);

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
