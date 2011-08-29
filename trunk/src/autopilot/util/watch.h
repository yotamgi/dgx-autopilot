
#ifndef WATCH_H_
#define WATCH_H_

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include "components/generators.h"
#include "common/exceptions.h"

/**
 * Connects to a server and sends the data.
 * Should work with WatchPresenter program.
 */
template <typename T, int channels>
class Watch : public DataFilter<T> {
public:
	Watch(DataGenerator<T>* data_gen,
			std::string hostname,
			std::string watch_name,
			float minval,
			float maxval);

	virtual ~Watch();

	boost::shared_ptr<T> get_data();

	void run(bool open_thread=true);

private:

	/**
	 * Connects to the server at m_host. Non blocking.
	 * @param - sock_fd - output param, the connected socket filedescryptor.
	 * @return - true if succeded, false otherwise.
	 */
	bool connect_to_host(int &sock_fd);

	boost::shared_ptr<boost::thread> m_thread;
	boost::shared_ptr<T> m_curr_data;

	std::string m_host;

	std::string m_watch_name;

	float m_minval;
	float m_maxval;

	const int PORT_NUM;
};

#include "watch.inl"

#endif /* WATCH_H_ */
