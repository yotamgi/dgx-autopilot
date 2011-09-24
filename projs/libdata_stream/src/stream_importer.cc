#include "stream_importer.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>

namespace stream {

StreamImporter::StreamImporter(std::string address):
	m_host_address(address),
	m_sock(0)
{
	connect_to_host();
}

StreamImporter::~StreamImporter() {
	if (m_sock) close(m_sock);
}

void StreamImporter::connect_to_host() {

	// init the connection
    struct sockaddr_in serv_addr;
    struct hostent *server;

    m_sock  = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sock  < 0) {
    	throw std::runtime_error("Could not create socket!");
    }

    server = gethostbyname(m_host_address.c_str());
    if (server == NULL) {
    	throw std::runtime_error("No such host");
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
	serv_addr.sin_port = htons(protocol::PORT);

    while (connect(m_sock,(struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    	std::cout << "Could not connect to " << m_host_address << ". Trying again in 1 second" << std::endl;
		sleep(1);
    }
}

void StreamImporter::write_sock(std::string data) {
	while (write(m_sock, data.c_str(), data.size()) != (signed)data.size())  {
		connect_to_host();
	}
}
std::string StreamImporter::read_sock() {
	const size_t BLOCK_SIZE = 100;
	char buff[BLOCK_SIZE];
	std::string data;
	while (read(m_sock, buff, BLOCK_SIZE) == (int)BLOCK_SIZE) {
		data += std::string(buff);
	}
	data += std::string(buff);
	std::cout << "Read " << data << std::endl;
	return data;
}

std::vector<std::string> StreamImporter::list_avail() {
	std::vector<std::string> avail;

	// ask for the streams
	write_sock(std::string(&protocol::LIST_COMMAND, 1));

	// get the data
	std::string data = read_sock();

	// parse the data
	size_t begin = 0;
	size_t end  = data.find(protocol::SEPERATOR, begin);
	while (end != std::string::npos) {
		std::string one = data.substr(begin, end-begin);
		avail.push_back(one);

		begin = end+1;
		end = data.find(protocol::SEPERATOR, begin);
	}

	return avail;
}

} // namespace stream
