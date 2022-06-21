#include "format.h"

#include <sstream>
#include <string>

using std::string;

// DONE: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  long minutes = seconds / 60;
  long hours = minutes / 60;
  std::stringstream ss;
  ss << hours << ":" << (minutes % 60) << ":" << (seconds % 60);
  return ss.str();
}