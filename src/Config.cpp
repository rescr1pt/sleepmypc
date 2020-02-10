#include <iostream>
#include <algorithm>
#include <set>

#include "Config.h"

static const std::string CONFIG_FILE_NAME = "sleepmypc.ini";

enum class eConfigParameters : unsigned
{
    Action,
    Inactive,
    Warn,
    Interval,
    CheckMouseMovement,
    Max
};

static const std::string CONFIG_STRINGS[static_cast<size_t>(eConfigParameters::Max)] =
{ 
    "ACTION", 
    "INACTIVE", 
    "WARN", 
    "INTERVAL", 
    "CHECK_MOUSE_MOVEMENT" 
};

// trim from start (in place)
static inline void ltrim(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string& s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string& s) {
    ltrim(s);
    rtrim(s);
}

Config::Config()
{

}

Config::~Config()
{
    fs_.close();
}

void Config::load()
{
    fs_.open(CONFIG_FILE_NAME, std::fstream::in);

    if (fs_.is_open()) {
        std::string line;

        std::set<unsigned> readedParameters;
        for (unsigned i = 0; i < (unsigned)eConfigParameters::Max; ++i) {
            readedParameters.insert(i);
        }

        while (std::getline(fs_, line))
        {
            size_t namePos = line.find('=');
            if (namePos == std::string::npos) {
                throw ConfigException("Incorrect parameter definition in line: " + line);
            }

            if (namePos + 1 >= line.size()) {
                throw ConfigException("Incorrect value definition in line: " + line);
            }

            std::string name = line.substr(0, namePos);
            trim(name);

            // Use line as value
            std::string& value = line;
            value.erase(0, namePos + 1);
            trim(value);

            for (auto itIndex = readedParameters.begin(); itIndex != readedParameters.end(); ++itIndex) {
                if (CONFIG_STRINGS[*itIndex] != name) {
                    continue;
                }

                switch ((eConfigParameters)*itIndex)
                {
                    case eConfigParameters::Action:
                    {
                        action_ = (EAction)std::stoull(value);
                        break;
                    }
                    case eConfigParameters::Inactive:
                    {
                        inactive_ = std::stoull(value);
                        break;
                    }
                    case eConfigParameters::Warn:
                    {
                        warn_ = std::stoull(value);
                        break;
                    }
                    case eConfigParameters::Interval:
                    {
                        timeInterval_.setTextInFormat(value);
                        break;
                    }
                    case eConfigParameters::CheckMouseMovement:
                    {
                        checkMouseMovement_ = std::stoul(value) != 0;
                        break;
                    }
                    default:
                    {
                        throw ConfigException("Incorrect config parameter in line: " + line);
                    }
                }

                readedParameters.erase(itIndex);
                break;
            }
        }

        if (!readedParameters.empty())
        {
            throw ConfigException("Not all parameters were found in confg file");
        }
    }

    fs_.close();
}

void Config::save()
{
    if (fs_.is_open()) {
        fs_.close();
    }

    fs_.open(CONFIG_FILE_NAME, std::fstream::out | std::ios::trunc);

    if (!fs_.is_open()) {
        throw ConfigException("Unable to open configuration file");
    }

    fs_ << CONFIG_STRINGS[(unsigned)eConfigParameters::Action] << '=' << (size_t)action_ << std::endl
        << CONFIG_STRINGS[(unsigned)eConfigParameters::Inactive] << '=' << inactive_ << std::endl
        << CONFIG_STRINGS[(unsigned)eConfigParameters::Warn] << '=' << warn_ << std::endl
        << CONFIG_STRINGS[(unsigned)eConfigParameters::Interval] << '=' << timeInterval_.getInTextFormat() << std::endl
        << CONFIG_STRINGS[(unsigned)eConfigParameters::CheckMouseMovement] << '=' << checkMouseMovement_ << std::endl;

    fs_.flush();
    fs_.close();
}

