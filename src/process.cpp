#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) {
    pid_ = pid;
}

int Process::Pid() {
    return pid_;
}

float Process::CpuUtilization() const {
    return cpu_;
}

string Process::Command() {
    return LinuxParser::Command(Pid());
}

string Process::Ram() {
    return LinuxParser::Ram(Pid());
}

string Process::User() {
    return LinuxParser::User(Pid());
}

long int Process::UpTime() {
    return LinuxParser::UpTime(Pid());
}

bool Process::operator<(Process const& a) const {
    return CpuUtilization() < a.CpuUtilization();
}

bool Process::operator>(Process const& a) const {
    return CpuUtilization() > a.CpuUtilization();
}

void Process::CpuUtilization(long actTicks, long sysTicks) {
    cpu_ = static_cast<float>(actTicks - cachActTicks) / (sysTicks - cachSysTicks);
    cachActTicks = actTicks;
    cachSysTicks = sysTicks;
    return;
}
