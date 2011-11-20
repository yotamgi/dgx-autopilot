#ifndef CONNECTION_H_
#define CONNECTION_H_

#include <string>
#include <stdexcept>
#include <boost/shared_ptr.hpp>

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

	virtual int fd() = 0;

};

class ConnectionFactory {
public:
	virtual boost::shared_ptr<Connection> get_connection() = 0;
};



}  // namespace stream

#endif /* CONNECTION_H_ */
