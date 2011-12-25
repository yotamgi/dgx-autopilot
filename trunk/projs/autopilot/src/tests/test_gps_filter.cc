#include "gps_filter.h"
#include <gtest/gtest.h>
#include <cstdlib>

TEST(simple_gps_filter, speed) {
	SimpleGpsFilter gps(100);
	lin_algebra::vec3f dir;
	dir.randu();
	dir = lin_algebra::normalize(dir);

	for (int i=0; i<100; i++) {
		lin_algebra::vec3f noise;
		noise.randu();
		noise = lin_algebra::normalize(noise);
		gps.set_data(dir * float(i) + noise);
	}
	lin_algebra::vec3f speed = gps.get_speed_stream()->get_data();

	ASSERT_NEAR(lin_algebra::vec_len(speed), 100., 1.01);

	speed = lin_algebra::normalize(speed);
	ASSERT_NEAR(speed[0], dir[0], 0.01);
	ASSERT_NEAR(speed[1], dir[1], 0.01);
	ASSERT_NEAR(speed[2], dir[2], 0.01);
}

TEST(simple_gps_filter, position) {
	SimpleGpsFilter gps(100);
	lin_algebra::vec3f pos;
	pos.randu();

	gps.set_data(pos);

	lin_algebra::vec2f gps_pos = gps.get_position_stream()->get_data();
	float gps_alt = gps.get_alt_stream()->get_data();
	ASSERT_NEAR(gps_pos[0], pos[0], 0.001);
	ASSERT_NEAR(gps_pos[1], pos[2], 0.001);
	ASSERT_NEAR(gps_alt, pos[1], 0.001);
}
