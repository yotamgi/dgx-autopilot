#ifndef FLAPRON_CONTROLLER_H_
#define FLAPRON_CONTROLLER_H_

#include "interfaces/servo_stream.h"
#include <stream/data_push_stream.h>
#include <stream/stream_utils.h>

namespace autopilot {

/**
 * The flapron controller class gets two servo streams: one for the left wing
 * ailron surface servo and one for the right one.
 * With this two streams it creates ailron and flaps stream.
 */
class FlapronController {
public:
	FlapronController(boost::shared_ptr<stream::DataPushStream<float> > servo_right,
					  boost::shared_ptr<stream::DataPushStream<float> > servo_left);

	boost::shared_ptr<ServoStream> get_flaps_servo() 	{  return m_flaps_stream;  	}
	boost::shared_ptr<ServoStream> get_ailron_servo()	{  return m_ailron_stream;	}

private:

	class State {
	public:
		State(boost::shared_ptr<stream::DataPushStream<float> > servo_right,
			  boost::shared_ptr<stream::DataPushStream<float> > servo_left);

		void set_flaps(float data);
		void set_ailron(float data);

	private:
		void update();

		boost::shared_ptr<stream::DataPushStream<float> > m_servo_right;
		boost::shared_ptr<stream::DataPushStream<float> > m_servo_left;

		float m_curr_flaps_state;
		float m_curr_ailron_state;
	};



	class FlapsStream : public ServoStream {
	public:
		FlapsStream(boost::shared_ptr<State> state):m_state(state) {}

		void set_data(const float& data) { m_state->set_flaps(trim(data)); }
	private:
		boost::shared_ptr<State> m_state;
	};

	class AilronStream : public ServoStream {
	public:
		AilronStream(boost::shared_ptr<State> state):m_state(state) {}

		void set_data(const float& data) { m_state->set_ailron(trim(data)); }
	private:
		boost::shared_ptr<State> m_state;
	};

	boost::shared_ptr<State> m_state;
	boost::shared_ptr<FlapsStream> m_flaps_stream;
	boost::shared_ptr<AilronStream> m_ailron_stream;
};

}  // namespace autopilot

#endif /* FLAPRON_CONTROLLER_H_ */
