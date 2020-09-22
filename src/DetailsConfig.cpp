#include <iostream>
#include <set>

#include "DetailsConfig.h"



std::string getFileName(size_t number)
{
    std::string fileName = "sleepmypc_detail_" + std::to_string(number);
    fileName += ".ini";
    return move(fileName);
}

DetailsConfig::DetailsConfig(size_t number)
    : CushyConfig(getFileName(number))
    , timeIntervalType_(dayInterval_.time_)
{
    addParameter("ACTION", (unsigned&)action_);
    addParameter("INACTIVE", inactive_);
    addParameter("WARN", warn_);
    addParameter("TIME_INTERVAL", timeIntervalType_);
    addParameter("DAYS", dayInterval_.days_);
}

DetailsConfig::~DetailsConfig()
{
}

