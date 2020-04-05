#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

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
  string os, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> kernel;
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

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  string line;
  string key;
  float value;
  float memTotal;
  float memFree;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()){
    while(std::getline(filestream, line)){
      std::istringstream linestream(line);
      while(linestream >> key >> value){
        if(key == "MemTotal:"){
          memTotal = value * 0.001;
        } else if(key == "MemFree:"){
          memFree = value * 0.001;
        }
      }
    }
  }
  return (memTotal - memFree) / memTotal;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  long uptime;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  return uptime; 
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies(); 
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { 
  string line;
  string value;
  long utime;
  long stime;
  long cutime;
  long cstime;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if(filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for(int n=0; n < 13; n++){
      linestream >> value;
    }
    linestream >> utime >> stime >> cutime >> cstime;
  }
  return (utime + stime + cutime + cstime) / sysconf(_SC_CLK_TCK); 
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  string line;
  string key;
  long user;
  long nice;
  long system;
  long idle;
  long iowait;
  long irq;
  long softirq;
  long steal;
  long active;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if(filestream.is_open()){
    while(std::getline(filestream, line)){
      std::istringstream linestream(line);
      while(linestream >> key >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal){
        if(key=="cpu"){
          active = user + nice + system + irq + softirq + steal;
        }
      }
    }
  }
  return active / sysconf(_SC_CLK_TCK); 
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  string line;
  string key;
  long user;
  long nice;
  long system;
  long idle;
  long iowait;
  long irq;
  long softirq;
  long steal;
  long idleTime;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if(filestream.is_open()){
    while(std::getline(filestream, line)){
      std::istringstream linestream(line);
      while(linestream >> key >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal){
        if(key == "cpu"){
          idleTime = idle + iowait;
        }
      }
    }
  }
  return idleTime = idle + iowait; 
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  string line;
  string key;
  string value;
  vector<string> values{};
  std::ifstream filestream(LinuxParser::kProcDirectory + LinuxParser::kStatFilename);
  if(filestream.is_open()){
    while(std::getline(filestream, line)){
      std::istringstream linestream(line);
      while(linestream >> key){
        if(key == "cpu"){
          linestream >> value;
          values.push_back(value);
        }
      }
    }
  }
  return values; 
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string line;
  string key;
  int total_proc;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if(filestream.is_open()){
    while(std::getline(filestream, line)){
      std::istringstream linestream(line);
      while(linestream >> key >> total_proc){
        if(key == "processes"){
          return total_proc;
        }
      }
    }
  }
  return total_proc; 
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string line;
  string key;
  int running_proc;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if(filestream.is_open()){
    while(std::getline(filestream, line)){
      std::istringstream linestream(line);
      while(linestream >> key >> running_proc){
        if(key == "procs_running"){
          return running_proc;
        }
      }
    }
  }
  return running_proc; 
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
  string line;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if(filestream.is_open()){
    std::getline(filestream, line);
  }
  return line; 
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
  string line;
  string key;
  float value;
  int ram;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if(filestream.is_open()){
    while(std::getline(filestream, line)){
      std::istringstream linestream(line);
      while(linestream >> key >> value){
        ram = value * 0.001;
      }
    }
  }
  return to_string(ram); 
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  string line;
  string key;
  string uid;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if(filestream.is_open()){
    while(std::getline(filestream, line)){
      std::istringstream linestream(line);
      while(linestream >> key >> uid){
        if(key == "Uid:"){
          return uid;
        }
      }
    }
  }
  return uid; 
}


// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  string line;
  string username;
  string pwd;
  string uid;
  std::ifstream filestream(kPasswordPath);
  if(filestream.is_open()){
    while(std::getline(filestream, line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while(linestream >> username >> pwd >> uid){
        if(uid == Uid(pid)){
          return username;
        }
      }
    }
  }
  return username; 
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
  string line;
  string value;
  long uptime;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if(filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for(int n = 0; n < 21; n++){
      linestream >> value;
    }
    linestream >> uptime;
  }
  return uptime / sysconf(_SC_CLK_TCK); 
}