#include "cockpit.h"
#include <stream/util/lin_algebra.h>
#include <stream/filters/integral_filter.h>
#include <stream/filters/matrix_to_euler_filter.h>
#include <stream/filters/acc_compass_rot.h>
#include <stream/filters/gyro_to_av_matrix.h>
#include <stream/filters/low_pass_filter.h>
#include <stream/func_filter.h>
#include <boost/make_shared.hpp>
#include <boost/bind/bind.hpp>

namespace autopilot {

static lin_algebra::mat3f update_matrix(const lin_algebra::mat3f& m1,
		const lin_algebra::mat3f& m2)
{

	lin_algebra::mat3f rot = m1 + m1 * m2;

	// maintain the matrix orthogonal
	rot.col(2) = lin_algebra::cross_product<lin_algebra::vec3f>(rot.col(0), rot.col(1));
	rot.col(0) = lin_algebra::cross_product<lin_algebra::vec3f>(rot.col(1), rot.col(2));

	// maintain the mtrix normal
	rot.col(0) /= lin_algebra::vec_len(rot.col(0));
	rot.col(1) /= lin_algebra::vec_len(rot.col(1));
	rot.col(2) /= lin_algebra::vec_len(rot.col(2));

	return rot;
}

Cockpit::Cockpit(NormalPlainPlatform platform):
		m_platform(platform),
		m_gps_pos(boost::make_shared<stream::PushToPopConv<lin_algebra::vec3f> >(lin_algebra::vec3f())),
		m_gps_speed_dir(boost::make_shared<stream::PushToPopConv<float> >(0.)),
		m_gps_speed_mag(boost::make_shared<stream::PushForwarder<float> >())
{
	// This is the stream schematics:
	//
	//				  +----GyroToAv----IntegralFilter----MatrixToEulerFilter---- m_gyro_orientation
	//         (watch)|
	// gyro ----------+--------------------GyroToAv-------+
	//			    				   				      |
	// acc --------+       orientation+----WatchFilter----+ KalmanFilter----MatrixToEulerFilter----WatchFilter----m_orientation
	//			   | AccCompassRot    |		  +			  |
	// compass ----+	   reliability+----WatchFilter----+
	//                                        |    +
	//                                        |    +------------ m_rest_orientation
	//								          +----------------- m_rest_reliability
	//

	namespace filter = stream::filters;

	// add watch to the gyro
	boost::shared_ptr<vec3_watch_stream> gyro_watch(new vec3_watch_stream(m_platform.gyro_sensor));
	m_platform.gyro_sensor = gyro_watch;

	m_platform.gps_pos_generator->register_receiver(m_gps_pos);
	m_platform.gps_speed_dir_generator->register_receiver(m_gps_speed_dir);
	m_platform.gps_speed_mag_generator->register_receiver(m_gps_speed_mag);

	m_gyro_orientation = boost::make_shared<vec3_watch_stream>(
		boost::make_shared<filter::MatrixToEulerFilter>(
			boost::make_shared<filter::IntegralFilter<lin_algebra::mat3f> >(
				boost::make_shared<filter::GyroToAVMatrix>(
					gyro_watch->get_watch_stream()
				),
				lin_algebra::identity_matrix<lin_algebra::mat3f>(3u, 3u),
				update_matrix
			)
		)
	);

	boost::shared_ptr<FusionFilter> fusion_filter = boost::make_shared<FusionFilter>(
					m_platform.acc_sensor,
					m_platform.compass_sensor,
					m_platform.gyro_sensor,
					20., // the north explected angle
					m_gps_speed_mag
	);

	m_orientation_calibration =
		boost::make_shared<CalibrationFilter<lin_algebra::vec3f> >(
			fusion_filter
		);
	m_orientation = boost::make_shared<vec3_watch_stream>(m_orientation_calibration);

	m_rest_reliability = fusion_filter->get_reliability_stream();
	m_rest_orientation = fusion_filter->get_rest_orientation_stream();
	m_fixed_acc = fusion_filter->get_fixed_acc_stream();

	m_alt_calibration =
		boost::make_shared<CalibrationFilter<float> >(
			stream::create_func_pop_filter<lin_algebra::vec3f,float>(
				m_gps_pos,
				lin_algebra::get<2>
			)
		);
	m_alt_stream = boost::make_shared<float_watch_stream>(m_alt_calibration);

	m_alt_calibration->calibrate(0);
	m_orientation_calibration->calibrate(lin_algebra::create_vec3f(0., 0., 0.));

	m_airspeed_stream = boost::make_shared<float_watch_stream>(platform.airspeed_sensor);
	m_battery_stream = boost::make_shared<float_watch_stream>(platform.battery_sensor);

	m_running_thread = boost::thread(&Cockpit::run, this);
}

Cockpit::~Cockpit() {}

Cockpit::calibration_data Cockpit::calibrate() {
	Cockpit::calibration_data cal_data;
	cal_data.orientation 	= -m_orientation->get_data();
	cal_data.alt 			= -m_alt_stream->get_data();
	calibrate(cal_data);
	return cal_data;
}

void Cockpit::calibrate(const Cockpit::calibration_data& calibration) {
	m_orientation_calibration->calibrate(calibration.orientation);
	m_alt_calibration->calibrate(calibration.alt);
}

void Cockpit::run() {

	size_t i=0;
	Timer t;
	while (true) {

		// reset the timer
		t.reset();

		// update the data
		m_orientation->get_data();
		m_gyro_orientation->get_data();
		m_alt_stream->get_data();

		// some things should happen less than others
		if (i%5 == 0) {
			m_airspeed_stream->get_data();
		}
		if (i%39 == 0) {
			m_battery_stream->get_data();
		}
		i++;

		// maintain constant and not too high FPS
		float dt = 1./UPDATE_RATE - t.passed();
		while (dt > 0) {
			usleep(1000000*dt);
			dt = 1./UPDATE_RATE - t.passed();
		}
	}
}


boost::shared_ptr<vec3_stream> Cockpit::watch_gyro_orientation() {
	return m_gyro_orientation->get_watch_stream();
}
boost::shared_ptr<vec3_stream> Cockpit::watch_rest_orientation() {
	return m_rest_orientation;
}

boost::shared_ptr<vec3_stream> Cockpit::orientation() {
	return m_orientation->get_watch_stream();
}

boost::shared_ptr<float_stream> Cockpit::watch_rest_reliability() {
	return m_rest_reliability;
}

boost::shared_ptr<vec3_stream> 	Cockpit::watch_fixed_acc() {
	return m_fixed_acc;
}

boost::shared_ptr<float_stream> Cockpit::ground_speed() {
	return boost::shared_ptr<float_watch_stream>();
//	return boost::make_shared<float_watch_stream>(
//			(boost::shared_ptr<stream::DataPopStream<float> >)m_gps_speed_mag
//	);
}

boost::shared_ptr<float_stream> Cockpit::air_speed() {
	return m_airspeed_stream->get_watch_stream();
}

boost::shared_ptr<float_stream> Cockpit::battery_state() {
	return m_battery_stream->get_watch_stream();
}

boost::shared_ptr<vec2_stream> Cockpit::position() {
	return boost::make_shared<vec2_watch_stream>(
			stream::create_func_pop_filter<lin_algebra::vec3f,lin_algebra::vec2f> (
					m_gps_pos,
					lin_algebra::get<0, 1>
			)
	);
}

boost::shared_ptr<float_stream> Cockpit::alt() {
	return m_alt_stream->get_watch_stream();
}

void Cockpit::alive() {
	m_platform.alive();
}

servo_stream_ptr Cockpit::tilt_servo() {
	return m_platform.tilt_servo;
}

servo_stream_ptr Cockpit::yaw_servo() {
	return m_platform.yaw_servo;
}

servo_stream_ptr Cockpit::pitch_servo(){
	return m_platform.pitch_servo;
}

servo_stream_ptr Cockpit::gas_servo() {
	return m_platform.gas_servo;
}


} // namespace autopilot
