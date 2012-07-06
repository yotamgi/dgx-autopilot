#include "dgx1_platform.h" 
#include "hw/l3g4200d_gyro.h"
#include "hw/adxl345_acc.h"
#include "hw/itg3200_gyro.h"
#include "hw/hmc5843_compass.h"
#include "hw/bmp085_baro.h"
#include "hw/ads1115_adc.h"
#include "hw/maestro_servo_controller.h"
#include "hw/beagleboard_gpio.h"
#include "hw/mpxv7002_pitot.h"
#include "flapron_controller.h"
#include <stream/stream_connection.h>
#include <stream/stream_utils.h>

#include <boost/make_shared.hpp>

static void stam() {}
typedef stream::PushForwarder<lin_algebra::vec3f> vec3_push_forwarder;
typedef stream::PushForwarder<float> float_push_forwarder;

namespace autopilot {

class AliveGpioInformer {
public:
	AliveGpioInformer(int port):m_pin(port) {}

	/**
	 * Shake that pin
	 */
	void operator()() {
		m_pin = !(bool)m_pin;
	}

private:
	GpioPin m_pin;
};

class BatteryState : public stream::DataPopStream<float> {
public:
	BatteryState(boost::shared_ptr<autopilot::Ads1115_ADC> adc, uint8_t channel):
		m_adc(adc),
		m_channel(channel)
	{
		m_adc->set_channel_range(m_channel, Ads1115_ADC::V_4_096);
	}

	float get_data() {
		return (float(m_adc->read_channel(m_channel)) / 0x7fff) * 4.096f * 4.0f;
	}

private:
	boost::shared_ptr<autopilot::Ads1115_ADC> m_adc;
	uint8_t m_channel;
};

NormalPlainPlatform create_dgx1_platform() {
	NormalPlainPlatform dgx1_platform;
	dgx1_platform.acc_sensor = boost::make_shared<Adxl345Acc>(2,
			(Adxl345Acc::Direction[3]){ Adxl345Acc::X_DIR, Adxl345Acc::Z_DIR, Adxl345Acc::Y_DIR },
			(bool[3]){ true, false, true}
	);

	dgx1_platform.gyro_sensor = boost::make_shared<Itg3200Gyro>(2,
			(Itg3200Gyro::Direction[3]){ Itg3200Gyro::Y_DIR, Itg3200Gyro::Z_DIR, Itg3200Gyro::X_DIR },
			(bool[3]){ false, true, true }
	);

	dgx1_platform.compass_sensor = boost::make_shared<Hmc5843Compass>(2,
			(Hmc5843Compass::Direction[3]){ Hmc5843Compass::X_DIR, Hmc5843Compass::Z_DIR, Hmc5843Compass::Y_DIR },
			(bool[3]){ true, false, true}
	);

	boost::shared_ptr<vec3_push_forwarder> pos_forwarder(new vec3_push_forwarder);
	boost::shared_ptr<float_push_forwarder> speed_dir_forwarder(new float_push_forwarder);
	boost::shared_ptr<float_push_forwarder> speed_mag_forwarder(new float_push_forwarder);

	static Gps gps_sensor;
	gps_sensor.set_pos_reciever_stream(pos_forwarder);
	gps_sensor.set_speed_dir_reciever_stream(speed_dir_forwarder);
	gps_sensor.set_speed_mag_reciever_stream(speed_mag_forwarder);

	dgx1_platform.gps_pos_generator = pos_forwarder;
	dgx1_platform.gps_speed_mag_generator = speed_mag_forwarder;
	dgx1_platform.gps_speed_dir_generator = speed_dir_forwarder;

	// the alive
	//dgx1_platform.alive = AliveGpioInformer(130);
	dgx1_platform.alive = stam;

	// fill the platform's servos
	static MaestroServoController maestro("/dev/ttyACM0");
	FlapronController flapron(maestro.getServoChannel(0), maestro.getServoChannel(4));
	dgx1_platform.tilt_servo	= maestro.getServoChannel(1);
	dgx1_platform.pitch_servo 	= flapron.get_ailron_servo();
	dgx1_platform.gas_servo 	= maestro.getServoChannel(2);
	dgx1_platform.yaw_servo 	= maestro.getServoChannel(3);

	// trim the servos
	maestro.getServoChannel(2)->trim_middle(52.5f);
	maestro.getServoChannel(2)->trim_range(95.0f);

	dgx1_platform.yaw_servo->set_data(50.0f);

	return dgx1_platform;
}

NormalPlainPlatform create_dgx1_2_platform() {
	NormalPlainPlatform dgx1_platform;

	dgx1_platform.acc_sensor = boost::make_shared<Adxl345Acc>(2,
			(Adxl345Acc::Direction[3]){ Adxl345Acc::Y_DIR, Adxl345Acc::Z_DIR, Adxl345Acc::X_DIR },
			(bool[3]){ true, false, false }
	);

	dgx1_platform.gyro_sensor = boost::make_shared<L3g4200dGyro>(2,
			(L3g4200dGyro::Direction[3]){ L3g4200dGyro::Y_DIR, L3g4200dGyro::Z_DIR, L3g4200dGyro::X_DIR },
			(bool[3]){ true, false, false }
	);

	dgx1_platform.compass_sensor = boost::make_shared<Hmc5843Compass>(2,
			(Hmc5843Compass::Direction[3]){ Hmc5843Compass::Z_DIR, Hmc5843Compass::Y_DIR, Hmc5843Compass::X_DIR },
			(bool[3]){ false, true, true}
	);

	dgx1_platform.alt_sensor = boost::make_shared<autopilot::Bmp085baro>(2, true);

	boost::shared_ptr<autopilot::Ads1115_ADC> adc = boost::make_shared<autopilot::Ads1115_ADC>(2);
	dgx1_platform.airspeed_sensor = boost::make_shared<autopilot::Mpxv7002Pitot>(adc, 0);
	dgx1_platform.battery_sensor = boost::make_shared<BatteryState>(adc, 1);

	boost::shared_ptr<vec3_push_forwarder> pos_forwarder(new vec3_push_forwarder);
	boost::shared_ptr<float_push_forwarder> speed_dir_forwarder(new float_push_forwarder);
	boost::shared_ptr<float_push_forwarder> speed_mag_forwarder(new float_push_forwarder);

	static Gps gps_sensor;
	gps_sensor.set_pos_reciever_stream(pos_forwarder);
	gps_sensor.set_speed_dir_reciever_stream(speed_dir_forwarder);
	gps_sensor.set_speed_mag_reciever_stream(speed_mag_forwarder);

	dgx1_platform.gps_pos_generator = pos_forwarder;
	dgx1_platform.gps_speed_mag_generator = speed_mag_forwarder;
	dgx1_platform.gps_speed_dir_generator = speed_dir_forwarder;

	// the alive
	//dgx1_platform.alive = AliveGpioInformer(130);
	dgx1_platform.alive = stam;

	// fill the platform's servos
	static MaestroServoController maestro("/dev/ttyACM0");
	FlapronController flapron(maestro.getServoChannel(0), maestro.getServoChannel(4));
	dgx1_platform.tilt_servo	= maestro.getServoChannel(1);
	dgx1_platform.pitch_servo 	= flapron.get_ailron_servo();
	dgx1_platform.gas_servo 	= maestro.getServoChannel(2);
	dgx1_platform.yaw_servo 	= maestro.getServoChannel(3);

	// trim the servos
	maestro.getServoChannel(2)->trim_middle(52.5f);
	maestro.getServoChannel(2)->trim_range(95.0f);

	dgx1_platform.yaw_servo->set_data(50.0f);

	return dgx1_platform;
}

} //namespace autopilot
