#include "cockpit_player.h"
#include <string>
#include <iostream>

void usage_and_exit(std::string arg0) {
	std::cout << arg0 << " <play_dir>  [--help] " << std::endl;
	exit(1);
}


int main(int argc, char** argv) {
	// commandline parsing

	if (argc != 2)							usage_and_exit(argv[0]);
	if (std::string(argv[1]) == "--help")	usage_and_exit(argv[0]); 

	std::string play_dir = argv[1];

	CockpitPlayer cp(play_dir);
	cp.run();
}
