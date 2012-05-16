#ifdef _MSC_VER
// We'll also define this to stop MSVC complaining about sprintf().
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "Irrlicht.lib")
#endif

#include "plain.h"
#include "simulator.h"
#include "platforms.h"
#include "simulator_platform.h"

#include <autopilot/platform/platform_export_import.h>
#include <stream/util/tcpip_connection.h>

#include <boost/thread.hpp>
#include <cmath>
#include <boost/make_shared.hpp>
#include <sys/time.h>

void export_import(boost::shared_ptr<autopilot::NormalPlainPlatform> platform)
{

	while (true) {
		//while (!plain->data_ready());
		try {
			boost::shared_ptr<stream::TcpipClient> client =
					boost::make_shared<stream::TcpipClient>("localhost", 0x6060);

			autopilot::export_platform(platform, client);
		} catch (stream::ConnectionExceptioin& e) {
			std::cout << "Connection died" << std::endl;
		}
	}
}

int main(int argc, char** argv)
{
	simulator::WindGen::Params wind_params;
	wind_params.const_wind = lin_algebra::create_vec3f(0., 0, 0);
	wind_params.long_disturbance_strength = 0.0;
	wind_params.little_disturbance_strength = 0.0;

	if (argc == 2) {
		if (argv[1] == std::string("--wind")) {
			wind_params.const_wind = lin_algebra::create_vec3f(2., 0, 0);
			wind_params.long_disturbance_strength = 0.5;
			wind_params.little_disturbance_strength = 0.1;
		}
	}

	boost::shared_ptr<autopilot::NormalPlainPlatform> platform =
			simulator::create_simulator_platform(simulator::platforms::dgx_platform, wind_params);
	export_import(platform);

	return 0;
}
