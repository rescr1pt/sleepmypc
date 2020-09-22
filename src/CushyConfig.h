/*
   CushyConfig header
   author: rescr1pt@.ya.ru
   Apache License 2.0
*/

#ifndef CUSHYCONFIG_H
#define CUSHYCONFIG_H

#include <string>
#include <fstream>
#include <unordered_map>

class CushyConfig
{
public:
    typedef std::runtime_error Exception;
    typedef unsigned int ParameterTypeNative;

protected:
    enum class EParameterTypes : ParameterTypeNative
    {
        UChar,
        Char,
        UShort,
        Short,
        UInt,
        Int,
        ULong,
        Long,
        ULongLong,
        LongLong,
/*
        UFloat,
        Float,
        UDouble,
        Double,*/
        String, // std::string
        Custom,
        
    };

    struct ICustomType
    {
        virtual ~ICustomType() {}
        virtual const std::string getAsString() const = 0;
        virtual void setAsString(const std::string& value) = 0;
    };

    struct Parameter
    {
        EParameterTypes type_;
        void* outValue_ = nullptr;
    };

public:
    explicit CushyConfig(const std::string& fileName);
    virtual ~CushyConfig();

    virtual void load();
    virtual void save();

    bool isInitialized() const noexcept { return isInitialized_; }

protected:
    void resetParameters();

    void addParameter(const std::string& name, unsigned char& outValue) { addParameter(name, EParameterTypes::UChar, &outValue); }
    void addParameter(const std::string& name, char& outValue) { addParameter(name, EParameterTypes::Char, &outValue); }
    void addParameter(const std::string& name, unsigned short& outValue) { addParameter(name, EParameterTypes::UShort, &outValue); }
    void addParameter(const std::string& name, short& outValue) { addParameter(name, EParameterTypes::Short, &outValue); }
    void addParameter(const std::string& name, unsigned int& outValue) { addParameter(name, EParameterTypes::UInt, &outValue); }
    void addParameter(const std::string& name, int& outValue) { addParameter(name, EParameterTypes::Int, &outValue); }
    void addParameter(const std::string& name, unsigned long& outValue) { addParameter(name, EParameterTypes::ULong, &outValue); }
    void addParameter(const std::string& name, long& outValue) { addParameter(name, EParameterTypes::Long, &outValue); }
    void addParameter(const std::string& name, unsigned long long& outValue) { addParameter(name, EParameterTypes::ULongLong, &outValue); }
    void addParameter(const std::string& name, long long & outValue) { addParameter(name, EParameterTypes::LongLong, &outValue); }
    void addParameter(const std::string& name, std::string& outValue) { addParameter(name, EParameterTypes::String, &outValue); }
    void addParameter(const std::string& name, ICustomType& outValue) { addParameter(name, EParameterTypes::Custom, &outValue); }

    void removeParameter(const std::string& name);

private:
    void addParameter(const std::string& name, EParameterTypes type, void* outValue);
    std::string toSave(void* data, CushyConfig::EParameterTypes type);

private:
    std::string fileName_;
    std::unordered_map<std::string, Parameter> parameters_;
    bool isInitialized_ = false;
    std::fstream fs_;
};

#endif