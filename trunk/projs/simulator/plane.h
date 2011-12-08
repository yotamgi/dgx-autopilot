#ifndef PLANE_H_
#define PLANE_H_


#include <stream/data_pop_stream.h>
#include <stream/data_push_stream.h>
#include <stream/stream_utils.h>
#include <stream/util/lin_algebra.h>
#include "flying_object.h"

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/noncopyable.hpp>
#include <stdexcept>

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

typedef boost::shared_ptr<stream::DataPushStream<float> > servo_stream_ptr_t;
typedef boost::shared_ptr<stream::DataPopStream<lin_algebra::vec3f> > sensor_stream_ptr_t;

/**
 * The plane class - gets a plane param and creates a plane in the field.
 */
class Plane : public FlyingObject, private boost::noncopyable {
public:
	Plane(irr::IrrlichtDevice* device,
			irr::core::vector3df start_pos,
			const PlainParams plane_params);

	virtual void update(float time_delta);

	virtual irr::core::vector3df get_pos() const { return m_object->getPosition(); }

	// controlling the plane
	servo_stream_ptr_t get_pitch_servo() { return m_pitch_servo; }
	servo_stream_ptr_t get_yaw_servo()   { return m_yaw_servo; }
	servo_stream_ptr_t get_tilt_servo()  { return m_tilt_servo; }

	// getting data from sensors
	sensor_stream_ptr_t gyro_gen() { return m_gyro; }
	sensor_stream_ptr_t acc_gen() { return m_acc; }
	sensor_stream_ptr_t compass_gen() { return m_compass; }
	void set_gps_listener(boost::shared_ptr<stream::DataPushStream<lin_algebra::vec3f> > listenr);

private:

	void update_sensors(float time_delta);
	irr::core::vector3df calc_angle_vel() const;
	void gps_update(); // blocking

	/**
	 * all the streams that the plane simulates.
	 */
	boost::shared_ptr<stream::PushToPopConv<lin_algebra::vec3f> > m_gyro;
	boost::shared_ptr<stream::PushToPopConv<lin_algebra::vec3f> > m_acc;
	boost::shared_ptr<stream::PushToPopConv<lin_algebra::vec3f> > m_compass;

	boost::shared_ptr<stream::PushToPopConv<float> > m_tilt_servo;
	boost::shared_ptr<stream::PushToPopConv<float> > m_pitch_servo;
	boost::shared_ptr<stream::PushToPopConv<float> > m_yaw_servo;

	boost::shared_ptr<stream::DataPushStream<lin_algebra::vec3f> > m_gps_listener;

	irr::core::vector3df m_direction;
	irr::core::vector3df m_priv_dir;

	irr::scene::ISceneNode * m_object;

	PlainParams m_params;
	irr::core::vector3df m_gyro_drift;

	boost::thread m_gps_thread;

};

}  // namespace simulator


#endif /* PLANE_H_ */
