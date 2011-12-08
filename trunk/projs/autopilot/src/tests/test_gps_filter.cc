#include "gps_filter.h"
#include <gtest/gtest.h>

TEST(simple_gps_filter, functional) {
	SimpleGpsFilter gps(3);

	for (int i=0; i<100; i++) {
		lin_algebra::vec3f v;
		v[0] = 0.; v[1] = 0.; v[2] = 0.;
		gps.set_data(v);
		v[0] = 1.; v[1] = 1.; v[2] = 1.;
		gps.set_data(v);
		v[0] = 2.; v[1] = 2.; v[2] = 2.;
		gps.set_data(v);
		lin_algebra::vec3f pos = gps.get_position_stream()->get_data();
		ASSERT_NEAR(pos[0], 2., 0.01);
		ASSERT_NEAR(pos[1], 2., 0.01);
		ASSERT_NEAR(pos[2], 2., 0.01);

		lin_algebra::vec3f speed = gps.get_speed_stream()->get_data();
		ASSERT_NEAR(speed[0], 2., 0.01);
		ASSERT_NEAR(speed[1], 2., 0.01);
		ASSERT_NEAR(speed[2], 2., 0.01);

	}
}
