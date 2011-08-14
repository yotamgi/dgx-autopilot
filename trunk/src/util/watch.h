
#ifndef WATCH_H_
#define WATCH_H_

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include "components/generators.h"
#include "common/types.h"
#include "common/exceptions.h"

/**
 * Connects to a server and sends the data.
 * Should work with WatchPresenter program.
 */
class VecWatch : DataFilter<vector_t> {
public:
	VecWatch(DataGenerator<vector_t>* data_gen, std::string hostname);
	virtual ~VecWatch();

	boost::shared_ptr<vector_t> get_data();

	void run(bool open_thread=true);

private:

	boost::shared_ptr<boost::thread> m_thread;
	boost::shared_ptr<vector_t> m_curr_data;

	int m_sock_fd;
	const int PORT_NUM;
};


#endif /* WATCH_H_ */
