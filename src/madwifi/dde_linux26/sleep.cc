/**
 * \brief  Linux DDE Kit interface to timer session
 * \author Stefan Kalkowski
 * \date   2010-08-09
 */

#include <timer_session/connection.h>


static Timer::Connection *timer() {
	static Timer::Connection _timer;
	return &_timer;
}


extern "C" {

#include <dde_linux26/sleep.h>

	void dde_linux26_timer_session_msleep(unsigned ms)
	{
		timer()->msleep(ms);
	}

}

