/*
   author: rescr1pt@.ya.ru
*/

#ifndef SLEEPMYPC_TIMEINTERVAL
#define SLEEPMYPC_TIMEINTERVAL

#include <string>

struct TimeInterval
{
    bool setTextInFormat(const std::string& textFormat);

    std::string& getInTextFormat() const;

    bool isEq(const TimeInterval& right) const;
    bool isInInterval(unsigned short hours, unsigned short minutes) const;

    unsigned short beginHours_ = 0;
    unsigned short beginMinutes_ = 0;
    unsigned short endHours_ = 0;
    unsigned short endMinutes_ = 0;

private:
    mutable std::string inTextBuffer_;
    void addTimeLikeValue(unsigned short val) const;
};

#endif