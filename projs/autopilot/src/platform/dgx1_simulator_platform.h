#ifndef DGX1_SIMULATOR_PLATFORM_H_
#define DGX1_SIMULATOR_PLATFORM_H_

#include "interfaces/plain_platform.h"
#include <stream/stream_importer.h>
#include <boost/shared_ptr.hpp>

namespace autopilot {

typedef stream::DataGenerator<lin_algebra::vec3f>			vec_stream;
typedef boost::shared_ptr<vec_stream> 						vec_stream_ptr;
typedef stream::filters::WatchFilter<lin_algebra::vec3f> 	vec_watch_stream;


class DGX1SimulatorPlatform : public NormalPlainPlatform {
public:
	DGX1SimulatorPlatform(std::string address);

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

	stream::StreamImporter m_importer;

};

} // namespace autopilot

#endif /* DGX1_SIMULATOR_PLATFORM_H_ */
