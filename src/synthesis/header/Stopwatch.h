#ifndef STOPWATCH_H
#define STOPWATCH_H

#include <chrono>

namespace Syft {

/**
 * \brief Stopwatch for timing executions.
 */
class Stopwatch {
 private:

  bool is_started_;
  std::chrono::high_resolution_clock::time_point start_time_;
  
 public:

  /**
   * \brief Create a stopwatch without starting it.
   */
  Stopwatch();
  
  /**
   * \brief Start the stopwatch.
   */
  void start();

  /**
   * \brief Stop the stopwatch.
   *
   * Throws an exception if stopwatch hasn't been started since the last time
   * it was stopped.
   *
   * \return The time elapsed in ms between starting and stopping the stopwatch.
   */
  std::chrono::milliseconds stop();

};

}

#endif // STOPWATCH_H
