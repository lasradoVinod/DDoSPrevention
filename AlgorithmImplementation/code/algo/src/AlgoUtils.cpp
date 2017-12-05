#include "AlgoUtils.h"
#include <sys/time.h>
#include <signal.h>
#include <map>
#include <iostream>
#include "log.hpp"

std::map<timer_t,callBackInfo> Timer::timerMap;

void timerHandler(int sig, siginfo_t *si, void *uc)
{
  Logger * log = Logger::getInstance();
  timer_t tidp;

  log->log("In timer handler",DEBUG_LEVEL);
  tidp = si->si_value.sival_ptr;

  std::map<timer_t,callBackInfo>::iterator it;
  it = Timer::timerMap.find(tidp);

  log->log("Calling function",DEBUG_LEVEL);
  (it->second).func((it->second).arg);
  log->log("Back in timer handler",DEBUG_LEVEL);
}

Timer::Timer()
{
}

timer_t * Timer::createTimer(uint32_t ms,callback_t function,void * object)
{
  callBackInfo tempInfo = {function,object};
  timer_t * timerID = new (timer_t);
  struct sigevent te;
  struct itimerspec its;
  struct sigaction sa;
  int sigNo = SIGRTMIN;

  /* Set up signal handler. */
  sa.sa_flags = SA_SIGINFO | SA_RESTART;
  sa.sa_sigaction = timerHandler;
  sigemptyset(&sa.sa_mask);
  if (sigaction(sigNo, &sa, NULL) == -1) {
      perror("sigaction");
  }

  /* Set and enable alarm */
  te.sigev_notify = SIGEV_SIGNAL;
  te.sigev_signo = sigNo;
  te.sigev_value.sival_ptr = timerID;
  timer_create(CLOCK_REALTIME, &te, timerID);

  its.it_interval.tv_sec = ms / 1000;
  its.it_interval.tv_nsec = (ms % 1000) * 1000000;
  its.it_value.tv_sec = ms / 1000;
  its.it_value.tv_nsec = (ms % 1000) * 1000000;
  timer_settime(*timerID, 0, &its, NULL);

  timerMap.insert(std::pair<timer_t,callBackInfo>(timerID,tempInfo));
  return timerID;
}

void reset_timer(timer_t * timer_id)
{
    struct itimerspec ts;
    ts.it_value.tv_sec = 0;
    ts.it_value.tv_nsec = 0;
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;
    if (timer_settime(*timer_id, 0, &ts, NULL) == -1){
            std::cerr << "errno code: " << errno;
            exit (0);
    }
}

void Timer::stopTimer(timer_t * ev)
{
  std::map<timer_t,callBackInfo>::iterator it;
  it = Timer::timerMap.find(ev);
  reset_timer(ev);
  timer_delete(*ev);
  timerMap.erase(it);
}

Timer::~Timer()
{
  /*TODO delete all timers from map*/
}
