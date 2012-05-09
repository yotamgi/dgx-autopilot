#ifndef COMMAND_PROTOCOL_H_
#define COMMAND_PROTOCOL_H_

#include <string>

namespace commands {

	const int port = 0x6061;

	const std::string NAVIGATE_TO = "NAVTO";

	const std::string SWITCH_TO_WAYPOINT_PILOT = "TOWP";

	const std::string SWITCH_TO_SA_PILOT = "TOSA";
}

#endif /* COMMAND_PROTOCOL_H_ */
