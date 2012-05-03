#include "stability_augmenting_pilot.h"
#include <boost/make_shared.hpp>
namespace autopilot {

StabilityAugmentingPilot::StabilityAugmentingPilot(const StabilityAugmentingPilot::Params& params,
					boost::shared_ptr<NormalPlainCockpit> cockpit):
		m_cockpit(cockpit),
		m_pitch_strenth(boost::make_shared<stream::PushToPopConv<float> >(50.0f)),
		m_tilt_strenth(boost::make_shared<stream::PushToPopConv<float> >(50.0f)),
		m_params(params),
		m_running(false)
{
}


void StabilityAugmentingPilot::update() {

	// get the oreintation
	lin_algebra::vec3f orientation = m_cockpit->orientation()->get_data();

	// maintain pitch
	float plain_pitch = orientation[0];
	float wanted_pitch = m_params.max_pitch_angle * (m_pitch_strenth->get_data()-50.0f)/50.0f;
	float pitch_delta =  wanted_pitch - plain_pitch;
	m_cockpit->pitch_servo()->set_data(50. - 50.*(pitch_delta)/m_params.max_pitch_angle);

	// maintain tillt
	float plain_tilt = orientation[2];
	float wanted_tilt = m_params.max_tilt_angle * (m_tilt_strenth->get_data()-50.0f)/50.0f;
	float tilt_delta = wanted_tilt - plain_tilt;
	m_cockpit->tilt_servo()->set_data(50. - 50.*tilt_delta/m_params.max_tilt_angle);
}

void StabilityAugmentingPilot::run(bool open_thread) {
	if (open_thread) {
		m_running_thread = boost::thread(&StabilityAugmentingPilot::run, this, false);
	} else {
		m_running = true;
		while (m_running) {
			update();
		}
	}
}

void StabilityAugmentingPilot::stop() {
	if (!m_running) {
		throw std::runtime_error("StabilityAugmentingPilot::stop function called but it was not ran");
	}
	m_running = false;
	m_running_thread.join();
}

boost::shared_ptr<stream::DataPushStream<float> > StabilityAugmentingPilot::get_pitch_control() {
	return m_pitch_strenth;
}
boost::shared_ptr<stream::DataPushStream<float> > StabilityAugmentingPilot::get_tilt_control() {
	return m_tilt_strenth;
}


} // namespace autopilot
