#ifndef COCKPIT_H_
#define COCKPIT_H_

#include "interfaces/plain_cockpit.h"
#include "interfaces/plain_platform.h"
#include "fusion_filter.h"
#include <stream/util/lin_algebra.h>
#include <stream/stream_utils.h>

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

namespace autopilot {



/**
 * Specific cockpit for normal plains.
 */
class Cockpit : public NormalPlainCockpit {
public:

	struct calibration_data {
		lin_algebra::vec3f orientation;
		float alt;
	};

	Cockpit(NormalPlainPlatform platform);
	virtual ~Cockpit();

	/**
	 * Calibrate function.
	 * This one reads the data from the sensors, understand the calibration
	 * data and calibrate the cocktpit.
	 * @return the calibration data that the plane was calibrated by.
	 */
	calibration_data calibrate();

	/**
	 * Calibrate function
	 * This function gets calibration data and by that calibrated the plane.
	 * @param calibration - the calibration data to calibrate the plane by.
	 */
	void calibrate(const calibration_data& calibration);

	/* NormalPlainCockpit Implementation */

	boost::shared_ptr<vec3_stream> orientation();

	boost::shared_ptr<float_stream> ground_speed();

	boost::shared_ptr<float_stream> air_speed();

	boost::shared_ptr<vec2_stream> position();

	boost::shared_ptr<float_stream> alt();

	boost::shared_ptr<float_stream> battery_state();

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

	typedef stream::filters::WatchFilter<lin_algebra::vec3f> vec3_watch_stream;

	static const float UPDATE_RATE = 140.;

	void run();

	template <typename T>
	class CalibrationFilter : public stream::StreamPopFilter<T> {
	public:
		CalibrationFilter(boost::shared_ptr<stream::DataPopStream<T> > filtered_stream):
			stream::StreamPopFilter<T>(filtered_stream) {}

		T get_data()
		{ return stream::StreamPopFilter<T>::m_generator->get_data() + m_calibration_constant; }

		void calibrate(T calibration_constatnt)
		{ m_calibration_constant = calibration_constatnt; }

	private:
		T m_calibration_constant;
	};

	NormalPlainPlatform m_platform;

	boost::shared_ptr<vec3_watch_stream>			m_orientation;
	boost::shared_ptr<vec3_watch_stream> 			m_gyro_orientation;
	boost::shared_ptr<vec3_stream> 					m_rest_orientation;
	boost::shared_ptr<float_stream> 				m_rest_reliability;
	boost::shared_ptr<vec3_stream> 					m_fixed_acc;
	boost::shared_ptr<float_watch_stream> 			m_alt_stream;
	boost::shared_ptr<float_watch_stream> 			m_airspeed_stream;
	boost::shared_ptr<float_watch_stream> 			m_battery_stream;

	boost::shared_ptr<FusionFilter> 				m_orientation_filter;
	boost::shared_ptr<CalibrationFilter<float> > 	m_alt_calibration;

	boost::shared_ptr<stream::PushToPopConv<lin_algebra::vec3f> > m_gps_pos;
	boost::shared_ptr<stream::PushToPopConv<float> > m_gps_speed_dir;
	boost::shared_ptr<stream::PushForwarder<float> > m_gps_speed_mag;

	boost::thread m_running_thread;

};

}  // namespace autopilot

#endif /* COCKPIT_H_ */
