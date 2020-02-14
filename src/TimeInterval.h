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
    bool isInInterval(unsigned char hours, unsigned char minutes) const;

    unsigned char beginHours_ = 0;
    unsigned char beginMinutes_ = 0;
    unsigned char endHours_ = 0;
    unsigned char endMinutes_ = 0;

private:
    mutable std::string inTextBuffer_;
    void addTimeLikeValue(unsigned char val) const;
};

#endif