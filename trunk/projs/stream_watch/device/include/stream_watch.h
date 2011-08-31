
#ifndef WATCH_H_
#define WATCH_H_

#include <boost/thread.hpp>
#include <boost/optional.hpp>

#include <stdexcept>

#include "generators.h"


/**
 * Thrown when a ground station or any other units does not wait for
 * connection.
 */
class HostException : public std::runtime_error {
public:
	HostException(std::string _what): runtime_error(_what) {}
};


/**
 * Connects to a server and sends the data.
 * Should work with WatchPresenter program.
 */
template <typename T, size_t N>
class Watch : public VecFilter<T,N> {
public:
	Watch(VecGenerator<T,N>* data_gen,
			std::string hostname,
			std::string watch_name,
			float minval,
			float maxval);

	virtual ~Watch();

	typename VecFilter<T,N>::vector_t get_data();

	void run(bool open_thread=true);

private:

	/**
	 * Connects to the server at m_host. Non blocking.
	 * @param - sock_fd - output param, the connected socket filedescryptor.
	 * @return - true if succeded, false otherwise.
	 */
	bool connect_to_host(int &sock_fd);

	boost::shared_ptr<boost::thread> m_thread;
	boost::optional< typename VecFilter<T,N>::vector_t > m_curr_data;

	std::string m_host;

	std::string m_watch_name;

	float m_minval;
	float m_maxval;

	const int PORT_NUM;
};

#include "stream_watch.inl"

#endif /* WATCH_H_ */
