#include "TimeInterval.h"


bool TimeInterval::setTextInFormat(const std::string& textFormat)
{
    // hh:mm - hh:mm
    if (textFormat.size() != 13) {
        return false;
    }

    beginHours_ = (unsigned char)std::stoul(textFormat.substr(0, 2));
    beginMinutes_ = (unsigned char)std::stoul(textFormat.substr(3, 2));
    endHours_ = (unsigned char)std::stoul(textFormat.substr(8, 2));
    endMinutes_ = (unsigned char)std::stoul(textFormat.substr(11, 2));

    return true;
}

std::string& TimeInterval::getInTextFormat() const
{
    inTextBuffer_.clear();
    addTimeLikeValue(beginHours_);
    inTextBuffer_ += ':';
    addTimeLikeValue(beginMinutes_);
    inTextBuffer_ += " - ";

    addTimeLikeValue(endHours_);
    inTextBuffer_ += ':';
    addTimeLikeValue(endMinutes_);
    return inTextBuffer_;
}

bool TimeInterval::isEq(const TimeInterval& right) const
{
    return beginHours_ == right.beginHours_ && beginMinutes_ == right.beginMinutes_ && endHours_ == right.endHours_ && endMinutes_ == right.endMinutes_;
}

bool TimeInterval::isInInterval(unsigned char hours, unsigned char minutes) const
{
    const float currentD = hours + (0.01f * minutes);
    const float beginD = beginHours_ + (0.01f * beginMinutes_);
    const float endD = endHours_ + (0.01f * endMinutes_);

    if (currentD >= beginD && currentD < endD) {
        return true;
    }
    else if (beginD > endD && (currentD >= beginD || currentD < endD)) {
        return true;
    }

    return false;
}

void TimeInterval::addTimeLikeValue(unsigned char val) const
{
    if (val < 10) {
        inTextBuffer_ += '0';
    }

    inTextBuffer_ += std::to_string(val);
}


