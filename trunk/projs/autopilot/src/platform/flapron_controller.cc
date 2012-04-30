#include "flapron_controller.h"
#include <boost/make_shared.hpp>

namespace autopilot {


FlapronController::FlapronController(boost::shared_ptr<stream::DataPushStream<float> > servo_right,
		  boost::shared_ptr<stream::DataPushStream<float> > servo_left):
		  m_state(boost::make_shared<FlapronController::State>(servo_right, servo_left)),
		  m_flaps_stream(boost::make_shared<FlapronController::FlapsStream>(m_state)),
		  m_ailron_stream(boost::make_shared<FlapronController::AilronStream>(m_state))
{}

FlapronController::State::State(boost::shared_ptr<stream::DataPushStream<float> > servo_right,
		  boost::shared_ptr<stream::DataPushStream<float> > servo_left):
	m_servo_right(servo_right),
	m_servo_left(servo_left),
	m_curr_flaps_state(0.0f),
	m_curr_ailron_state(50.0f)
{}

void FlapronController::State::set_ailron(float data) {
	m_curr_ailron_state = data;
	update();
}

void FlapronController::State::set_flaps(float data) {
	m_curr_flaps_state = data;
	update();
}

void FlapronController::State::update() {
	m_servo_right->set_data(m_curr_ailron_state + m_curr_flaps_state);
	m_servo_left->set_data (m_curr_ailron_state - m_curr_flaps_state);
}

}  // namespace autopilot
