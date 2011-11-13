#ifndef CONNECTION_H_
#define CONNECTION_H_

#include <string>
#include <stdexcept>

namespace stream {

class ConnectionExceptioin : public std::runtime_error {
public:
	ConnectionExceptioin(std::string what): std::runtime_error(what) {}
	~ConnectionExceptioin() throw() {}
};

class Connection {
public:
	~Connection(){}

	virtual void write(std::string) = 0;
	virtual std::string read() = 0;

};


}  // namespace stream

#endif /* CONNECTION_H_ */
