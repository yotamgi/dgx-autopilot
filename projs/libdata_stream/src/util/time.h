/*
 * time.h
 *
 *  Created on: Aug 29, 2011
 *      Author: yotam
 */

#ifndef TIME_H_
#define TIME_H_

#include <sys/time.h>

/**
 * Returns the curr time in seconds in high resolution.
 * Returns the seconds since the epoch (or something...)
 */
inline double get_curr_time() {
	timeval curr_tv;
	gettimeofday(&curr_tv, NULL);
	return (double)curr_tv.tv_sec + curr_tv.tv_usec/1000000.;
}

/**
 * Useful class for time mesurment.
 */
class Timer {
public:
	Timer():m_paused(false) { reset(); }

	double passed() const {
		if (!m_paused) {
			return get_curr_time() - m_start;
		} else {
			return m_paused_time;
		}
	}

	void pause() {
		m_paused_time = passed();
		m_paused = true;
	}

	void cont() {
		m_paused = false;
		m_start += (passed() - m_paused_time);
	}

	void reset() {
		m_start = get_curr_time();
	}

private:

	double m_start;

	bool m_paused;
	double m_paused_time;

};


#endif /* TIME_H_ */
