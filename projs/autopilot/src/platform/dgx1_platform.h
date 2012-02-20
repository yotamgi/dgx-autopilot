#ifndef DGX1_PLATFORM_H_
#define DGX1_PLATFORM_H_

#include "interfaces/plain_platform.h"
#include "platform/hw/gps.h"
#include <stream/data_pop_stream.h>
#include <stream/data_push_stream.h>
#include <stream/connection.h>
#include <stream/stream_connection.h>
#include <stdexcept>

namespace autopilot {


NormalPlainPlatform create_dgx1_platform();

NormalPlainPlatform create_dgx1_simulator_platform(
		boost::shared_ptr<stream::ConnectionFactory> conn_factory);


} //namespace autopilot

#endif /* DGX1_PLATFORM_H_ */
