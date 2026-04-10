#include "Timer.h"

/**
 * Initialize timer members
 */
Timer::Timer(void) : running(false),
                     durationMs(0),
                     startTimeTicks(0) {
}

/**
 * Set the duration (in milliseconds) from
 * when the timer start to when it expires
 *
 * @param[in] duration - The duration in milliseconds
 */
void Timer::setDurationMs(Ticks duration) {
  durationMs = duration;
}

/**
 * Get the duration (in milliseconds) from
 * when the timer start to when it expires
 *
 * @retval Duration in milliseconds
 */
Ticks Timer::getDurationMs(void) {
  return durationMs;
}

/**
 * Start the timer
 */
void Timer::start(void) {
  startTimeTicks = millis();
  running = true;
}

/**
 * Stop the timer
 */
void Timer::stop(void) {
  startTimeTicks = 0;
  running = false;
}

/**
 * Check if timer is running
 *
 * @retval true  - Timer is running
 * @retval false - Timer is stopped
 */
bool Timer::isRunning(void) {
  return running;
}

/**
 * Check if timer is finished
 *
 * @retval true  - The timer has finished
 * @retval false - The timer has not finished
 */
bool Timer::isDone(void) {
  if (running && elaspedTimeMs() >= durationMs)
    return true;
  return false;
}

/**
 * Get the elapsed time (in milliseconds) since the timer started.
 *
 * @retval Elapsed time in milliseconds.
 */
Ticks Timer::elaspedTimeMs(void) {
  if (running)
    return calculateElapsedTime(startTimeTicks, millis());

  return 0;
}

/**
 * Calculate the elapsed time (in milliseconds) since the timer started.
 *
 * @retval Elapsed time in milliseconds.
 */
Ticks Timer::calculateElapsedTime(Ticks startTimeTicks, Ticks endTimeTicks) {
  const Ticks MAX_TICKS = (Ticks)(-1);
  Ticks elapsedTimeMs = 0;

  if (endTimeTicks > startTimeTicks) {
    elapsedTimeMs = endTimeTicks - startTimeTicks;
  } else {
    elapsedTimeMs = endTimeTicks + (MAX_TICKS - startTimeTicks);
  }

  return elapsedTimeMs;
}