#ifndef FUSION_FILTER_H_
#define FUSION_FILTER_H_

#include <boost/shared_ptr.hpp>

#include <stream/util/lin_algebra.h>
#include <stream/data_pop_stream.h>
#include <stream/stream_utils.h>
#include <stream/util/time.h>

typedef stream::DataPopStream<lin_algebra::vec3f> vec_stream_t;
typedef stream::PushGenerator<float> float_push_gen_t;

namespace autopilot {

class FusionFilter : public stream::DataPopStream<lin_algebra::vec3f> {
public:
	FusionFilter(
			boost::shared_ptr<vec_stream_t> acc,
			boost::shared_ptr<vec_stream_t> compass,
			boost::shared_ptr<vec_stream_t> gyro,
			boost::shared_ptr<float_push_gen_t> speed = boost::shared_ptr<float_push_gen_t>());

	FusionFilter(
			boost::shared_ptr<vec_stream_t> acc,
			boost::shared_ptr<vec_stream_t> compass,
			boost::shared_ptr<vec_stream_t> gyro,
			lin_algebra::vec3f expected_north,
			boost::shared_ptr<float_push_gen_t> speed = boost::shared_ptr<float_push_gen_t>());

	FusionFilter(
			boost::shared_ptr<vec_stream_t> acc,
			boost::shared_ptr<vec_stream_t> compass,
			boost::shared_ptr<vec_stream_t> gyro,
			float north_pitch_angle,
			boost::shared_ptr<float_push_gen_t> speed = boost::shared_ptr<float_push_gen_t>());

	virtual ~FusionFilter() {}

	void calibrate(lin_algebra::vec3f calibration_data);

	lin_algebra::vec3f get_data();

	boost::shared_ptr<stream::DataPopStream<float> > get_reliability_stream()
			{ return m_reliable_stream; }

	boost::shared_ptr<vec_stream_t> get_rest_orientation_stream()
				{ return m_rest_orientation; }

	boost::shared_ptr<vec_stream_t> get_fixed_acc_stream()
				{ return m_fixed_acc; }


private:

	lin_algebra::vec3f filter(lin_algebra::vec3f acc_data,
							  lin_algebra::vec3f compass_data,
							  lin_algebra::vec3f gyro_data);

	float understand_reliability(lin_algebra::vec3f ground,
			 				     lin_algebra::vec3f north);

	lin_algebra::mat3f acc_compass_orientation(lin_algebra::vec3f ground,
											   lin_algebra::vec3f north);

	lin_algebra::vec3f matrix_to_euler(lin_algebra::mat3f rot_mat);

	lin_algebra::mat3f av_to_rot_mat(lin_algebra::vec3f av);

	lin_algebra::mat3f maintain_orthonormal(lin_algebra::mat3f m);


	lin_algebra::mat3f m_state;
	Timer m_timer;

	boost::shared_ptr<vec_stream_t> m_acc;
	boost::shared_ptr<vec_stream_t> m_compass;
	boost::shared_ptr<vec_stream_t> m_gyro;
	boost::shared_ptr<stream::PushToPopConv<float> > m_curr_speed;

	float m_north_pitch_angle;

	boost::shared_ptr<stream::PushToPopConv<float> > m_reliable_stream;
	boost::shared_ptr<stream::PushToPopConv<lin_algebra::vec3f> > m_rest_orientation;
	boost::shared_ptr<stream::PushToPopConv<lin_algebra::vec3f> > m_fixed_acc;

	lin_algebra::vec3f m_calibration_data;

	static const size_t ACC_LOW_PASS;
};

}  // namespace autopilot


#endif /* FUSION_FILTER_H_ */
