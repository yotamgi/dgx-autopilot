#include "simulator_platform.h"
#include "simulator.h"
#include "sensors/simulator_gyro.h"
#include "sensors/simulator_accelerometer.h"
#include "sensors/simulator_magnetometer.h"
#include "sensors/simulator_gps.h"
#include "sensors/simulator_altmeter.h"
#include "sensors/simulator_pitot.h"
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <stream/stream_utils.h>
#include <stream/util/lin_algebra.h>

namespace simulator {

void thread(PlainParams plain_params, WindGen::Params wind_params, boost::shared_ptr<autopilot::NormalPlainPlatform> platform);

boost::shared_ptr<autopilot::NormalPlainPlatform> create_simulator_platform(
		PlainParams plain_params,
		WindGen::Params wind_params)
{
	boost::shared_ptr<autopilot::NormalPlainPlatform> platform =
			boost::make_shared<autopilot::NormalPlainPlatform>();

	new boost::thread(thread, plain_params, wind_params, platform);

	// wait until it is ready
	while (!platform->tilt_servo);

	// let the simulator start...
	// we should wait for the plain->data_ready bit.
	sleep(1);

	return platform;
}

void nada() {}


void thread(PlainParams plain_params, WindGen::Params wind_params, boost::shared_ptr<autopilot::NormalPlainPlatform> platform) {

	Simulator sim(plain_params, wind_params);

	boost::shared_ptr<Plain> plain = sim.get_plane();

	boost::shared_ptr<SimulatorGyroSensor> gyro_sensor(
			new SimulatorGyroSensor(plain_params.get_rot())
	);
	boost::shared_ptr<SimulatorAccelerometerSensor> acc_sensor(
			new SimulatorAccelerometerSensor(plain_params.get_rot())
	);
	boost::shared_ptr<SimulatorMagnetometerSensor> magneto_sensor(
			new SimulatorMagnetometerSensor(plain_params.get_rot())
	);
	boost::shared_ptr<SimulatorAltmeterSensor> alt_sensor(
			new SimulatorAltmeterSensor()
	);
	boost::shared_ptr<SimulatorGpsSensor> gps_sensor(
			new SimulatorGpsSensor()
	);

	boost::shared_ptr<SimulatorPitotSensor> pitot_sensor(
			new SimulatorPitotSensor(sim.get_wind())
	);

	plain->carry(gyro_sensor);
	plain->carry(acc_sensor);
	plain->carry(magneto_sensor);
	plain->carry(gps_sensor);
	plain->carry(alt_sensor);
	plain->carry(pitot_sensor);

	// inital servo data
	plain->get_elevator_servo()->set_data(50.);
	plain->get_ailron_servo()->set_data(50.);
	plain->get_rudder_servo()->set_data(50.);

	// fill the platform
	boost::shared_ptr<stream::PushForwarder<lin_algebra::vec3f> > gps_pos = boost::make_shared<stream::PushForwarder<lin_algebra::vec3f> >();
	boost::shared_ptr<stream::PushForwarder<float> > gps_speed_dir = boost::make_shared<stream::PushForwarder<float> >();
	boost::shared_ptr<stream::PushForwarder<float> > gps_speed_mag = boost::make_shared<stream::PushForwarder<float> >();

	platform->acc_sensor = acc_sensor;
	platform->gyro_sensor = gyro_sensor;
	platform->compass_sensor = magneto_sensor;
	platform->airspeed_sensor = pitot_sensor;
	platform->gps_pos_generator = gps_pos;
	platform->gps_speed_dir_generator = gps_speed_dir;
	platform->gps_speed_mag_generator = gps_speed_mag;
	platform->gas_servo = plain->get_throttle_servo();
	platform->yaw_servo = plain->get_rudder_servo();
	platform->tilt_servo = plain->get_elevator_servo();
	platform->pitch_servo = plain->get_ailron_servo();
	platform->alive = nada;
	gps_sensor->set_pos_listener(gps_pos);
	gps_sensor->set_speed_dir_listener(gps_speed_dir);
	gps_sensor->set_speed_mag_listener(gps_speed_mag);

	sim.run();
}

} // namespace simulator
