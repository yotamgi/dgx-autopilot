#include "dgx1_platform.h"
#include "hw/adxl345_acc.h"
#include "hw/itg3200_gyro.h"
#include "hw/hmc5843_compass.h"
#include "hw/maestro_servo_controller.h"
#include "flapron_controller.h"
#include <stream/stream_connection.h>
#include <stream/stream_utils.h>

#include <boost/make_shared.hpp>

typedef stream::PushForwarder<lin_algebra::vec3f> vec3_push_forwarder;
typedef stream::PushForwarder<float> float_push_forwarder;

namespace autopilot {

NormalPlainPlatform create_dgx1_platform() {
	NormalPlainPlatform dgx1_platform;
	dgx1_platform.acc_sensor = boost::make_shared<Adxl345Acc>(2);
	dgx1_platform.gyro_sensor = boost::make_shared<Itg3200Gyro>(2);
	dgx1_platform.compass_sensor = boost::make_shared<Hmc5843Compass>(2);

	boost::shared_ptr<vec3_push_forwarder> pos_forwarder(new vec3_push_forwarder);
	boost::shared_ptr<float_push_forwarder> speed_dir_forwarder(new float_push_forwarder);
	boost::shared_ptr<float_push_forwarder> speed_mag_forwarder(new float_push_forwarder);

	static Gps gps_sensor;
	gps_sensor.set_pos_reciever_stream(pos_forwarder);
	gps_sensor.set_speed_dir_reciever_stream(speed_dir_forwarder);
	gps_sensor.set_speed_mag_reciever_stream(speed_mag_forwarder);

	dgx1_platform.gps_pos_generator = pos_forwarder;
	dgx1_platform.gps_speed_mag_generator = speed_mag_forwarder;
	dgx1_platform.gps_speed_dir_generator = speed_dir_forwarder;

	// fill the platform's servos
	static MaestroServoController maestro("/dev/ttyACM0");
	FlapronController flapron(maestro.getServoChannel(1), maestro.getServoChannel(4));
	dgx1_platform.pitch_servo	= maestro.getServoChannel(0);
	dgx1_platform.tilt_servo 	= flapron.get_ailron_servo();
	dgx1_platform.gas_servo 	= maestro.getServoChannel(2);
	dgx1_platform.yaw_servo 	= maestro.getServoChannel(3);

	// trim the servos
	maestro.getServoChannel(3)->trim_middle(52.5f);
	maestro.getServoChannel(3)->trim_range(95.0f);

	return dgx1_platform;
}

} //namespace autopilot
