#include "processor.h"
#include "linux_parser.h"

float Processor::Utilization() {
    float utilization = static_cast<float>(LinuxParser::ActiveJiffies() - cachActTicks) / 
        static_cast<float>(LinuxParser::ActiveJiffies() - cachActTicks + LinuxParser::IdleJiffies() - cachIdlTicks);
    cachActTicks = LinuxParser::ActiveJiffies();
    cachIdlTicks = LinuxParser::IdleJiffies();
    return utilization;
}