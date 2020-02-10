/*
   author: rescr1pt@.ya.ru
*/

#ifndef SLEEPMYPC_CONFIG
#define SLEEPMYPC_CONFIG

#include <fstream>
#include <unordered_map>

#include "ActionEnums.h"
#include "TimeInterval.h"

class Config
{
public:
    Config();
    ~Config();

    void load();
    void save();

    // Read or modify parameters to save
public:
    EAction action_ = EAction::No;
    size_t inactive_ = 60; // min
    size_t warn_ = 120; // secs
    TimeInterval timeInterval_;
    bool checkMouseMovement_ = true;

private:

private:
    std::fstream fs_;
};

#endif
