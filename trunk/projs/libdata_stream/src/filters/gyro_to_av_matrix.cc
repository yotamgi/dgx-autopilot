
#include "gyro_to_av_matrix.h"
#include <cmath>
#include "stream/util/time.h"

namespace stream {
namespace filters {

GyroToAVMatrix::GyroToAVMatrix(boost::shared_ptr<DataGenerator<lin_algebra::vec3f> > data_gen):
	DataFilter<lin_algebra::vec3f, lin_algebra::mat3f>(data_gen)
{}

GyroToAVMatrix::~GyroToAVMatrix() {}


lin_algebra::mat3f GyroToAVMatrix::get_data() {
	lin_algebra::vec3f data =
		DataFilter<lin_algebra::vec3f, lin_algebra::mat3f>::m_generator->get_data();

	float wx = -data[0] / 180. * lin_algebra::PI;
	float wy = -data[1] / 180. * lin_algebra::PI;
	float wz = -data[2] / 180. * lin_algebra::PI;

	lin_algebra::mat3f update; // = W + I
	update(0,0) = 0.;		update(0,1) = -1.*wz;	update(0,2) = wy;
	update(1,0) = wz;   	update(1,1) = 0.;   	update(1,2) = -1.*wx;
	update(2,0) = -1.*wy;  	update(2,1) = wx;   	update(2,2) = 0.;

	return  update;
}


} // namespace filters
} // namespace stream

