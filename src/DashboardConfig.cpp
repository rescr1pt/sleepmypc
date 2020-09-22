#include "DashboardConfig.h"

static const std::string DASHBOARD_CONFIG_NAME = "sleepmypc_dashboard.ini";

DashboardConfig::DashboardConfig()
    : CushyConfig(DASHBOARD_CONFIG_NAME)
{
    addParameter("CHECK_MOUSE_MOVEMENT", (unsigned char&)checkMouseMovement_);
    addParameter("ACTIVE_DETAILS", activeDetails_.value_);
}

DashboardConfig::~DashboardConfig()
{
}
