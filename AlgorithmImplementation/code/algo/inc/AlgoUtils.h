/*Utility Functions*/
#ifndef ALGO_UTILS
#define ALGO_UTILS

#include <sys/time.h>
#include <map>
#include <thread>
#include <functional>

typedef void (*callback_t) (void *);

typedef struct __callBackInfo
{
  callback_t func;
  void *arg;
}callBackInfo;


class Timer
{
private:

public:
  static std::map<timer_t,callBackInfo> timerMap;
  Timer();
	timer_t * createTimer(uint32_t ms,callback_t function, void * );
	void stopTimer(timer_t *);
	~Timer();
};
#endif
