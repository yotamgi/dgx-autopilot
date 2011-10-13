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

/**
 * Specific cockpit for normal plains.
 */
class Cockpit : public NormalPlainCockpit {
public:

	Cockpit(boost::shared_ptr<NormalPlainPlatform> platform);
	virtual ~Cockpit() {}

	/* NormalPlainCockpit Implementation */

	boost::shared_ptr<stream::VecGenerator<float,3> > orientation();

	stream::DataGenerator<float>* speed();

	Servo* tilt_servo();

	Servo* yaw_servo();

	Servo* pitch_servo();

	Servo* gas_servo();

	/* Extending Fcuntions */
	boost::shared_ptr<stream::VecGenerator<float,3> > orientation_gyro();
	boost::shared_ptr<stream::VecGenerator<float,3> > orientation_rest();
	boost::shared_ptr<stream::DataGenerator<float> > rest_reliablity();

private:
	boost::shared_ptr<NormalPlainPlatform> m_platform;

	boost::shared_ptr<stream::DataGenerator<lin_algebra::matrix_t> > m_gyro_orientation;
	boost::shared_ptr<stream::DataGenerator<lin_algebra::matrix_t> > m_rest_orientation;
	boost::shared_ptr<stream::DataGenerator<lin_algebra::matrix_t> > m_orientation;

	boost::shared_ptr<stream::DataGenerator<float> > m_rest_reliability;

};

}  // namespace autopilot

#endif /* COCKPIT_H_ */
