#ifndef PLANE_H_
#define PLANE_H_

#include "carriable.h"

#include <stream/data_pop_stream.h>
#include <stream/data_push_stream.h>
#include <stream/stream_utils.h>
#include <stream/util/lin_algebra.h>
#include "flying_object.h"

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/noncopyable.hpp>
#include <boost/circular_buffer.hpp>
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
				const irr::core::vector3df rot,
				float yaw_speed,					// degrees/sec
				float pitch_speed,					// degrees/sec
				float ailron_speed,					// degrees/sec
				float mass, 						// kg
				float engine_power,					// N
				float drag,
				float wings_area,					// m^2
				float wings_lift);					// min angle (degree) for the wing to lift


	const std::string& get_mesh_file() const { return m_mesh_file; }
	const std::string& get_texture_file() const { return m_texture_file; }
	const irr::core::vector3df& get_scale() const { return m_scale; }
	const irr::core::vector3df& get_rot() const { return m_rot; }
	float get_yaw_speed() const { return m_yaw_speed; }
	float get_pitch_speed() const { return m_pitch_speed; }
	float get_ailron_speed() const { return m_ailron_speed; }

	float get_mass() const { return m_mass; }
	float get_engine_power() const { return m_engine_power; }
	float get_drag() const { return m_drag; }
	float get_wings_area() const { return m_wings_area; }
	float get_wings_lift() const { return m_wings_lift; }

private:

	std::string m_mesh_file;
	std::string m_texture_file;
	const irr::core::vector3df m_scale;
	const irr::core::vector3df m_rot;
	float m_yaw_speed;
	float m_pitch_speed;
	float m_ailron_speed;

	float m_mass;
	float m_engine_power;
	float m_drag;
	float m_wings_area;
	float m_wings_lift;
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

	~Plane();

	virtual void update(float time_delta);

	virtual irr::core::vector3df get_pos() const { return m_object->getPosition(); }

	bool data_ready() { return m_data_ready; }

	// controlling the plane
	servo_stream_ptr_t get_pitch_servo() { return m_pitch_servo; 	}
	servo_stream_ptr_t get_yaw_servo()   { return m_yaw_servo; 		}
	servo_stream_ptr_t get_tilt_servo()  { return m_tilt_servo; 	}
	servo_stream_ptr_t get_gas_servo()   { return m_gas_servo; 		}

	// forcing servos methods
	void force_tilt(float howmuch) 		{ m_tilt_servo->override(howmuch); 	}
	void unforce_tilt() 				{ m_tilt_servo->stop_override();	}
	void force_pitch(float howmuch) 	{ m_pitch_servo->override(howmuch); }
	void unforce_pitch()				{ m_pitch_servo->stop_override(); 	}

	void carry(boost::shared_ptr<Carriable> carried);

private:

	class Servo : public stream::DataPushStream<float> {
	public:
		Servo(float speed, float inital_val=0.); // speed deg/sec

		void set_data(const float& data);
		void override(const float& data);
		void stop_override();

		float get_data(float time_delta);

	private:
		float m_speed;
		float m_state;

		bool m_override;
		float m_override_target;
		float m_target;
	};

	irr::core::vector3df calc_plane_acceleration(float time_delta);
	irr::core::vector3df calc_angle_vel(float time_delta);

	/**
	 * The plain's servos
	 */
	boost::shared_ptr<Servo> m_tilt_servo;
	boost::shared_ptr<Servo> m_pitch_servo;
	boost::shared_ptr<Servo> m_yaw_servo;
	boost::shared_ptr<Servo> m_gas_servo;

	irr::core::vector3df m_velocity;

	irr::scene::ISceneNode * m_object;
	irr::core::matrix4 m_transformation;

	PlainParams m_params;

	float m_print_timer;

	volatile bool m_data_ready;

	std::vector<boost::shared_ptr<Carriable> > m_stuff;
};

}  // namespace simulator


#endif /* PLANE_H_ */
