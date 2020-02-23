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

class DayIntervalForm : public nana::form
{
public:
    DayIntervalForm(const DayInterval& initDayInterval, nana::window wd, const ::nana::size& sz = { 220, 200 }, const nana::appearance& apr = { true, true, false, false, false, false, false });
    ~DayIntervalForm();

    const DayInterval& getDayInterval() const { return dayInterval_; }

private:
    void init_();
    void eventHandler();
    void enableAllDays();
    void enableAllTimes();
    void updateTimes();

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

    nana::label intervalSeparatorLabel_;
    HoursComboBox wComboBeginHours_;
    MinutesComboBox wComboBeginMinutes_;
    HoursComboBox wComboEndHours_;
    MinutesComboBox wComboEndMinutes_;
    nana::checkbox weekCheckBoxes_[Days::DAY_MAX];
    
    nana::button wButtSet_;
    nana::button anyTimeButton_;
    nana::button anyDaysButton_;
    nana::button everTimeButton_;

    DayInterval dayInterval_;
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
    FaceFrom(nana::window wd, const ::nana::size& sz = { 250, 280 }, const nana::appearance& apr = { true, true, false, false, true /*min*/, false, false });

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
    const DayInterval& getDayInterval() const { return config_.dayInterval_; }
    bool isAnyDayInterval() const;

private:
    void init_();

    void updateConfigState();
    void updateWarnSpinboxRange();
    void updateDayIntervalCaption(const DayInterval& dayInterval);

protected:
    nana::place place_ { *this };

    /// Execute
    nana::label executeLab_;
    nana::label actionLab_;
    nana::combox actionCombo_;
    nana::label idleTimerLab_;
    nana::spinbox idleTimerSpin_;
    nana::label warnTimerLab_;
    nana::spinbox warnTimerSpin_;

    /// Period
    nana::label dayIntervalLab_;
    nana::combox dayIntervalCombo_;

    /// Additional options
    nana::label additionalOptionsLab_;
    nana::button mouseMoveButton_;

    nana::label progValueLab_;
    nana::button historyButton_;
    nana::button saveButton_;

    std::unique_ptr<DayIntervalForm> dayIntervalForm_;
    std::unique_ptr<HistoryForm> logForm_;
    std::unique_ptr<WarnForm> warnForm_;

    Config config_;
};


#endif