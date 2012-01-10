#ifndef COCKPIT_H_
#define COCKPIT_H_

#include "interfaces/plain_cockpit.h"
#include "interfaces/plain_platform.h"
#include "gps_filter.h"
#include "fusion_filter.h"
#include <stream/util/lin_algebra.h>

#include <boost/shared_ptr.hpp>

namespace autopilot {

/**
 * Specific cockpit for normal plains.
 */
class Cockpit : public NormalPlainCockpit {
public:

	Cockpit(boost::shared_ptr<NormalPlainPlatform> platform);
	virtual ~Cockpit() {}


	/* NormalPlainCockpit Implementation */

	boost::shared_ptr<vec3_watch_stream> orientation();

	boost::shared_ptr<vec3_watch_stream> speed();

	boost::shared_ptr<vec2_watch_stream> position();

	boost::shared_ptr<float_watch_stream> alt();

	servo_stream_ptr tilt_servo();

	servo_stream_ptr yaw_servo();

	servo_stream_ptr pitch_servo();

	servo_stream_ptr gas_servo();


	/* Extending function */

	boost::shared_ptr<vec3_stream> 		watch_gyro_orientation();

	boost::shared_ptr<vec3_stream> 		watch_rest_orientation();

	boost::shared_ptr<float_stream> 	watch_rest_reliability();

private:
	boost::shared_ptr<NormalPlainPlatform> m_platform;

	boost::shared_ptr<vec3_watch_stream> m_orientation;
	boost::shared_ptr<vec3_stream> 		m_gyro_orientation;
	boost::shared_ptr<vec3_stream> 		m_rest_orientation;
	boost::shared_ptr<float_stream> 	m_rest_reliability;

	boost::shared_ptr<SimpleGpsFilter> m_gps_filter;

};

}  // namespace autopilot

#include "cockpit.inl"

#endif /* COCKPIT_H_ */
