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

int main()
{
	boost::shared_ptr<autopilot::NormalPlainPlatform> platform = simulator::create_simulator_platform(simulator::platforms::dgx_platform);
	export_import(platform);

	return 0;
}
