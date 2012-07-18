#include "gps.h"

namespace autopilot {

Gps::Gps():m_update_thread(&Gps::update, this)
{}

void Gps::set_pos_reciever_stream(vec3_reciever_ptr reciever) {
	m_pos = reciever;
}
void Gps::set_speed_vec_reciever_stream(vec3_reciever_ptr reciever) {
	m_speed_vec = reciever;
}
void Gps::set_speed_mag_reciever_stream(mag_reciever_ptr reciever) {
	m_speed_mag = reciever;
}
void Gps::set_speed_dir_reciever_stream(mag_reciever_ptr reciever) {
	m_speed_dir = reciever;
}


void Gps::update() {
	// init
	m_gpsmm.open();
	if (m_gpsmm.stream(WATCH_ENABLE | WATCH_JSON) == NULL) {
		throw stream::PushStreamException("No GPS Found");
	}

	float height = 0.;
	while (true) {
		struct gps_data_t* data = m_gpsmm.poll();

		if (data->fix.mode == 3) {
			height = data->fix.altitude;
		}

		if (data->fix.mode >= 2) {
			if (m_pos) {
				lin_algebra::vec3f pos = lin_algebra::create_vec3f(
						data->fix.longitude, data->fix.latitude, height
				);
				m_pos->set_data(pos);
 			}

			if (m_speed_vec) {
				lin_algebra::vec3f speed_vec = lin_algebra::create_vec3f(
						data->fix.speed * std::cos(data->fix.track / 180. * M_PI),
						data->fix.climb,
						data->fix.speed * std::sin(data->fix.track / 180. * M_PI)
				);
				m_speed_vec->set_data(speed_vec);
			}

			if (m_speed_dir) {
				m_speed_dir->set_data(data->fix.track);
			}

			if (m_speed_mag) {
				m_speed_mag->set_data(data->fix.speed);
			}
		}
	}
}

}  // namespace autopilot
