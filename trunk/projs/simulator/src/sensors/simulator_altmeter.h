#ifndef SIMULATOR_ALTMETER_H_
#define SIMULATOR_ALTMETER_H_

#include "carriable.h"
#include <stream/data_pop_stream.h>

namespace simulator {


class SimulatorAltmeterSensor : public Carriable, public stream::DataPopStream<float> {
public:

	SimulatorAltmeterSensor();

	void setSensedObject(irr::scene::ISceneNode *object);

	void update(float time_delta);

	float get_data();

private:

	irr::scene::ISceneNode *m_traced_object;

	float m_alt_data;
};


}  // namespace simulator



#endif /* SIMULATOR_ALTMETER_H_ */
