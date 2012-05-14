#ifndef COCKPIT_H_
#define COCKPIT_H_

#include "interfaces/plain_cockpit.h"
#include "interfaces/plain_platform.h"
#include "fusion_filter.h"
#include <stream/util/lin_algebra.h>
#include <stream/stream_utils.h>

#include <boost/shared_ptr.hpp>

namespace autopilot {

typedef stream::filters::WatchFilter<lin_algebra::vec3f> vec3_watch_stream;

/**
 * Specific cockpit for normal plains.
 */
class Cockpit : public NormalPlainCockpit {
public:

	Cockpit(NormalPlainPlatform platform);
	virtual ~Cockpit();


	/* NormalPlainCockpit Implementation */

	boost::shared_ptr<vec3_watch_stream> orientation();

	boost::shared_ptr<float_watch_stream> ground_speed();

	boost::shared_ptr<vec2_watch_stream> position();

	boost::shared_ptr<float_watch_stream> alt();

	servo_stream_ptr tilt_servo();

	servo_stream_ptr yaw_servo();

	servo_stream_ptr pitch_servo();

	servo_stream_ptr gas_servo();

	void alive();

	/* Extending function */

	boost::shared_ptr<vec3_stream> 		watch_gyro_orientation();

	boost::shared_ptr<vec3_stream> 		watch_rest_orientation();

	boost::shared_ptr<float_stream> 	watch_rest_reliability();

	boost::shared_ptr<vec3_stream> 		watch_fixed_acc();


private:
	NormalPlainPlatform m_platform;

	boost::shared_ptr<vec3_watch_stream> 	m_orientation;
	boost::shared_ptr<vec3_stream> 			m_gyro_orientation;
	boost::shared_ptr<vec3_stream> 			m_rest_orientation;
	boost::shared_ptr<float_stream> 		m_rest_reliability;
	boost::shared_ptr<vec3_stream> 			m_fixed_acc;

	boost::shared_ptr<stream::PushToPopConv<lin_algebra::vec3f> > m_gps_pos;
	boost::shared_ptr<stream::PushToPopConv<float> > m_gps_speed_dir;
	boost::shared_ptr<stream::PushForwarder<float> > m_gps_speed_mag;

};

}  // namespace autopilot

#endif /* COCKPIT_H_ */
