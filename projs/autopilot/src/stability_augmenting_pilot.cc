#include "stability_augmenting_pilot.h"
#include <stream/util/time.h>
#include <boost/make_shared.hpp>

namespace autopilot {

StabilityAugmentingPilot::StabilityAugmentingPilot(boost::shared_ptr<NormalPlainCockpit> cockpit,
					float pitch_strength,
					float roll_strength):
		m_cockpit(cockpit),
		m_tilt_angle(boost::make_shared<stream::PushToPopConv<float> >(0.0f)),
		m_roll_angle(boost::make_shared<stream::PushToPopConv<float> >(0.0f)),
		m_pitch_strength(pitch_strength),
		m_roll_strength(roll_strength),
		m_running(false)
{
}


void StabilityAugmentingPilot::update() {

	// get the oreintation
	lin_algebra::vec3f orientation = m_cockpit->orientation()->get_data();

	// maintain tilt
	float plain_tilt = orientation[0];
	float wanted_tilt = m_tilt_angle->get_data();
	float tilt_delta =  wanted_tilt - plain_tilt;
	m_cockpit->tilt_servo()->set_data(50. - 50.*tilt_delta/m_pitch_strength);

	// maintain roll
	float plain_roll = orientation[2];
	float wanted_roll = m_roll_angle->get_data();
	float roll_delta = wanted_roll - plain_roll;
	m_cockpit->pitch_servo()->set_data(50. - 50.*roll_delta/m_roll_strength);

	m_cockpit->alive();
}

void StabilityAugmentingPilot::start(bool open_thread) {
	if (m_running) return;
	if (open_thread) {
		m_running_thread = boost::thread(&StabilityAugmentingPilot::start, this, false);
	} else {
		m_running = true;

		Timer t;
		while (m_running) {

			t.reset();

			update();

			// maintain constant and not too high FPS
			float dt = 1./UPDATE_RATE - t.passed();
			while (dt > 0) {
				usleep(1000000*dt);
				dt = 1./UPDATE_RATE - t.passed();
			}
		}
	}
}

void StabilityAugmentingPilot::stop() {
	if (!m_running) return;
	m_running = false;
	m_running_thread.join();
}

boost::shared_ptr<stream::DataPushStream<float> > StabilityAugmentingPilot::get_tilt_control() {
	return m_tilt_angle;
}
boost::shared_ptr<stream::DataPushStream<float> > StabilityAugmentingPilot::get_roll_control() {
	return m_roll_angle;
}


} // namespace autopilot
