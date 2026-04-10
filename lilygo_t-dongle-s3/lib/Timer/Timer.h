#ifndef _TIMER_H_
#define _TIMER_H_

#include <Arduino.h>

typedef unsigned long Ticks;

class Timer {
 private:
  bool running;
  Ticks durationMs;
  Ticks startTimeTicks;
  Ticks calculateElapsedTime(Ticks startTimeTicks, Ticks endTimeTicks);

 public:
  Timer(void);
  void setDurationMs(Ticks duration);
  Ticks getDurationMs(void);
  void start(void);
  void stop(void);
  bool isRunning(void);
  bool isDone(void);
  Ticks elaspedTimeMs(void);
};

#endif