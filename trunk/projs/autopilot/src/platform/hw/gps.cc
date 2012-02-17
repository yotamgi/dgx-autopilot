#include "gps.h"

namespace autopilot {

Gps::Gps():m_update_thread(Gps::update)
{

}


void Gps::set_reciever_stream(reciever_ptr reciever) {
	m_reciever = reciever;
}

void Gps::update() {
	// init
	m_gpsmm.open();
	if (mm.stream(WATCH_ENABLE | WATCH_JSON) == NULL) {
		std::cout << "No GPS!" << std::endl;
	}

	while (true) {
		struct gps_data_t* data = mm.poll();
		std::cout << data->fix.altitude << std::endl;
	}

}

}  // namespace autopilot
