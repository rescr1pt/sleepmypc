/*
   author: rescr1pt@.ya.ru
*/

#ifndef SLEEPMYPC_DAYINTERVAL
#define SLEEPMYPC_DAYINTERVAL

#include "TimeInterval.h"

struct Days
{
    enum : unsigned char
    {
        DAY_NONE = 0,
        DAY_MON = 1 << 0,
        DAY_TUE = 1 << 1,
        DAY_WED = 1 << 2,
        DAY_THU = 1 << 3,
        DAY_FRI = 1 << 4,
        DAY_SAT = 1 << 5,
        DAY_SUN = 1 << 6,
        DAY_ALL = 127
    };

    enum
    {
        DAY_MAX = 7
    };

    static const char* getDayShort(unsigned char day);
    static const char* getDayFull(unsigned char day);
};

class DayInterval
{
public:
    bool isEq(const DayInterval& right) const { return days_ == right.days_ && time_.isEq(right.time_); }

    // By flags
    inline void enableDays(unsigned char daysFlag) { days_ = ((days_) | daysFlag); }
    inline bool hasDays(unsigned char daysFlag) const { return ((days_) & (daysFlag)) != 0; }
    inline void disableDays(unsigned char daysFlag) { days_ = ((days_) & ~daysFlag); }
    
    // By index
    inline bool hasDay(unsigned char dayIndex) const { return hasDays(1 << dayIndex); }
    inline void enableDay(unsigned char dayIndex) { enableDays(1 << dayIndex); }
    inline void disableDay(unsigned char dayIndex) { disableDays(1 << dayIndex); }

    inline void enableAllDays() { days_ = Days::DAY_ALL; }
    inline bool isEnabledAllDays() const { return days_ == Days::DAY_ALL; }
    
    inline void disableAllDays() { days_ = Days::DAY_NONE; }
    inline bool isDisabledAllDays() const { return days_ == Days::DAY_NONE; }

public:
    TimeInterval time_;
    unsigned char days_ = Days::DAY_ALL;
};

#endif