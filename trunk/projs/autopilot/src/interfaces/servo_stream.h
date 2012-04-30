#ifndef SERVO_STREAM_H_
#define SERVO_STREAM_H_

#include <stream/data_push_stream.h>

namespace autopilot {

/**
 * This class is an interface for a servo class.
 * It enhances the normal DataPushStream<float> class and adds trim functions.
 * In addition, it supplies a nice helper function for trimming.
 */
class ServoStream : public stream::DataPushStream<float> {
public:
	ServoStream():  m_middle_trim(0.0f),
					m_range_trim(100.0f),
					m_direction(1.0f)
	{}

	/**
	 * Trims the servo middle place.
	 * @param middle the new middle percentage.
	 */
	virtual void trim_middle(float middle) { m_middle_trim = middle; }

	/**
	 * Trims the servo range.
	 * @param range the new servo range in percentage.
	 */
	virtual void trim_range(float range) { m_range_trim = range; }

	/**
	 * Sets the servo direction.
	 * @paran dir the new servo direction
	 */
	virtual void reverse(bool dir) { m_direction = (dir? 1.:-1.); }

protected:

	float trim(float untrimmed) {
		// apply the trims
		float trimmed = (untrimmed - m_middle_trim) * m_direction * (m_range_trim / 100.0f) +  m_middle_trim;

		// limit the data
		trimmed = std::min(100.0f, trimmed);
		trimmed = std::max(0.0f, trimmed);

		return trimmed;
	}

	float m_middle_trim;
	float m_range_trim;
	float m_direction;
};

}  // namespace autopilot

#endif /* SERVO_STREAM_H_ */
