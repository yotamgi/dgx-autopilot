/*
 * protocol.h
 *
 *  Created on: Sep 19, 2011
 *      Author: yotam
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include <string>

namespace stream {

namespace protocol {

	const size_t PORT = 0x6666;

	const char GET_COMMAND = 'G';

	const char LIST_COMMAND = 'L';

	const std::string NOT_EXIST_COMMAND = "NOT_EXIST";

	const std::string SEPERATOR = "|";

	const char END = 'E';

}  // namespace protocol

} // namespace stream

#endif /* PROTOCOL_H_ */
