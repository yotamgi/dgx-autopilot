#ifndef GPS_H_
#define GPS_H_

#include <stream/data_push_stream.h>
#include <stream/util/lin_algebra.h>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <libgpsmm.h>

namespace autopilot {

typedef boost::shared_ptr<stream::DataPushStream<float> > 				mag_reciever_ptr;
typedef boost::shared_ptr<stream::DataPushStream<lin_algebra::vec3f> > 	vec3_reciever_ptr;

class Gps {
public:
	Gps();

	void set_pos_reciever_stream(vec3_reciever_ptr reciever);
	void set_speed_vec_reciever_stream(vec3_reciever_ptr reciever);
	void set_speed_mag_reciever_stream(mag_reciever_ptr reciever);
	void set_speed_dir_reciever_stream(mag_reciever_ptr reciever);

private:

	void update();

	boost::thread m_update_thread;
	mag_reciever_ptr m_speed_mag;
	mag_reciever_ptr m_speed_dir;
	vec3_reciever_ptr m_pos;
	vec3_reciever_ptr m_speed_vec;
	gpsmm m_gpsmm;
};


}  // namespace autopilot

#endif /* GPS_H_ */
