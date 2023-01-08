#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>

#include "linux_parser.h"

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
  string os, host, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> host >> kernel;
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
        pids.emplace_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() {
  std::string line{""}, key{""}, value{""}, memTotal{""}, memFree{""}, buffers{""};
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if(stream.is_open()) {
    while(std::getline(stream, line)) {
      std::istringstream linestream(line);
      while(linestream >> key >> value) {
        if(key == "MemTotal:")
          memTotal = value;
        if(key == "MemFree:")
          memFree = value;
        if(key == "Buffers:")
          buffers = value;
      }
    }
  }
  return 1.0 - (std::stof(memFree) / (std::stof(memTotal) - std::stof(buffers)));
}

long LinuxParser::UpTime() {
  std::string line{""}, value{""}, value2{""};
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if(stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> value;
  }
  return std::stol(value);
}

long LinuxParser::Jiffies() {
  return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies();
}

long LinuxParser::ActiveJiffies(int pid) {
  std::string line{""}, value{""};
  std::vector<string> allValue;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if(stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while(linestream >> value) {
      allValue.emplace_back(value);
    }
  }
  long upTimeVal{0};  
  if(allValue.size() >= 21)
    upTimeVal = std::stol(allValue[13]) + std::stol(allValue[14]) + std::stol(allValue[15]) + std::stol(allValue[16]);
  return upTimeVal;
}

long LinuxParser::ActiveJiffies() {
  return std::stol(CpuUtilization()[CPUStates::kUser_]) +
    std::stol(CpuUtilization()[CPUStates::kNice_]) +
    std::stol(CpuUtilization()[CPUStates::kSystem_]) +
    std::stol(CpuUtilization()[CPUStates::kIRQ_]) +
    std::stol(CpuUtilization()[CPUStates::kSoftIRQ_]) +
    std::stol(CpuUtilization()[CPUStates::kSteal_]) +
    std::stol(CpuUtilization()[CPUStates::kGuest_]) +
    std::stol(CpuUtilization()[CPUStates::kGuestNice_]);
}

long LinuxParser::IdleJiffies() {
  return std::stol(CpuUtilization()[CPUStates::kIdle_]) +
    std::stol(CpuUtilization()[CPUStates::kIOwait_]);
}

vector<string> LinuxParser::CpuUtilization() {
  std::string line{""}, value{""};
  std::vector<string> allValue;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if(stream.is_open()) {
    while(std::getline(stream, line)){
      std::istringstream linestream(line);
      while(linestream >> value) {
        if(value == "cpu") {
          while(linestream >> value)
            allValue.emplace_back(value);
          return allValue;
        }
      }
    }
  }
  return allValue;
}

int LinuxParser::TotalProcesses() {
  std::string line{""}, key{""}, value{""};
  std::ifstream stream(kProcDirectory + kStatFilename);
  if(stream.is_open()) {
    while(std::getline(stream, line)) {
      std::istringstream linestream(line);
      while(linestream >> key >> value) {
        if(key == "processes")
          return std::stoi(value);
      }
    }
  }
  return std::stoi(value);
}

int LinuxParser::RunningProcesses() {
  std::string line{""}, key{""}, value{""};
  std::ifstream stream(kProcDirectory + kStatFilename);
  if(stream.is_open()) {
    while(std::getline(stream, line)) {
      std::istringstream linestream(line);
      while(linestream >> key >> value) {
        if(key == "procs_running")
          return std::stoi(value);
      }
    }
  }
  return std::stoi(value);
}

string LinuxParser::Command(int pid) {
  std::string line{""};
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if(stream.is_open())
    std::getline(stream, line);
  line = line.substr(0, line.find(' '));
  return line;
}

string LinuxParser::Ram(int pid) {
  std::string value{""};
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if(stream.is_open()) {
    while(stream >> value) {
      // I changed VmSize to VmRSS after a suggestion in the 1st review,
      //to get the exact physical memory being used as part of Physical RAM.
      if(value == "VmRSS:") {
        if(stream >> value) 
          return std::to_string(stoi(value) / 1024);
      }
    }
  }
  return std::to_string(stoi(value) / 1024);
}

string LinuxParser::Uid(int pid) {
  std::string value{""};
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if(stream.is_open()) {
    while(stream >> value) {
      if(value == "Uid:") {
        if(stream >> value)
          return value;
      }
    }
  }
  return value;
}

string LinuxParser::User(int pid) {
  std::string line{""}, user{""}, value{""};
  std::ifstream stream(kPasswordPath);
  if(stream.is_open()) {
    while(std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::replace(line.begin(), line.end(), 'x', ' ');
      std::istringstream linestream(line);
      while(linestream >> user >> value) {
        if(value == Uid(pid)) {
          return user;
        }
      }
    }
  }
  return user;
}

long LinuxParser::UpTime(int pid) {
  std::string line{""}, value{""};
  std::vector<string> allValue;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if(stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while(linestream >> value)
      allValue.emplace_back(value);
  }
  return UpTime() - (std::stol(allValue[21]) / sysconf(_SC_CLK_TCK));
}
