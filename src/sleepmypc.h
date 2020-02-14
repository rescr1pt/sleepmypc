/*
   author: rescr1pt@.ya.ru
*/

#ifndef SLEEPMYPC
#define SLEEPMYPC

#include <nana/gui.hpp>
#include <nana/gui/place.hpp>
#include <nana/gui/widgets/checkbox.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/combox.hpp>
#include <nana/gui/widgets/spinbox.hpp>
#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/timer.hpp>

#include <chrono>

#include "Config.h"

typedef std::chrono::steady_clock SteadyClock;

class NoticeForm : public nana::form
{
public:
    enum class Action
    {
        DebugTriggeredAction,
        TriggeredInvalidAction,
    };
public:
    NoticeForm(const std::string& message, nana::window wd, const ::nana::size& sz = { 340, 200 }, const nana::appearance& apr = { true, true, false, false, false, false, false });
    ~NoticeForm();

private:
    void init_();


protected:
    std::string message_;
    nana::place _place{ *this };
    nana::label wLabel_;
};


class WarnForm : public nana::form
{
public:
    WarnForm(nana::window wd, const ::nana::size& sz = { 340, 140 }, const nana::appearance& apr = { true, true, false, false, false, false, false });
    ~WarnForm();

    void updateWarningCaption(size_t remainingTime, const std::string& option);

private:
    void init_();


protected:
    nana::place _place{ *this };
    nana::label wLabel;
};

class HistoryForm : public nana::form
{
public:
    HistoryForm(nana::window wd, const ::nana::size& sz = { 640, 480 }, const nana::appearance& apr = { true, true, false, false, false, false, false });
    virtual ~HistoryForm() override;

    static std::string& getFileName();

private:
    void init_();

private:
    void loadFile();

protected:
    nana::place place_{ *this };
    nana::textbox wBoxLog_{ *this };
    nana::button wButtClear_{ *this, "Clear" };
    nana::button wButtClose_{ *this, "Close" };

    std::fstream fs_;
};

class TimeIntervalForm : public nana::form
{
public:
    TimeIntervalForm(const TimeInterval& initTimeInterval, nana::window wd, const ::nana::size& sz = { 180, 160 }, const nana::appearance& apr = { true, true, false, false, false, false, false });
    ~TimeIntervalForm();

    const TimeInterval& getTimeInterval() const { return timeInterval; }

private:
    void init_();

protected:

    struct HoursComboBox : nana::combox
    {
        void _m_complete_creation() override;
    };

    struct MinutesComboBox : nana::combox
    {
        void _m_complete_creation() override;
    };

protected:

    nana::place place_{ *this };

    nana::label wLabBegin_;
    nana::label wLabEnd_;
    HoursComboBox wComboBeginHours_;
    MinutesComboBox wComboBeginMinutes_;
    HoursComboBox wComboEndHours_;
    MinutesComboBox wComboEndMinutes_;
    
    nana::button wButtAnyTime_;
    nana::button wButtSet_;

    TimeInterval timeInterval;
    bool isTimeChanged_ = false;
};

class FaceFrom : public nana::form
{
public:
    enum class ECurrentStatus
    {
        Disabled,
        Active,
        Inactive
    };

public:
    FaceFrom(nana::window wd, const ::nana::size& sz = { 260, 230 }, const nana::appearance& apr = { true, true, false, false, true /*min*/, false, false });

    ~FaceFrom();

    void showWarning(bool val);
    bool isShowWarning() const;

    void updateProgState(ECurrentStatus currentStatus);
    void updateWarningCaption(size_t remainingTime);

    void processTriggeredAction();

    size_t getConfInactive() const { return config_.inactive_; }
    size_t getConfWarn() const { return config_.warn_; }
    bool actionIsNone() const { return config_.action_ == EAction::No; }
    bool isCheckMouseMovement() const { return config_.checkMouseMovement_; }
    const TimeInterval& getTimeInterval() const { return config_.timeInterval_; }
    bool isAnyTimeInterval() const;

private:
    void init_();

    void updateConfigState();
    void updateWarnSpinboxRange();
    void updateTimeIntervalCaption(const TimeInterval& timeInterval);

protected:
    nana::place place_ { *this };
    nana::label wLabAction_;
    nana::combox wComboAction_;
    nana::label wLabIdleTimer_;
    nana::spinbox wSpinIdleTimer_;
    nana::label wLabWarnTimer_;
    nana::spinbox wSpinWarnTimer_;
    nana::label wLabTimeInterval_;
    nana::combox wComboTimeInterval_;
    nana::label wLabProgDesc_;
    nana::label wLabProgVal_;
    nana::button wButtOptions_;
    nana::button wButtHistory_;
    nana::label wLabCop_;
    nana::checkbox wCheckAutoStart_;
    nana::checkbox wCheckMouseMove_;
    nana::button wButtSave_;

    std::unique_ptr<TimeIntervalForm> timeIntervalForm_;
    std::unique_ptr<HistoryForm> logForm_;
    std::unique_ptr<WarnForm> warnForm_;

    Config config_;
};


#endif