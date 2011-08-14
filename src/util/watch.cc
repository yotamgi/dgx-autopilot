#include "watch.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <boost/thread.hpp>


VecWatch::VecWatch(DataGenerator<vector_t>* data_gen, std::string hostname):
	DataFilter(data_gen),
	PORT_NUM(0x6666)
{
	// init the connection
    struct sockaddr_in serv_addr;
    struct hostent *server;

    m_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sock_fd < 0) {
        throw HostException("The watch could not connect to host");
    }

    server = gethostbyname(hostname.data());
    if (server == NULL) {
    	throw HostException("No such host");
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);

    serv_addr.sin_port = htons(PORT_NUM);
    if (connect(m_sock_fd,(struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        throw HostException("Error connecting to host");
    }
}

VecWatch::~VecWatch() {
    close(m_sock_fd);
}

boost::shared_ptr<vector_t> VecWatch::get_data() {

	m_curr_data = m_generator->get_data();
	return m_curr_data;
}

void VecWatch::run(bool open_thread) {
	if (open_thread) {
		m_thread.reset(new boost::thread(&VecWatch::run, this, false));
	} else {

		while (true) {
			if (m_curr_data) {
				char buff[4];

				// wait for asking
				ssize_t n = read(m_sock_fd, buff, 4);
				if (n != 4 || strncmp(buff, "GIVE", 4) != 0) {
					 throw HostException("Wierd server");
				}

				std::cout << "GOT GIVE" << std::endl;

				// write data
				n = write(m_sock_fd, (const char*)m_curr_data.get(), sizeof(vector_t));
				if (n < 0) {
					 throw HwExcetion("Could not write to host");
				}
			} // if
		} // while

	} // else
}
