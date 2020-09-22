/*
   author: rescr1pt@.ya.ru
*/

#ifndef SLEEPMYPC_DETAILS_FORM
#define SLEEPMYPC_DETAILS_FORM

#include <nana/gui.hpp>
#include <nana/gui/place.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/combox.hpp>
#include <nana/gui/widgets/spinbox.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/checkbox.hpp>

#include <chrono>

#include "DetailsConfig.h"

class DayIntervalForm : public nana::form
{
public:
    DayIntervalForm(const DayInterval& initDayInterval, nana::window wd);
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
    HoursComboBox beginHoursCombo_;
    MinutesComboBox beginMinutesCombo_;
    HoursComboBox endHoursCombo_;
    MinutesComboBox endMinutesCombo_;
    nana::checkbox weekCheckBoxes_[Days::DAY_MAX];
    
    nana::button setButton_;
    nana::button anyTimeButton_;
    nana::button anyDaysButton_;
    nana::button everTimeButton_;

    DayInterval dayInterval_;
    bool isTimeChanged_ = false;
};

class DetailsForm : public nana::form
{
public:
    DetailsForm(nana::window wd, DetailsConfig& config);
    ~DetailsForm();

private:
    void init_();

    void updateConfigState();
    void updateWarnSpinboxRange();
    void updateDayIntervalCaption(const DayInterval& dayInterval);

protected:
    DetailsConfig& config_;
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

    nana::button saveButton_;

    std::unique_ptr<DayIntervalForm> dayIntervalForm_;
};


#endif