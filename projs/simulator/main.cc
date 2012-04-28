/** Example 004 Movement

This Tutorial shows how to move and animate SceneNodes. The
basic concept of SceneNodeAnimators is shown as well as manual
movement of nodes using the keyboard.  We'll demonstrate framerate
independent movement, which means moving by an amount dependent
on the duration of the last run of the Irrlicht loop.

Example 19.MouseAndJoystick shows how to handle those kinds of input.

As always, I include the header files, use the irr namespace,
and tell the linker to link with the .lib file.
*/
#ifdef _MSC_VER
// We'll also define this to stop MSVC complaining about sprintf().
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "Irrlicht.lib")
#endif

#include "plain.h"
#include "simulator.h"
#include "sensors/simulator_gyro.h"
#include "sensors/simulator_accelerometer.h"
#include "sensors/simulator_magnetometer.h"
#include "sensors/simulator_gps.h"
#include "sensors/simulator_altmeter.h"
#include <cmath>
#include <boost/make_shared.hpp>
#include <sys/time.h>

#include <stream/stream_connection.h>
#include <stream/util/tcpip_connection.h>
#include <stream/filters/integral_filter.h>
#include <stream/filters/gyro_to_av_matrix.h>
#include <stream/filters/acc_compass_rot.h>
#include <stream/filters/matrix_to_euler_filter.h>

#include <boost/thread.hpp>


typedef stream::DataPopStream<lin_algebra::vec3f> vec3stream;
typedef stream::DataPopStream<float> floatstream;
typedef boost::shared_ptr<vec3stream> vec3stream_ptr;
typedef boost::shared_ptr<floatstream> floatstream_ptr;

void export_import(boost::shared_ptr<simulator::Plain> plain,
		vec3stream_ptr gyro,
		vec3stream_ptr acc,
		vec3stream_ptr magneto,
		floatstream_ptr alt,
		boost::shared_ptr<simulator::SimulatorGpsSensor> gps_sensor)
{

	while (true) {
		while (!plain->data_ready());
		try {
			boost::shared_ptr<stream::TcpipClient> client =
					boost::make_shared<stream::TcpipClient>("localhost", 0x6060);
			stream::StreamConnection conn(client);
			conn.export_pop_stream<lin_algebra::vec3f>(gyro, "simulator_gyro");
			conn.export_pop_stream<lin_algebra::vec3f>(acc, "simulator_acc");
			conn.export_pop_stream<lin_algebra::vec3f>(magneto, "simulator_compass");
			conn.export_pop_stream<float>(alt, "simulator_alt");

			conn.export_push_stream<float>(plain->get_elevator_servo(), "simulator_pitch_servo");
			conn.export_push_stream<float>(plain->get_rudder_servo(), "simulator_yaw_servo");
			conn.export_push_stream<float>(plain->get_ailron_servo(), "simulator_tilt_servo");
			conn.export_push_stream<float>(plain->get_throttle_servo(), "simulator_gas_servo");
			conn.run(true);

			gps_sensor->set_pos_listener(conn.import_push_stream<lin_algebra::vec3f>("gps_pos_reciever"));
			gps_sensor->set_speed_dir_listener(conn.import_push_stream<float>("gps_speed_dir_reciever"));
			gps_sensor->set_speed_mag_listener(conn.import_push_stream<float>("gps_speed_mag_reciever"));

			// now wait
			while (true) usleep(10000000);
		} catch (stream::ConnectionExceptioin& e) {
			std::cout << "Connection died" << std::endl;
		}
	}
}

/*
The event receiver for keeping the pressed keys is ready, the actual responses
will be made inside the render loop, right before drawing the scene. So lets
just create an irr::IrrlichtDevice and the scene node we want to move. We also
create some other additional scene nodes, to show that there are also some
different possibilities to move and animate scene nodes.
*/
int main()
{

	simulator::PlainParams plane_params(
			 "media/pf-cessna-182.x",
			 "media/pf-cessna-182.bmp",
			 irr::core::vector3df(1., 1., 1.),
			 irr::core::vector3df(0., 180., 0.),
			 500.0f,
			 500.0f,
			 500.0f,
			 6.5f,  	// mass
			 40.0f,  	// engine power
			 0.15f, 	// drag
			 1.4f, 		// wing area
			 -3.,  		// lift
 			 30.,		// sideslide to yaw effect strenth
			 20.); 		// dihedral effect strenth

//	simulator::PlainParams plane_params(
//			 "media/pf-cessna-182.x",
//			 "media/pf-cessna-182.bmp",
//			 irr::core::vector3df(4.0f, 4.0f, 4.0f),
//			 200.0f,
//			 60.0f,
//			 100.0f,
//			 1000.0f,  // mass
//			 3000.0f,  // engine power
//			 0.8f, // drag
//			 13.0f, // wing area
//			 -5.); // lift

	simulator::Simulator sim(plane_params);

	boost::shared_ptr<simulator::Plain> plain = sim.get_plane();

	boost::shared_ptr<simulator::SimulatorGyroSensor> gyro_sensor(
			new simulator::SimulatorGyroSensor(plane_params.get_rot())
	);
	boost::shared_ptr<simulator::SimulatorAccelerometerSensor> acc_sensor(
			new simulator::SimulatorAccelerometerSensor(plane_params.get_rot())
	);
	boost::shared_ptr<simulator::SimulatorMagnetometerSensor> magneto_sensor(
			new simulator::SimulatorMagnetometerSensor(plane_params.get_rot())
	);
	boost::shared_ptr<simulator::SimulatorAltmeterSensor> alt_sensor(
			new simulator::SimulatorAltmeterSensor()
	);
	boost::shared_ptr<simulator::SimulatorGpsSensor> gps_sensor(
			new simulator::SimulatorGpsSensor()
	);

	plain->carry(gyro_sensor);
	plain->carry(acc_sensor);
	plain->carry(magneto_sensor);
	plain->carry(gps_sensor);
	plain->carry(alt_sensor);

	// inital servo data
	plain->get_elevator_servo()->set_data(50.);
	plain->get_ailron_servo()->set_data(50.);
	plain->get_rudder_servo()->set_data(50.);

	// export all the sensors
	boost::thread t(export_import, plain, gyro_sensor, acc_sensor, magneto_sensor, alt_sensor, gps_sensor);

	sim.run();
	t.detach();

	return 0;
}

