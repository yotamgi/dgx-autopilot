#include "ground_station.h"
#include <string>
#include <iostream>

void usage_and_exit(std::string arg0) {
	std::cout << arg0 << " <plane_address>  [--help] " << std::endl;
	exit(1);
}


int main(int argc, char** argv) {
	// commandline parsing
	std::string plane_address;

	if (argc != 2)							usage_and_exit(argv[0]);
	if (std::string(argv[1]) == "--help")	usage_and_exit(argv[0]); 

	plane_address = argv[1];

	GroundStation gs(plane_address);
	gs.run();
}
