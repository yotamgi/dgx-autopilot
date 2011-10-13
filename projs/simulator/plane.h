/*
 * plane.h
 *
 *  Created on: Aug 20, 2011
 *      Author: yotam
 */

#ifndef PLANE_H_
#define PLANE_H_

#include <boost/shared_ptr.hpp>
#include <stdexcept>

#include "stream/generators.h"
#include "flying_object.h"

namespace simulator {

/**
 * A class representing all the parameters that differ from one plane object
 * to another.
 */
struct PlainParams {

	PlainParams(const std::string& mesh_file,
				const std::string& texture_file,
				const irr::core::vector3df scale,
				float speed,
				float yaw_speed,					// degrees/sec
				float pitch_speed,					// degrees/sec
				float ailron_speed);				// degrees/sec


	const std::string& get_mesh_file() const { return m_mesh_file; }
	const std::string& get_texture_file() const { return m_texture_file; }
	const irr::core::vector3df& get_scale() const {	return m_scale; }
	float get_speed() const { return m_speed; }
	float get_yaw_speed() const { return m_yaw_speed; }
	float get_pitch_speed() const { return m_pitch_speed; }
	float get_ailron_speed() const { return m_ailron_speed; }

private:

	std::string m_mesh_file;
	std::string m_texture_file;
	irr::core::vector3df m_scale;
	float m_speed;
	float m_yaw_speed;
	float m_pitch_speed;
	float m_ailron_speed;
};


/**
 * The plane class - gets a plane param and creates a plane in the field.
 */
class Plane : public FlyingObject {
public:
	Plane(irr::IrrlichtDevice* device,
			irr::core::vector3df start_pos,
			const PlainParams plane_params);

	virtual void update(float time_delta);

	virtual irr::core::vector3df get_pos() const { return m_object->getPosition(); }

	// controlling the plane
	void set_pitch_servo(float percentage);
	void set_yaw_servo(float percentage);
	void set_ailron_servo(float percentage);

	/**
	 * Our generic sensor generator.
	 * The plane class will calculate the sensor data and tell every sensor
	 * what is its data.
	 */
	class SensorGenerator : public stream::VecGenerator<float,3> {
	public:
		vector_t get_data() {return m_data; }

		bool is_data_losed() { return false; }

		void set_data(vector_t data) { m_data = data; }

	private:
		vector_t m_data;
	};

	boost::shared_ptr<stream::VecGenerator<float,3> > gyro_gen() { return m_gyro; }
	boost::shared_ptr<stream::VecGenerator<float,3> > acc_gen() { return m_acc; }
	boost::shared_ptr<stream::VecGenerator<float,3> > compass_gen() { return m_compass; }

private:

	void update_sensors(float time_delta);
	irr::core::vector3df calc_angle_vel() const;

	boost::shared_ptr<SensorGenerator> m_gyro;
	boost::shared_ptr<SensorGenerator> m_acc;
	boost::shared_ptr<SensorGenerator> m_compass;

	/**
	 * Returns the angle_diff for this frame
	 * It uses the data from the servos and various other things.
	 * It is responsible to make it realistic.
	 */

	irr::core::vector3df m_direction;
	irr::core::vector3df m_priv_dir;

	irr::scene::ISceneNode * m_object;

	struct servo_states {
		float yaw_percentage;
		float pitch_percentage;
		float ailron_percentage;
	};

	servo_states m_servos;

	PlainParams m_params;

};

}  // namespace simulator


#endif /* PLANE_H_ */
