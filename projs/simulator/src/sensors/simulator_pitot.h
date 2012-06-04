#ifndef SIMULATOR_PITOT_H_
#define SIMULATOR_PITOT_H_

#include "carriable.h"
#include "wind_gen.h"
#include <stream/util/lin_algebra.h>
#include <stream/data_pop_stream.h>
#include <boost/shared_ptr.hpp>

namespace simulator {


class SimulatorPitotSensor : public Carriable, public stream::DataPopStream<float> {
public:

	SimulatorPitotSensor(boost::shared_ptr<WindGen> simulator_wind);

	void setSensedObject(irr::scene::ISceneNode *object);

	void update(float time_delta);

	float get_data();

private:
	boost::shared_ptr<WindGen> m_simulator_wind;

	irr::scene::ISceneNode *m_traced_object;

	irr::core::vector3df m_old_pos;

	float m_pitot_data;

};


}  // namespace simulator


#endif /* SIMULATOR_PITOT_H_ */
