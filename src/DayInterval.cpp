#include "DayInterval.h"

const char* Days::getDayShort(unsigned char day)
{
    static const char weekDesc[DAY_MAX][4] = { "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun" };
    return weekDesc[day];
}

const char* Days::getDayFull(unsigned char day)
{
    static const std::string weeks[DAY_MAX] = { "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday" };
    return weeks[day].c_str();
}
