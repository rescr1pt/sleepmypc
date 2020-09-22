/*
   author: rescr1pt@.ya.ru
*/

#ifndef SLEEPMYPC_DASHBOARD_CONFIG
#define SLEEPMYPC_DASHBOARD_CONFIG

#include <fstream>

#include "CushyConfig.h"
#include "DashboardDetails.h"

class DashboardConfig : public CushyConfig
{
public:
    DashboardConfig();
    ~DashboardConfig() override;

    // Read or modify parameters to save
public:
    bool checkMouseMovement_ = true;
    ActiveDetails activeDetails_;
};

#endif
