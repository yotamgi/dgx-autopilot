#ifndef GPS_SENSOR_H_
#define GPS_SENSOR_H_

#include "carriable.h"
#include <stream/util/lin_algebra.h>
#include <stream/data_push_stream.h>
#include <boost/shared_ptr.hpp>
#include <boost/circular_buffer.hpp>

namespace simulator {


class SimulatorGpsSensor : public Carriable {
public:
	SimulatorGpsSensor(lin_algebra::vec3f start_pos = lin_algebra::create_vec3f(0., 0., 0.));

	void setSensedObject(irr::scene::ISceneNode *object);

	void update(float time_delta);

	void set_pos_listener(boost::shared_ptr<stream::DataPushStream<lin_algebra::vec3f> > listenr);
	void set_speed_dir_listener(boost::shared_ptr<stream::DataPushStream<float> > listenr);
	void set_speed_mag_listener(boost::shared_ptr<stream::DataPushStream<float> > listenr);

private:

	irr::core::vector3df m_sensor_orientation;
	irr::scene::ISceneNode *m_traced_object;

	float m_update_timer;
	const float UPDATE_TIME_INTERVAL;

	// members for calculating the gps speed
	const size_t AVARGE_LEN;
	boost::circular_buffer<lin_algebra::vec3f> m_past_samples;

	boost::shared_ptr<stream::DataPushStream<lin_algebra::vec3f> > m_gps_pos_listener;
	boost::shared_ptr<stream::DataPushStream<float> > m_gps_speed_dir_listener;
	boost::shared_ptr<stream::DataPushStream<float> > m_gps_speed_mag_listener;

	lin_algebra::vec3f m_start_pos;
};


}  // namespace simulator



#endif /* GPS_SENSOR_H_ */
