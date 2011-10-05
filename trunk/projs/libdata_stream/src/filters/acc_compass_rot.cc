#include "acc_compass_rot.h"

namespace stream {
namespace filters {

AccCompassRotation::AccCompassRotation(VecGenerator<float,3> *acc, VecGenerator<float,3> *compass) {

}


lin_algebra::matrix_t AccCompassRotation::get_data() {


	// get the sensor data
	typename VecGenerator<float,3>::vector_t ground_data, north_data;
	north_data = m_compass->get_data();
	ground_data = m_acc->get_data();

	// convert it to comfortable vectors
	lin_algebra::vector_t ground(3), north(3);
	north[0] = north_data[0]; 	north[1] = north_data[1]; 	north[2] = north_data[2];
	ground[0] = ground_data[0]; ground[1] = ground_data[1]; ground[2] = ground_data[2];

	// create the rotation matrix and fill its components
	lin_algebra::matrix_t rot;

	// the y we know, so just set it
	lin_algebra::mat_row(rot, 1) = -1. * ground;

	// calculating the x using the north
	lin_algebra::orthogonalize(north, ground);
	lin_algebra::mat_row(rot, 0) = north;

	// calculate z as the vector orthogonal to x and y
	lin_algebra::mat_row(rot, 2) = lin_algebra::cross_product(north, ground);

	return rot;
}

} // namespace filters
} // namespace stream
