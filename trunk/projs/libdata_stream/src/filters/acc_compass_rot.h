#ifndef ACC_TO_EULER_FILTER_H_
#define ACC_TO_EULER_FILTER_H_

#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>

#include <stream/util/lin_algebra.h>
#include <stream/generators.h>

typedef stream::DataGenerator<lin_algebra::vec3f> vec_stream_t ;

namespace stream {
namespace filters {

class AccCompassRotation : public DataGenerator<lin_algebra::mat3f> {
public:
	AccCompassRotation(
			boost::shared_ptr<vec_stream_t> acc,
			boost::shared_ptr<vec_stream_t> compass);

	AccCompassRotation(
			boost::shared_ptr<vec_stream_t> acc,
			boost::shared_ptr<vec_stream_t> compass,
			lin_algebra::vec3f expected_north);

	AccCompassRotation(
			boost::shared_ptr<vec_stream_t> acc,
			boost::shared_ptr<vec_stream_t> compass,
			float north_pitch_angle);

	virtual ~AccCompassRotation() {}

	lin_algebra::mat3f get_data();

	boost::shared_ptr<DataGenerator<float> > reliable_stream() { return m_reliable_stream; }

private:

	class AccCompassReliable : public DataGenerator<float> {
	public:
		AccCompassReliable(){}

		float get_data() { return reliability; }

	private:
		float reliability;
		friend class AccCompassRotation;
	};

	boost::shared_ptr<vec_stream_t> m_acc;
	boost::shared_ptr<vec_stream_t> m_compass;

	float m_north_pitch_angle;

	boost::shared_ptr<AccCompassReliable> m_reliable_stream;
};

} // namespace filters
} // namespace stream

#endif /* ACC_TO_EULER_FILTER_H_ */
