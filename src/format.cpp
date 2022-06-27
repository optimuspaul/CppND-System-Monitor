#include "format.h"

#include <iomanip>
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
  ss << std::setw(2) << std::setfill('0') << hours << ":" << std::setw(2)
     << std::setfill('0') << (minutes % 60) << ":" << std::setw(2)
     << std::setfill('0') << (seconds % 60);
  return ss.str();
}