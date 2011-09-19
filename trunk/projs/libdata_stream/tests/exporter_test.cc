#include <gtest/gtest.h>
#include "stream_exporter.h"

TEST(server, functional) {
	std::cout << "Running test " << std::endl;
	StreamExporter a;
	a.run();
}
