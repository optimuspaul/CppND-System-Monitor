#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// DONE: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  long total_memory = 0, free_memory = 0;
  string line, label, number;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while (!stream.eof()) {
      std::istringstream linestream(line);
      std::getline(stream, line);
      linestream >> label >> number;
      if (label == "MemTotal:") {
        total_memory = std::stof(number);
      } else if (label == "MemFree:") {
        free_memory = std::stof(number);
      }
    }
  }
  return (float)(total_memory - free_memory) / (float)total_memory;
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() {
  string uptime;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  return std::stol(uptime);
}

// DONE: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  std::ifstream stream(kProcDirectory + kStatFilename);
  string label, line;
  long user, nice, syst, idle, wait, irq, soft, steal;
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream iss(line);
      iss >> label;
      if ("cpu" == label) {
        iss >> user >> nice >> syst >> idle >> wait >> irq >> soft >> steal;
        return user + nice + syst + idle + wait + irq + soft + steal;
      }
    }
  }
  return 0;
}

// DONE: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  string line, ignore;
  int counter = 14;
  long utime, stime, cutime, cstime;
  // we only care about the first line which has all the cpu usage info in it
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream iss(line);
    /* Skip first 13 items in line */
    while (--counter) {
      iss >> ignore;
    }
    iss >> utime >> stime >> cutime >> cstime;
    return utime + stime + cutime + cstime;
  }
  return 0;
}

// DONE: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  std::ifstream stream(kProcDirectory + kStatFilename);
  string label, line;
  long user, nice, syst, irq, soft, steal, ignore;
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream iss(line);
      iss >> label;
      if ("cpu" == label) {
        iss >> user >> nice >> syst >> ignore >> ignore >> irq >> soft >> steal;
        return user + nice + syst + irq + soft + steal;
      }
    }
  }
  return 0;
}

// DONE: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  std::ifstream stream(kProcDirectory + kStatFilename);
  string label, line;
  long idle, ignore;
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream iss(line);
      iss >> label;
      if ("cpu" == label) {
        iss >> ignore >> ignore >> ignore >> idle;
        return idle;
      }
    }
  }
  return 0;
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  std::ifstream stream(kProcDirectory + kStatFilename);
  string label, line;
  int processes;
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream iss(line);
      iss >> label;
      if ("processes" == label) {
        iss >> processes;
        return processes;
      }
    }
  }
  return 0;
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  std::ifstream stream(kProcDirectory + kStatFilename);
  string label, line;
  int processes;
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream iss(line);
      iss >> label;
      if ("procs_running" == label) {
        iss >> processes;
        return processes;
      }
    }
  }
  return 0;
}

// DONE: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  string label, line;
  if (stream.is_open()) {
    std::getline(stream, line);
    if (line.length() > 40) {
      line = line.substr(0, 37) + "...";
    }
    return line;
  }
  return "";
}

// DONE: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  string label, line, value;
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream iss(line);
      iss >> label;
      // using VmRSS instead of VmSize because it represents physical ram usage.
      if ("VmRSS:" == label) {
        iss >> value;
        return std::to_string(std::stoi(value) / 1024);
      }
    }
  }
  return "";
}

// DONE: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  string label, line, value;
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream iss(line);
      iss >> label;
      if ("Uid:" == label) {
        iss >> value;
        return value;
      }
    }
  }
  return "";
}

// DONE: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string puid = Uid(pid);
  std::ifstream stream(kPasswordPath);
  string name, line, uid;
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream iss(line);
      std::getline(iss, name, ':');
      std::getline(iss, uid, ':');
      std::getline(iss, uid, ':');

      if (uid == puid) {
        if (name.length() > 6) {
          name = name.substr(0, 6);
        }
        return name;
      }
    }
  }
  return puid;
}

// DONE: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  string line, ignore;
  int counter = 22;
  long starttime;
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream iss(line);
    /* Skip first 21 items in line */
    while (--counter) {
      iss >> ignore;
    }
    iss >> starttime;
    // std::cout << starttime;
    starttime = starttime / sysconf(_SC_CLK_TCK);
    return starttime;
  }
  return 0;
}
