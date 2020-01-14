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
#include <fstream>

typedef std::chrono::steady_clock SteadyClock;

struct TimeIntervalInfo
{
    bool setTextInFormat(const std::string& textFormat);

    std::string& getInTextFormat() const;

    bool isEq(const TimeIntervalInfo& right) const;
    bool isInInterval(unsigned short hours, unsigned short minutes) const;

    unsigned short beginHours_ = 0;
    unsigned short beginMinutes_ = 0;
    unsigned short endHours_ = 0;
    unsigned short endMinutes_ = 0;

private:
    mutable std::string inTextBuffer_;
    void addTimeLikeValue(unsigned short val) const;
};

class NoticeForm : public nana::form
{
public:
    enum class Action
    {
        DebugTriggeredAction,
        TriggeredInvalidAction,
    };
public:
    NoticeForm(Action action, nana::window wd, const ::nana::size& sz = { 340, 200 }, const nana::appearance& apr = { true, true, false, false, false, false, false });
    ~NoticeForm();

private:
    void init_();


protected:
    Action action_;
    nana::place _place{ *this };
    nana::label wLabel;
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
    TimeIntervalForm(const TimeIntervalInfo& initTimeIntervalInfo, nana::window wd, const ::nana::size& sz = { 180, 120 }, const nana::appearance& apr = { true, true, false, false, false, false, false });
    ~TimeIntervalForm();

    const TimeIntervalInfo& getTimeIntervalInfo() const { return timeIntervalInfo_; }

private:
    void init_();

protected:
    struct HoursSpin : nana::spinbox
    {
        HoursSpin()
        {
            range(0, 23, 1);
            modifier("", " h");
        }
    };

    struct MinutesSpin : nana::spinbox
    {
        MinutesSpin()
        {
            range(0, 59, 1);
            modifier("", " m");
        }
    };

    nana::place place_{ *this };

    nana::label wLabBegin_;
    nana::label wLabEnd_;

    HoursSpin wSpinBeginHours_;
    MinutesSpin wSpinBeginMinutes_;
    HoursSpin wSpinEndHours_;
    MinutesSpin wSpinEndMinutes_;
    
    nana::button wButtAnyTime_;
    nana::button wButtSet_;

    TimeIntervalInfo timeIntervalInfo_;
    bool isTimeChanged_ = false;
};

class FaceFrom : public nana::form
{
    enum class EAction : size_t
    {
        No,
        Shutdown,
        Restart,
        Logoff,
        Hibernation
    };

    struct Config
    {
        Config();
        ~Config();

        void init();

        void save();

        EAction action_ = EAction::No;
        size_t inactive_ = 60; // min
        size_t warn_ = 120; // secs
        TimeIntervalInfo timeInterval_;
        bool checkMouseMovement_ = true;

    private:
        std::fstream fs_;
    };

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
    const TimeIntervalInfo& getTimeIntervalInfo() const { return config_.timeInterval_; }
    bool isSetTimeInterval() const;

private:
    void init_();

    void updateConfigState();
    void updateWarnSpinboxRange();
    void updateTimeIntervalCaption(const TimeIntervalInfo& timeIntervalInfo);

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
    nana::checkbox wCheckMouseMove_;
    nana::button wButtSave_;

    std::unique_ptr<TimeIntervalForm> timeIntervalForm_;
    std::unique_ptr<HistoryForm> logForm_;
    std::unique_ptr<WarnForm> warnForm_;
    std::unique_ptr<NoticeForm> noticeForm_;

    Config config_;
};


#endif