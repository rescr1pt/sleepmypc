/*
   author: rescr1pt@.ya.ru
*/

#ifndef SLEEPMYPC_DETAILS_CONFIG
#define SLEEPMYPC_DETAILS_CONFIG

#include <fstream>

#include "CushyConfig.h"
#include "ActionEnums.h"
#include "DayInterval.h"

class DetailsConfig : public CushyConfig
{
public:
    DetailsConfig(size_t number);
    ~DetailsConfig() override;

    // Read or modify parameters to save
public:
    EAction action_ = EAction::No;
    size_t inactive_ = 60; // min
    size_t warn_ = 120; // secs
    DayInterval dayInterval_;

private:
    struct TimeIntervalType : CushyConfig::ICustomType
    {
        TimeIntervalType(TimeInterval& timeInterval) : timeInterval_(timeInterval) {}

        const std::string getAsString() const override { return timeInterval_.getInTextFormat(); }
        void setAsString(const std::string& value) override { timeInterval_.setTextInFormat(value); }

        TimeInterval& timeInterval_;
    };

private:
    TimeIntervalType timeIntervalType_;
};

#endif
