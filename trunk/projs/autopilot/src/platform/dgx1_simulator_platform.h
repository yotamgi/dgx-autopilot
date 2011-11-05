#ifndef DGX1_SIMULATOR_PLATFORM_H_
#define DGX1_SIMULATOR_PLATFORM_H_

#include "interfaces/plain_platform.h"
#include <stream/stream_importer.h>
#include <boost/shared_ptr.hpp>

namespace autopilot {

typedef stream::DataGenerator<lin_algebra::vec3f> vec_stream;
typedef boost::shared_ptr<vec_stream> 				 vec_stream_ptr;

class DGX1SimulatorPlatform : public NormalPlainPlatform {
public:
	DGX1SimulatorPlatform(std::string address);

	vec_stream_ptr acc_sensor();

	vec_stream_ptr gyro_sensor();

	vec_stream_ptr compass_sensor();

	vec_stream_ptr gps_sensor();

	Servo* tilt_servo();

	Servo* yaw_servo();

	Servo* pitch_servo();

	Servo* gas_servo();

private:
	vec_stream_ptr m_acc;
	vec_stream_ptr m_gyro;
	vec_stream_ptr m_compass;

	stream::StreamImporter m_importer;

};

} // namespace autopilot

#endif /* DGX1_SIMULATOR_PLATFORM_H_ */
