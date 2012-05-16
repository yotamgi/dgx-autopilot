#ifndef PLANE_H_
#define PLANE_H_

#include "carriable.h"

#include <stream/data_pop_stream.h>
#include <stream/data_push_stream.h>
#include <stream/stream_utils.h>
#include <stream/util/lin_algebra.h>
#include "flying_object.h"
#include "wind_gen.h"

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
				float rudder_intensity,				// degrees/sec at speed 100m/h
				float elevator_intensity,			// degrees/sec at speed 100m/h
				float ailron_intensity,				// degrees/sec at speed 100m/h
				float mass, 						// kg
				float engine_power,					// N
				float drag,
				float wings_area,					// m^2
				float wings_lift,					// min angle (degree) for the wing to lift
				float sideslide_yaw_effect_strenth,
				float dihedral_effect_strenth
	);


	const std::string& get_mesh_file() const { return m_mesh_file; }
	const std::string& get_texture_file() const { return m_texture_file; }
	const irr::core::vector3df& get_scale() const { return m_scale; }
	const irr::core::vector3df& get_rot() const { return m_rot; }
	float get_rudder_intensity() const { return m_rudder_intensity; }
	float get_elevator_intensity() const { return m_elevator_intensity; }
	float get_ailron_intensity() const { return m_ailron_intensity; }

	float get_mass() const { return m_mass; }
	float get_engine_power() const { return m_engine_power; }
	float get_drag() const { return m_drag; }
	float get_wings_area() const { return m_wings_area; }
	float get_wings_lift() const { return m_wings_lift; }

	float get_sideslide_yaw_effect_strenth() const { return m_sideslide_yaw_effect_strenth; }
	float get_dihedral_effect_strenth() const { return m_dihedral_effect_strenth; }

private:

	std::string m_mesh_file;
	std::string m_texture_file;
	const irr::core::vector3df m_scale;
	const irr::core::vector3df m_rot;
	float m_rudder_intensity;
	float m_elevator_intensity;
	float m_ailron_intensity;

	float m_mass;
	float m_engine_power;
	float m_drag;
	float m_wings_area;
	float m_wings_lift;

	// sideslide effects
	float m_sideslide_yaw_effect_strenth;
	float m_dihedral_effect_strenth;
};


typedef boost::shared_ptr<stream::DataPopStream<lin_algebra::vec3f> > sensor_stream_ptr_t;

/**
 * The plane class - gets a plane param and creates a plane in the field.
 */
class Plain : public FlyingObject, private boost::noncopyable {
public:
	Plain(irr::IrrlichtDevice* device,
			irr::core::vector3df start_pos,
			boost::shared_ptr<WindGen> wind_gen,
			const PlainParams plane_params);

	~Plain();

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

	typedef boost::shared_ptr<Servo> servo_stream_ptr_t;

	virtual void update(float time_delta);

	virtual irr::core::vector3df get_pos() const { return m_object->getPosition(); }

	bool data_ready() { return m_data_ready; }

	// controlling the plane
	servo_stream_ptr_t get_elevator_servo() { return m_elevator_servo; 	}
	servo_stream_ptr_t get_rudder_servo()   { return m_rudder_servo; 	}
	servo_stream_ptr_t get_ailron_servo()  	{ return m_ailron_servo; 	}
	servo_stream_ptr_t get_throttle_servo() { return m_throttle_servo; 	}

	void carry(boost::shared_ptr<Carriable> carried);

private:

	irr::core::vector3df calc_plane_acceleration(float time_delta);
	irr::core::vector3df calc_angle_vel(float time_delta);

	/**
	 * The plain's servos
	 */
	boost::shared_ptr<Servo> m_ailron_servo;
	boost::shared_ptr<Servo> m_elevator_servo;
	boost::shared_ptr<Servo> m_rudder_servo;
	boost::shared_ptr<Servo> m_throttle_servo;

	irr::core::vector3df m_velocity;
	irr::core::vector3df m_acceleration;

	irr::scene::ISceneNode * m_object;
	irr::core::matrix4 m_transformation;

	PlainParams m_params;

	float m_print_timer;

	volatile bool m_data_ready;

	std::vector<boost::shared_ptr<Carriable> > m_stuff;

	boost::shared_ptr<WindGen> m_wind_gen;
};

}  // namespace simulator


#endif /* PLANE_H_ */
