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
double get_curr_time() {
	timeval curr_tv;
	gettimeofday(&curr_tv, NULL);
	return (double)curr_tv.tv_sec + curr_tv.tv_usec/1000000.;
}

/**
 * Useful class for time mesurment.
 */
class Timer {
public:
	Timer() { reset(); }

	double passed() const {
		return get_curr_time() - m_start;
	}

	void reset() {
		m_start = get_curr_time();
	}

private:

	double m_start;

};


#endif /* TIME_H_ */
