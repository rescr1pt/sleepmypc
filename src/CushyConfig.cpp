/*
   CushyConfig source
   author: rescr1pt@.ya.ru
   Apache License 2.0
*/

#include <algorithm>

#include "CushyConfig.h"

void ltrim(std::string& s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
}

void rtrim(std::string& s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

void trim(std::string& s)
{
    ltrim(s);
    rtrim(s);
}

template<typename T>
T& getOutType(void* value)
{
    return *((T*)value);
}

CushyConfig::CushyConfig(const std::string& fileName)
    : fileName_(fileName)
{

}

CushyConfig::~CushyConfig()
{
    fs_.close();
}

void CushyConfig::load()
{
    isInitialized_ = false;

    if (fs_.is_open()) {
        fs_.close();
    }

    fs_.open(fileName_, std::fstream::in);

    if (!fs_.is_open()) {
        throw Exception("Cannot configuration config file");
        return;
    }

    std::string line;
    unsigned loadedValueCount = 0;

    while (std::getline(fs_, line))
    {
        ltrim(line);

        // Skip empty lines
        if (line.empty()) {
            continue;
        }

        // Skip comments
        if (line[0] == '#') {
            continue;
        }

        size_t namePos = line.find('=');
        if (namePos == std::string::npos) {
            throw Exception("Incorrect configuration parameter definition in line: " + line);
        }

        if (namePos + 1 >= line.size()) {
            throw Exception("Incorrect configuration value definition in line: " + line);
        }

        std::string name = line.substr(0, namePos);
        rtrim(name);

        // Use line as value
        std::string& value = line;
        value.erase(0, namePos + 1);
        trim(value);

        auto foundIt = parameters_.find(name);
        if (foundIt == parameters_.end()) {
            throw Exception("Incorrect configuration parameter in line: " + line);
        }
        
        try
        {
            switch (foundIt->second.type_)
            {
                case EParameterTypes::UChar:
                {
                    auto& outVal = getOutType<unsigned char>(foundIt->second.outValue_);
                    outVal = static_cast<unsigned char>(std::stoul(value));
                    break;
                }
                case EParameterTypes::Char:
                {
                    auto& outVal = getOutType<char>(foundIt->second.outValue_);
                    outVal = std::stoi(value);
                    break;
                }
                case EParameterTypes::UShort:
                {
                    auto& outVal = getOutType<unsigned short>(foundIt->second.outValue_);
                    outVal = static_cast<unsigned short>(std::stoul(value));
                    break;
                }
                case EParameterTypes::Short:
                {
                    auto& outVal = getOutType<short>(foundIt->second.outValue_);
                    outVal = std::stoi(value);
                    break;
                }
                case EParameterTypes::UInt:
                {
                    auto& outVal = getOutType<unsigned int>(foundIt->second.outValue_);
                    outVal = std::stoul(value);
                    break;
                }
                case EParameterTypes::Int:
                {
                    auto& outVal = getOutType<int>(foundIt->second.outValue_);
                    outVal = std::stoi(value);
                    break;
                }
                case EParameterTypes::ULong:
                {
                    auto& outVal = getOutType<unsigned long>(foundIt->second.outValue_);
                    outVal = std::stoul(value);
                    break;
                }
                case EParameterTypes::Long:
                {
                    auto& outVal = getOutType<long>(foundIt->second.outValue_);
                    outVal = std::stol(value);
                    break;
                }
                case EParameterTypes::ULongLong:
                {
                    auto& outVal = getOutType<unsigned long long>(foundIt->second.outValue_);
                    outVal = std::stoull(value);
                    break;
                }
                case EParameterTypes::LongLong:
                {
                    auto& outVal = getOutType<long long>(foundIt->second.outValue_);
                    outVal = std::stoll(value);
                    break;
                }
                case EParameterTypes::String:
                {
                    auto& outVal = getOutType<std::string>(foundIt->second.outValue_);
                    outVal = value;
                    break;
                }
                case EParameterTypes::Custom:
                {
                    auto& outVal = getOutType<ICustomType>(foundIt->second.outValue_);
                    outVal.setAsString(value);
                    break;
                }
                default:
                {
                    throw Exception("Incorrect configuration parameter type in line: " + line);
                }
            }
        }
        catch (std::logic_error&) {
            throw Exception("Bad-cast on configuration parameter type in line: " + line);
        }

        ++loadedValueCount;
    }

    if (loadedValueCount != parameters_.size())
    {
        throw Exception("Not all parameters were found in configuration file");
    }

    isInitialized_ = true;

    fs_.close();
}

void CushyConfig::save()
{
    isInitialized_ = false;

    if (fs_.is_open()) {
        fs_.close();
    }

    fs_.open(fileName_, std::fstream::out | std::ios::trunc);

    if (!fs_.is_open()) {
        throw Exception("Unable to open configuration file");
    }

    for (auto& p : parameters_) {
        fs_ << p.first << '=' << toSave(p.second.outValue_, p.second.type_) << std::endl;
    }

    fs_.flush();
    fs_.close();

    isInitialized_ = true;
}

void CushyConfig::resetParameters()
{
    parameters_.clear();
}

void CushyConfig::addParameter(const std::string& name, EParameterTypes type, void* outValue)
{
    auto& parameter = parameters_[name];
    parameter.type_ = type;
    parameter.outValue_ = outValue;
}

void CushyConfig::removeParameter(const std::string& name)
{
    parameters_.erase(name);
}

std::string CushyConfig::toSave(void* data, EParameterTypes type)
{
    switch (type)
    {
        case EParameterTypes::UChar:
            return std::to_string(getOutType<unsigned char>(data));
        case EParameterTypes::Char:
            return std::to_string(getOutType<char>(data));
        case EParameterTypes::UShort:
            return std::to_string(getOutType<unsigned short>(data));
        case EParameterTypes::Short:
            return std::to_string(getOutType<short>(data));
        case EParameterTypes::UInt:
            return std::to_string(getOutType<unsigned int>(data));
        case EParameterTypes::Int:
            return std::to_string(getOutType<int>(data));
        case EParameterTypes::ULong:
            return std::to_string(getOutType<unsigned long>(data));
        case EParameterTypes::Long:
            return std::to_string(getOutType<long>(data));
        case EParameterTypes::ULongLong:
            return std::to_string(getOutType<unsigned long long>(data));
        case EParameterTypes::LongLong:
            return std::to_string(getOutType<long long>(data));
        case EParameterTypes::String:
            return getOutType<std::string>(data);
        case EParameterTypes::Custom:
            return getOutType<ICustomType>(data).getAsString();
        default:
            break;
    }

    throw Exception("Unsupported type: " + std::to_string((ParameterTypeNative)type) + " to save");
}
