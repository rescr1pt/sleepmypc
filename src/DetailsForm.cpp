/*
   author: rescr1pt@.ya.ru
*/

#include "DetailsForm.h"
#include <ctime>

#include "FormTools.h"

DayIntervalForm::DayIntervalForm(const DayInterval& initDayInterval, nana::window wd)
    : dayInterval_(initDayInterval)
    , nana::form(wd, { 220, 200 }, { true, true, false, false, false, false, false })
{
    init_();
}

DayIntervalForm::~DayIntervalForm()
{

}

void DayIntervalForm::init_()
{
    caption("Day interval selector");

    static const nana::paint::font labelsFont("", 11, { 400 });

    bgcolor(nana::colors::white_smoke);

    intervalSeparatorLabel_.create(*this);
    intervalSeparatorLabel_.typeface(labelsFont);
    intervalSeparatorLabel_.caption(" - ");
    intervalSeparatorLabel_.transparent(true);
    intervalSeparatorLabel_.text_align(static_cast<nana::align>(1), static_cast<nana::align_v>(1));

    beginHoursCombo_.create(*this);
    beginMinutesCombo_.create(*this);
    endHoursCombo_.create(*this);
    endMinutesCombo_.create(*this);

    for (unsigned char i = 0; i < Days::DAY_MAX; ++i) {
        auto& day = weekCheckBoxes_[i];
        day.create(*this);
        day.caption(Days::getDayShort(i));
        day.transparent(true);
    }

    setButton_.create(*this);
    setButton_.caption("Set");
    setButton_.bgcolor(nana::colors::light_green);

    anyTimeButton_.create(*this);
    anyTimeButton_.caption("Any time");

    anyDaysButton_.create(*this);
    anyDaysButton_.caption("Any days");

    everTimeButton_.create(*this);
    everTimeButton_.caption("Ever time");


    // Init values
    for (unsigned char i = 0; i < Days::DAY_MAX; ++i) {
        weekCheckBoxes_[i].check(dayInterval_.hasDay(i));
    }
    beginHoursCombo_.option(dayInterval_.time_.beginHours_);
    beginMinutesCombo_.option(dayInterval_.time_.beginMinutes_);
    endHoursCombo_.option(dayInterval_.time_.endHours_);
    endMinutesCombo_.option(dayInterval_.time_.endMinutes_);

    updateTimes();

    eventHandler();

    /// Collate
    place_.div(""
        "margin=8 vert"
        "< weight=15% <intervalBegin> <intervalSeparator vert weight=20> <intervalEnd> >"
        "<weight=8%> "
        "< weight=15% <week3> >"
        "< weight=15% <week4> >"
        "< <anyTime> <anyDays> <everTime> >"
        "<weight=10%> "
        "< <vert weight=50 > <setTime> >"
        "");

    place_["intervalBegin"] << beginHoursCombo_ << beginMinutesCombo_;
    place_["intervalSeparator"] << intervalSeparatorLabel_;
    place_["intervalEnd"] << endHoursCombo_ << endMinutesCombo_;
    place_["week3"] << weekCheckBoxes_[0] << weekCheckBoxes_[1] << weekCheckBoxes_[2];
    place_["week4"] << weekCheckBoxes_[3] << weekCheckBoxes_[4] << weekCheckBoxes_[5] << weekCheckBoxes_[6];
    place_["setTime"] << setButton_;
    place_["anyTime"] << anyTimeButton_;
    place_["anyDays"] << anyDaysButton_;
    place_["everTime"] << everTimeButton_;

    place_.collocate();

}

void DayIntervalForm::eventHandler()
{
    setButton_.events().click([&]()
    {
        dayInterval_.disableAllDays();

        for (unsigned char i = 0; i < Days::DAY_MAX; ++i) {
            if (weekCheckBoxes_[i].checked()) {
                dayInterval_.enableDay(i);
            }
        }

        dayInterval_.time_.beginHours_ = (unsigned char)beginHoursCombo_.option();
        dayInterval_.time_.beginMinutes_ = (unsigned char)beginMinutesCombo_.option();
        dayInterval_.time_.endHours_ = (unsigned char)endHoursCombo_.option();
        dayInterval_.time_.endMinutes_ = (unsigned char)endMinutesCombo_.option();

        close();
    });

    beginHoursCombo_.events().selected([this]() { updateTimes(); });
    beginMinutesCombo_.events().selected([this]() { updateTimes(); });
    endHoursCombo_.events().selected([this]() { updateTimes(); });
    endMinutesCombo_.events().selected([this]() { updateTimes(); });

    // Prevent all days off
    for (unsigned char checkI = 0; checkI < Days::DAY_MAX; ++checkI) {
        weekCheckBoxes_[checkI].events().checked([this, checkI]()
        {
            if (!weekCheckBoxes_[checkI].checked()) {
                for (unsigned char otherI = 0; otherI < Days::DAY_MAX; ++otherI) {
                    if (checkI == otherI) {
                        continue;
                    }
                    if (weekCheckBoxes_[otherI].checked()) {
                        return;
                    }
                }

                weekCheckBoxes_[checkI].check(true);
            }
        });
    }

    anyTimeButton_.events().click([this]()
    {
        enableAllTimes();
        updateTimes();
    });

    anyDaysButton_.events().click([this]()
    {
        enableAllDays();
    });

    everTimeButton_.events().click([this]()
    {
        enableAllTimes();
        enableAllDays();

        updateTimes();
    });
}

void DayIntervalForm::enableAllDays()
{
    for (unsigned char i = 0; i < Days::DAY_MAX; ++i) {
        weekCheckBoxes_[i].check(true);
    }
}

void DayIntervalForm::enableAllTimes()
{
    beginHoursCombo_.option(0);
    beginMinutesCombo_.option(0);
    endHoursCombo_.option(0);
    endMinutesCombo_.option(0);
}

void DayIntervalForm::updateTimes()
{
    static const nana::color defColor;

    if (beginHoursCombo_.option() == 0
        && beginMinutesCombo_.option() == 0
        && endHoursCombo_.option() == 0
        && endMinutesCombo_.option() == 0) {
        beginHoursCombo_.fgcolor(nana::colors::light_gray);
        beginMinutesCombo_.fgcolor(nana::colors::light_gray);
        endHoursCombo_.fgcolor(nana::colors::light_gray);
        endMinutesCombo_.fgcolor(nana::colors::light_gray);
    }
    else {  
        beginHoursCombo_.fgcolor(defColor);
        beginMinutesCombo_.fgcolor(defColor);
        endHoursCombo_.fgcolor(defColor);
        endMinutesCombo_.fgcolor(defColor);
    }
}

DetailsForm::DetailsForm(nana::window wd, DetailsConfig& config)
    : nana::form(wd, { 250, 180 }, { true, true, false, false, true /*min*/, false, false })
    , config_(config)
{
    init_();
}

DetailsForm::~DetailsForm()
{
    //<*dtor

    //*>
}

void DetailsForm::init_()
{
    formtools::initIco(*this);

    static const nana::paint::font descriptionsFont("", 12, { 400, true, true, false });
    static const nana::paint::font definitionFont("", 11, { 400, false, false, false });
    static const nana::paint::font buttonFont("", 12, { 400, false, false, false });

    caption("sleep my PC - config");
    bgcolor(nana::colors::white_smoke);

    executeLab_.create(*this);
    executeLab_.caption("Execute action");
    executeLab_.typeface(descriptionsFont);
    executeLab_.transparent(true);
    executeLab_.text_align(static_cast<nana::align>(1), static_cast<nana::align_v>(1));

    actionLab_.create(*this);
    actionLab_.caption("Action type");
    actionLab_.transparent(true);
    actionLab_.typeface(definitionFont);
    actionLab_.text_align(static_cast<nana::align>(1), static_cast<nana::align_v>(1));

    actionCombo_.create(*this);
    actionCombo_.bgcolor(nana::color(255, 255, 155));
    actionCombo_.push_back(getActionName(EAction::No));
    actionCombo_.push_back(getActionName(EAction::Shutdown));
    actionCombo_.push_back(getActionName(EAction::Restart));
    actionCombo_.push_back(getActionName(EAction::Logoff));
    actionCombo_.push_back(getActionName(EAction::Hibernation));
    actionCombo_.option((size_t)EAction::No);
    actionCombo_.events().selected([&]()
    {
        updateConfigState();
    });

    idleTimerLab_.create(*this);
    idleTimerLab_.caption("When inactive");
    idleTimerLab_.typeface(definitionFont);
    idleTimerLab_.transparent(true);
    idleTimerLab_.text_align(static_cast<nana::align>(1), static_cast<nana::align_v>(1));

    idleTimerSpin_.create(*this);
    idleTimerSpin_.modifier("", " min");
    idleTimerSpin_.range(1, 460800, 1); // 320 months 
    idleTimerSpin_.events().text_changed([&]()
    {
        updateWarnSpinboxRange();
        updateConfigState();
    });

    warnTimerLab_.create(*this);
    warnTimerLab_.typeface(definitionFont);
    warnTimerLab_.caption("Warn when left");
    warnTimerLab_.transparent(true);
    warnTimerLab_.text_align(static_cast<nana::align>(1), static_cast<nana::align_v>(1));

    warnTimerSpin_.create(*this);
    warnTimerSpin_.modifier("", " secs");
    warnTimerSpin_.range(1, ((idleTimerSpin_.range_int().second * 60) - 1), 1); // like idle timer -1
    warnTimerSpin_.events().text_changed([&]()
    {
        updateConfigState();
    });

    dayIntervalLab_.create(*this);
    dayIntervalLab_.typeface(definitionFont);
    dayIntervalLab_.caption("In period");
    dayIntervalLab_.transparent(true);
    dayIntervalLab_.text_align(static_cast<nana::align>(1), static_cast<nana::align_v>(1));

    dayIntervalCombo_.create(*this);
    dayIntervalCombo_.bgcolor(nana::color(255, 255, 155));
    dayIntervalCombo_.events().click([&]()
    {
        if (dayIntervalForm_) {
            dayIntervalForm_ = std::make_unique<DayIntervalForm>(dayIntervalForm_->getDayInterval() ,*this);
        }
        else {
            dayIntervalForm_ = std::make_unique<DayIntervalForm>(config_.dayInterval_, *this);
        }

        dayIntervalForm_->show();
        dayIntervalForm_->modality();

        updateDayIntervalCaption(dayIntervalForm_->getDayInterval());
        
        updateConfigState();
    });

    saveButton_.create(*this);
    saveButton_.typeface(buttonFont);
    saveButton_.events().click([&]()
    {
        config_.action_ = (EAction)actionCombo_.option();
        config_.inactive_ = idleTimerSpin_.to_int();
        config_.warn_ = warnTimerSpin_.to_int();
        if (dayIntervalForm_) {
            config_.dayInterval_ = dayIntervalForm_->getDayInterval();
        }

        config_.save();

        close();
    });

    actionCombo_.option((size_t)config_.action_);
    idleTimerSpin_.value(std::to_string(config_.inactive_));
    warnTimerSpin_.value(std::to_string(config_.warn_));
    updateDayIntervalCaption(config_.dayInterval_);

    updateWarnSpinboxRange();
    updateConfigState();

    place_["executeLab"] << executeLab_;
    place_["actionGrid"] 
        << actionLab_ 
        << actionCombo_
        << idleTimerLab_
        << idleTimerSpin_
        << warnTimerLab_
        << warnTimerSpin_;

    place_["periodLab"] << dayIntervalLab_;
    place_["periodCombo"] << dayIntervalCombo_;
    place_["saveButton"] << saveButton_;

    place_.div(""
        " margin=4 vert "
        " < <executeLab> weight=12%> "
        " < weight=5%> "
        " < <gap=4 grid=[2,3] actionGrid> weight=45%> "
        " < weight=6%>"
        " < <periodLab vert weight=30%> <periodCombo> weight=12%> "
        " <weight=5%> "
        "< <saveButton> weight=4% > "
        "");

    place_.collocate();
}

void DetailsForm::updateConfigState()
{
    if (actionCombo_.option() == (size_t)config_.action_
        && idleTimerSpin_.to_int() == config_.inactive_
        && warnTimerSpin_.to_int() == config_.warn_
        && (!dayIntervalForm_ || dayIntervalForm_->getDayInterval().isEq(config_.dayInterval_)))
    {
        saveButton_.bgcolor(nana::color(30, 240, 30));
        saveButton_.caption("Save");
        saveButton_.enabled(false);
    }
    else {
        saveButton_.bgcolor(nana::color(255, 0, 0));
        saveButton_.caption("Save*");
        saveButton_.enabled(true);
    }
}

void DetailsForm::updateWarnSpinboxRange()
{
    int lastVal = warnTimerSpin_.to_int();
    warnTimerSpin_.range(0, (idleTimerSpin_.to_int() * 60) - 1, 1);

    if (warnTimerSpin_.range_int().second >= lastVal) {
        warnTimerSpin_.value(std::to_string(lastVal));
    }
}

void DetailsForm::updateDayIntervalCaption(const DayInterval& dayInterval)
{
    if (dayInterval.isAny()) {
        dayIntervalCombo_.caption("Ever");
    }
    else {
        std::string caption;
        if (dayInterval.time_.isAny()) {
            caption = "Any time";
        }
        else {
            caption = dayInterval.time_.getInTextFormat();
        }

        caption += ". ";
        if (dayInterval.isEnabledAllDays()) {
            caption += "Any days";
        }
        else {
            // First enabled day
            static const unsigned char showDayMax = 1;

            for (unsigned char day = 0, showDayCount = 0; day < Days::DAY_MAX; ++day) {
                if (dayInterval.hasDay(day)) {
                    if (showDayCount < showDayMax) {
                        if (showDayCount != 0) {
                            caption += ", ";
                        }

                        caption += Days::getDayFull(day);
                    }
                    else {
                        caption += ", ...";
                        break;
                    }

                    ++showDayCount;
                }
            }
        }

        dayIntervalCombo_.caption(caption);
    }
}

void DayIntervalForm::HoursComboBox::_m_complete_creation()
{
    nana::combox::_m_complete_creation();

    std::string val;
    for (unsigned char i = 0; i < 24; ++i) {

        if (i < 10) {
            val = "0";
        }
        val += std::to_string(i);
        val += "h";

        push_back(std::move(val));
    }
}

void DayIntervalForm::MinutesComboBox::_m_complete_creation()
{
    nana::combox::_m_complete_creation();

    std::string val;
    for (unsigned char i = 0; i < 60; ++i) {

        if (i < 10) {
            val = "0";
        }
        val += std::to_string(i);
        val += "m";

        push_back(std::move(val));
    }
}
