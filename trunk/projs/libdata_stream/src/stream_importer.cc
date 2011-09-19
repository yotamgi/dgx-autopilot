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


StreamImporter::StreamImporter(std::string address):
	m_host_address(address)
{
	connect_to_host();
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
