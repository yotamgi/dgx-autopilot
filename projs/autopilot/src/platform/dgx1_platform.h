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

typedef stream::filters::WatchFilter<lin_algebra::vec3f> 	vec3_watch_stream;
typedef stream::DataPopStream<lin_algebra::vec3f>			vec3_stream;
typedef boost::shared_ptr<vec3_stream> 						vec3_stream_ptr;
typedef stream::DataPushStream<float> 						servo_stream;


NormalPlainPlatform create_dgx1_platform();

NormalPlainPlatform create_dgx1_simulator_platform(
		boost::shared_ptr<stream::ConnectionFactory> conn_factory);


} //namespace autopilot

#endif /* DGX1_PLATFORM_H_ */
