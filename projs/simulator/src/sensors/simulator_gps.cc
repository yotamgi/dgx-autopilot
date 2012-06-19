#include "simulator_gps.h"

namespace simulator {

typedef irr::core::vector3df irrvec3f;

const float METERS_TO_GEO = 111319.4889f;

SimulatorGpsSensor::SimulatorGpsSensor(lin_algebra::vec3f start_pos):
		m_traced_object(NULL),
		m_update_timer(0.),
		UPDATE_TIME_INTERVAL(1.),
		AVARGE_LEN(3),
		m_past_samples(AVARGE_LEN),
		m_start_pos(start_pos)
{}

void SimulatorGpsSensor::setSensedObject(irr::scene::ISceneNode *object) {
	m_traced_object = object;
}

void SimulatorGpsSensor::update(float time_delta) {
	if (!m_traced_object) throw std::runtime_error("No sensed object in the sensor");

	m_update_timer += time_delta;

	if (m_update_timer > UPDATE_TIME_INTERVAL) {
		m_update_timer = 0.;

		// calculate the position
		irrvec3f irrpos = m_traced_object->getPosition();
		lin_algebra::vec3f gps_pos = lin_algebra::create_vec3f(irrpos.X, irrpos.Y, irrpos.Z);

		lin_algebra::vec3f rand;
		rand.randu();
		rand = lin_algebra::normalize(rand);
		gps_pos += rand;

		// calculate the speed
		m_past_samples.push_back(gps_pos);

		// fill the data into a matrix
		lin_algebra::Mat<float> m(2, m_past_samples.size());
		for (size_t i=0; i<m_past_samples.size(); i++) {
			m.col(i) = lin_algebra::get<2, 0>(m_past_samples.at(i));
		}
		m = lin_algebra::trans(m);

		lin_algebra::Mat<float> ce, score;
		lin_algebra::princomp(ce, score, m);
		lin_algebra::vec2f speed = ce.col(0) * score.at(0,0) * -2.;
		//float reliability = score.at(0, 1);
		float speed_mag = std::sqrt(speed[0]*speed[0] + speed[1]*speed[1]);
		float speed_angle = std::atan(speed[1]/speed[0])/lin_algebra::PI * 180.;

		if (m_gps_pos_listener) {
			lin_algebra::vec3f geo_gps_pos = lin_algebra::create_vec3f(
					gps_pos[0]/METERS_TO_GEO, gps_pos[2]/METERS_TO_GEO, gps_pos[1]
			);
			m_gps_pos_listener->set_data(geo_gps_pos + m_start_pos);
		}
		if (m_gps_speed_dir_listener) {
			m_gps_speed_dir_listener->set_data(speed_angle);
		}
		if (m_gps_speed_mag_listener) {
			m_gps_speed_mag_listener->set_data(speed_mag);
		}
	}
}

void SimulatorGpsSensor::set_pos_listener(boost::shared_ptr<stream::DataPushStream<lin_algebra::vec3f> > listenr) {
	m_gps_pos_listener = listenr;
}

void SimulatorGpsSensor::set_speed_dir_listener(boost::shared_ptr<stream::DataPushStream<float> > listenr) {
	m_gps_speed_dir_listener = listenr;
}
void SimulatorGpsSensor::set_speed_mag_listener(boost::shared_ptr<stream::DataPushStream<float> > listenr) {
	m_gps_speed_mag_listener = listenr;
}


}  // namespace simulator
