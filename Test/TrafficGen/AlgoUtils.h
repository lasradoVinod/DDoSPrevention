/*Utility Functions*/
#ifndef ALGO_UTILS
#define ALGO_UTILS

#include <sys/time.h>
#include <event2/event.h>
#include <vector>
#include <thread>
#include <functional>

class Timer
{
private:
	uint8_t numTimers;
	struct event_base *base;
public:
	Timer();
	struct event * createTimer(uint32_t ms,event_callback_fn function, void * );
	void stopTimer(struct event * ev);
	~Timer();
};
#endif
