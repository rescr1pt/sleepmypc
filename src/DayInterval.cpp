#include "DayInterval.h"

const char* Days::getDayShort(unsigned char day)
{
    static const char weekDesc[7][4] = { "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun" };
    return weekDesc[day];
}
