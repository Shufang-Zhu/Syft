#include "Stopwatch.h"

#include <stdexcept>

namespace Syft {

Stopwatch::Stopwatch() {
  is_started_ = false;
}
  
void Stopwatch::start() {
  is_started_ = true;
  start_time_ = std::chrono::high_resolution_clock::now();
}

std::chrono::milliseconds Stopwatch::stop() {
  auto stop_time = std::chrono::high_resolution_clock::now();

  if (!is_started_) {
    throw std::runtime_error("Stopwatch wasn't started before being stopped");
  }

  is_started_ = false;
  
  return std::chrono::duration_cast<std::chrono::milliseconds>(
      stop_time - start_time_);
}

}
