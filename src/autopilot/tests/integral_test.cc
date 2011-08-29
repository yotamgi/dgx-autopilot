
#include <iostream>
#include <gtest/gtest.h>
#include <sys/time.h>
#include <cstdlib>
#include "common/types.h"
#include "components/generators.h"
#include "components/data_filters.h"
#include "util/time.h"

/**
 * Dummy generator for testing the integral.
 * Returns vector with those values:
 * 	- x: constant 1.0
 *  - y: time since creation
 *  - z: constant 1.0
 */
class DummyGen : public DataGenerator<vector_t> {
public:
	DummyGen() {
		gettimeofday(&m_start_time, NULL);
	}

	virtual boost::shared_ptr<vector_t> get_data() {
		timeval curr;
		gettimeofday(&curr, NULL);
		boost::shared_ptr<vector_t> ans(new vector_t(
				1.f,
				(curr.tv_usec - m_start_time.tv_usec)/1000000. + curr.tv_sec - m_start_time.tv_sec,
				1.f
		));
		return ans;
	}

	bool is_data_losed() {return false;}

private:
	timeval m_start_time;
};



TEST(IntegralTest, stress_test) {

	DummyGen a;
	IntergralFilter integ(&a);

	Timer t;

	// for three seconds
	while (t.passed() < 1.0f) {
		integ.get_data();
	}

	// validate that the integral is as expected
	ASSERT_NEAR(integ.get_data()->x, t.passed(), 0.0001);
	ASSERT_NEAR(integ.get_data()->y, t.passed()*t.passed()/2., 0.001);
	ASSERT_NEAR(integ.get_data()->z, t.passed(), 0.001);
}



TEST(IntegralTest, random_test) {

	DummyGen a;
	IntergralFilter integ(&a);

	Timer t;

	// for three seconds
	while (t.passed() < 1.0f) {
		usleep(rand()%1000);
		integ.get_data();
	}

	// validate that the integral is as expected
	ASSERT_NEAR(integ.get_data()->x, t.passed(), 0.001);
	ASSERT_NEAR(integ.get_data()->y, t.passed()*t.passed()/2., 0.001);
	ASSERT_NEAR(integ.get_data()->z, t.passed(), 0.001);
}
