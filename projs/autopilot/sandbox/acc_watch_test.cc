#include <stream/filters/euler_angles_integral.h>
#include <stream/filters/static_filter.h>
#include <stream/util/time.h>
#include <stream/stream_exporter.h>
#include "hw/adxl345_acc.h"


int main(int argc, char** argv) {
	Adxl345Acc gen(2);

	stream::StreamExporter exporter;
	exporter.register_stream(&gen, std::string("acc_test"));

	std::cout << "Exported stream. waiting for importers..." << std::endl;
	exporter.run();

	return 0;
}
