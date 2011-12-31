#include <libgpsmm.h>
#include <iostream>

int main() {
	gpsmm mm;
	mm.open();
	if (mm.stream(WATCH_ENABLE | WATCH_JSON) == NULL) {
		std::cout << "No GPS!" << std::endl;
	}
	while (true) {
		if (!mm.waiting()) {
			continue;
		}
		struct gps_data_t* data = mm.poll();
		std::cout << data->fix.altitude << std::endl;
	}
	
}
