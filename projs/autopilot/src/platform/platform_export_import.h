#ifndef PLATFORM_EXPORT_IMPORT_H_
#define PLATFORM_EXPORT_IMPORT_H_

#include "interfaces/plain_platform.h"
#include <stream/connection.h> // for the exception
#include <string>

namespace autopilot {


/*
 * Useful functions to export NormalPlainPlatform structs over a
 * StreamConnection class.
 * It blocks.
 * @throws ConnectionException if any problem occures
 */
void export_platform(boost::shared_ptr<NormalPlainPlatform> platform,
		boost::shared_ptr<stream::ConnectionFactory> conn_factory);

/*
 * Useful functions to import NormalPlainPlatform structs over a
 * StreamConnection class.
 * It does not block.
 * @throws ConnectionException if any problem occures
 */
boost::shared_ptr<NormalPlainPlatform> import_platform(
		boost::shared_ptr<stream::ConnectionFactory> conn_factory);

}  // namespace autopilot

#endif /* PLATFORM_EXPORT_IMPORT_H_ */
