/*Utility Functions*/
#ifndef ALGO_UTILS
#define ALGO_UTILS

#include <sys/time.h>
#include <event2/event.h>
#include <vector>
#include <thread>

/*Most generic callback that can be created.*/
typedef void * (*callback) (void *);

class Timer
{
private:
	uint8_t numTimers;
	struct event_base *base;
public:
	Timer();
	struct event * createTimer(uint32_t ms,callback function);
	void stopTimer(struct event * ev);
	~Timer();
};
#endif
