/*
 * Cockpit.h
 *
 *  Created on: Oct 9, 2011
 *      Author: yotam
 */

#ifndef COCKPIT_H_
#define COCKPIT_H_

#include "interfaces/plain_cockpit.h"
#include "interfaces/plain_platform.h"
#include <stream/util/lin_algebra.h>

#include <boost/shared_ptr.hpp>

namespace autopilot {

typedef boost::shared_ptr<stream::DataGenerator<lin_algebra::vector_t> > vec_stream_ptr;

/**
 * Specific cockpit for normal plains.
 */
class Cockpit : public NormalPlainCockpit {
public:

	Cockpit(boost::shared_ptr<NormalPlainPlatform> platform);
	virtual ~Cockpit() {}

	/* NormalPlainCockpit Implementation */

	vec_stream_ptr orientation();

	stream::DataGenerator<float>* speed();

	Servo* tilt_servo();

	Servo* yaw_servo();

	Servo* pitch_servo();

	Servo* gas_servo();

	/* Extending Fcuntions */
	vec_stream_ptr orientation_gyro();
	vec_stream_ptr orientation_rest();
	boost::shared_ptr<stream::DataGenerator<float> > rest_reliablity();

private:
	boost::shared_ptr<NormalPlainPlatform> m_platform;

	boost::shared_ptr<stream::DataGenerator<lin_algebra::matrix_t> > m_gyro_orientation;
	boost::shared_ptr<stream::DataGenerator<lin_algebra::matrix_t> > m_rest_orientation;
	boost::shared_ptr<stream::DataGenerator<lin_algebra::matrix_t> > m_orientation;

	boost::shared_ptr<stream::DataGenerator<float> > m_rest_reliability;

};

}  // namespace autopilot

#include "cockpit.inl"

#endif /* COCKPIT_H_ */
