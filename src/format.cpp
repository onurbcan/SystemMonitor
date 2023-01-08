#include <string>
#include <sstream>
#include <iomanip>
#include "format.h"

using std::string;

string Format::ElapsedTime(long seconds) {
    long sec = 1; // 1 second is chosen as unit
    long min = 60 * sec; // 1 minute in seconds
    long hour = 60 * min; // 1 hour in seconds
    std::stringstream timeFormat;
    timeFormat << std::setfill('0') << std::setw(2) << seconds / hour << ":"
        << std::setfill('0') << std::setw(2) << (seconds / min) % min << ":"
        << std::setfill('0') << std::setw(2) << seconds % min;
    return timeFormat.str();
}