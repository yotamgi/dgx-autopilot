
#ifndef GPS_LISTENER_H_
#define GPS_LISTENER_H_

#include <stream/data_pop_stream.h>
#include <stream/data_push_stream.h>
#include <stream/stream_utils.h>
#include <stream/util/lin_algebra.h>
#include <boost/circular_buffer.hpp>
#include <boost/thread/mutex.hpp>

class SimpleGpsFilter : public stream::DataPushStream<lin_algebra::vec3f>
{
public:
	SimpleGpsFilter(size_t avarge_len);

	/** The function for inserting the raw gps data */
	void set_data(const lin_algebra::vec3f& data);

	/** Functions for getting the output of the gps filter */
	boost::shared_ptr<stream::DataPopStream<lin_algebra::vec2f> > get_position_stream();
	boost::shared_ptr<stream::DataPopStream<float> > 			  get_alt_stream();
	boost::shared_ptr<stream::DataPopStream<lin_algebra::vec3f> > get_speed_stream();
	boost::shared_ptr<stream::DataPopStream<float> > 			  get_speed_reliable_stream();

private:

	size_t m_avarge_len;

	boost::circular_buffer<lin_algebra::vec3f> m_samples;

	boost::shared_ptr<stream::PushToPopConv<lin_algebra::vec2f> > m_position_stream;
	boost::shared_ptr<stream::PushToPopConv<float> >			  m_alt_stream;
	boost::shared_ptr<stream::PushToPopConv<lin_algebra::vec3f> > m_speed_stream;
	boost::shared_ptr<stream::PushToPopConv<float> > 			  m_speed_reliable_stream;
};

#endif /* GPS_LISTENER_H_ */
