#ifndef GPS_H_
#define GPS_H_

#include <libgpsmm.h>
#include <stream/data_push_stream.h>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

namespace autopilot {

typedef boost::shared_ptr<stream::DataPushStream> reciever_ptr;

class Gps {
	Gps();

	void set_reciever_stream(reciever_ptr reciever);

private:

	void update();

	boost::thread m_update_thread;
	reciever_ptr m_reciever;
	gpsmm m_gpsmm;
};


}  // namespace autopilot

#endif /* GPS_H_ */
