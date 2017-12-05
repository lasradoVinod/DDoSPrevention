#include "AlgoUtils.h"

void eventLoop(event_base * base)
{
	event_base_dispatch(base);
}

Timer::Timer()
{
	base = event_base_new();
	numTimers = 0;
}

struct event * Timer::createTimer(uint32_t ms,event_callback_fn function,void * object)
{
	struct timeval tv = {0,ms*1000};
	struct event * ev = event_new(base,-1,EV_TIMEOUT|EV_PERSIST,(event_callback_fn)function,object);
	event_add(ev,&tv);
	if (numTimers == 0)
	{
		std::thread temp(eventLoop,base);
		temp.detach();
	}
	numTimers ++;
	return ev;
}

void Timer::stopTimer(struct event * ev)
{
	if (numTimers == 1)
		event_base_loopbreak(base);
	event_del(ev);
	numTimers --;
}

Timer::~Timer()
{
	event_base_loopbreak(base);
}
