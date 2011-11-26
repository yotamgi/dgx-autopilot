#ifndef DGX1_PLATFORM_H_
#define DGX1_PLATFORM_H_

#include "interfaces/plain_platform.h"
#include <stdexcept>

namespace autopilot {

typedef stream::filters::WatchFilter<lin_algebra::vec3f> 	vec_watch_stream;

class DGX1Platform : public NormalPlainPlatform {
public:
	DGX1Platform();
	~DGX1Platform(){}

	typedef boost::shared_ptr<stream::DataPopStream<lin_algebra::vec3f> > vec_stream_ptr;

	boost::shared_ptr<vec_watch_stream> acc_sensor();

	boost::shared_ptr<vec_watch_stream> gyro_sensor();

	boost::shared_ptr<vec_watch_stream> compass_sensor();

	boost::shared_ptr<vec_watch_stream> gps_sensor();

	Servo* tilt_servo();

	Servo* yaw_servo();

	Servo* pitch_servo();

	Servo* gas_servo();

private:
	boost::shared_ptr<vec_watch_stream> m_acc;
	boost::shared_ptr<vec_watch_stream> m_gyro;
	boost::shared_ptr<vec_watch_stream> m_compass;
};

} //namespace autopilot

#endif /* DGX1_PLATFORM_H_ */
