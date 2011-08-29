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
#include <stdexcept>


template <typename T, int channels>
Watch<T, channels>::Watch(DataGenerator<T>* data_gen,
					std::string hostname,
					std::string watch_name,
					float minval,
					float maxval):
	DataFilter<T>(data_gen),
	m_host(hostname),
	m_watch_name(watch_name),
	m_minval(minval),
	m_maxval(maxval),
	PORT_NUM(0x6666)
{}

template <typename T, int channels>
bool Watch<T, channels>::connect_to_host(int& sock_fd) {
	// init the connection
    struct sockaddr_in serv_addr;
    struct hostent *server;

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
    	throw std::runtime_error("Could not create socket!");
    }

    server = gethostbyname(m_host.data());
    if (server == NULL) {
    	throw HostException("No such host");
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);

    serv_addr.sin_port = htons(PORT_NUM);
    if (connect(sock_fd,(struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        return false;
    }
    return true;
}

template <typename T, int channels>
Watch<T, channels>::~Watch() {}

template <typename T, int channels>
boost::shared_ptr<T> Watch<T, channels>::get_data() {

	m_curr_data = DataFilter<T>::m_generator->get_data();
	return m_curr_data;
}

template <typename T, int channels>
void Watch<T, channels>::run(bool open_thread) {
	if (open_thread) {
		m_thread.reset(new boost::thread(&VecWatch::run, this, false));
	} else {

		while(true) {

			// wait until there is data to send
			while (!m_curr_data) {}

			// try to connect to server
			int sock_fd;
			while (!connect_to_host(sock_fd)) {
				std::cout << "Trying..." << std::endl;
				usleep(100000);
			}

			std::cout << "Connected" << std::endl;

			// send first messages:
			std::stringstream ss;
			ss << m_watch_name << ";" << m_minval << ";" << m_maxval << ";" << channels << ";";
			assert(ss.str().size() < 100);
			ss << std::string('-', 100 - ss.str().size());
			write(sock_fd, ss.str().c_str(), 100);


			// connected. now talk to server
			while (true) {
				char buff[4];

				// wait for asking
				ssize_t n = read(sock_fd, buff, 4);
				if (n != 4 || strncmp(buff, "GIVE", 4) != 0) {
					std::cout << "server closed" << std::endl;
					break;
				}

				std::cout << "GOT GIVE" << std::endl;

				// write data
				n = write(sock_fd, (const char*)m_curr_data.get(), sizeof(T));
				if (n < 0) {
					std::cout << "server closed" << std::endl;
					break;
				}
			} // while
		} // while

	} // else
}
