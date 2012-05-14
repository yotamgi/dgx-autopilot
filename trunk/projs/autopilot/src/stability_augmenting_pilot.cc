#include "stability_augmenting_pilot.h"
#include <boost/make_shared.hpp>
namespace autopilot {

StabilityAugmentingPilot::StabilityAugmentingPilot(const StabilityAugmentingPilot::Params& params,
					boost::shared_ptr<NormalPlainCockpit> cockpit):
		m_cockpit(cockpit),
		m_tilt_strenth(boost::make_shared<stream::PushToPopConv<float> >(50.0f)),
		m_roll_strenth(boost::make_shared<stream::PushToPopConv<float> >(50.0f)),
		m_params(params),
		m_running(false)
{
}


void StabilityAugmentingPilot::update() {

	// get the oreintation
	lin_algebra::vec3f orientation = m_cockpit->orientation()->get_data();

	// maintain tilt
	float plain_tilt = orientation[0];
	float wanted_tilt = m_params.max_tilt_angle * (m_tilt_strenth->get_data()-50.0f)/50.0f;
	float tilt_delta =  wanted_tilt - plain_tilt;
	m_cockpit->tilt_servo()->set_data(50. - 50.*tilt_delta/m_params.max_tilt_angle);

	// maintain roll
	float plain_roll = orientation[2];
	float wanted_roll = m_params.max_pitch_angle * (m_roll_strenth->get_data()-50.0f)/50.0f;
	float roll_delta = wanted_roll - plain_roll;
	m_cockpit->pitch_servo()->set_data(50. - 50.*roll_delta/m_params.max_pitch_angle);

	m_cockpit->alive();
}

void StabilityAugmentingPilot::start(bool open_thread) {
	if (m_running) return;
	if (open_thread) {
		m_running_thread = boost::thread(&StabilityAugmentingPilot::start, this, false);
	} else {
		m_running = true;
		while (m_running) {
			update();
		}
	}
}

void StabilityAugmentingPilot::stop() {
	if (!m_running) return;
	m_running = false;
	m_running_thread.join();
}

boost::shared_ptr<stream::DataPushStream<float> > StabilityAugmentingPilot::get_tilt_control() {
	return m_tilt_strenth;
}
boost::shared_ptr<stream::DataPushStream<float> > StabilityAugmentingPilot::get_roll_control() {
	return m_roll_strenth;
}


} // namespace autopilot
