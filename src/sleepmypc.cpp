/*
   author: rescr1pt@.ya.ru
*/

#include "sleepmypc.h"
#include <ctime>

#include <windows.h>

inline bool timeIntervalIsAny(const TimeInterval& timeInterval)
{
    return timeInterval.beginHours_ == timeInterval.endHours_&& timeInterval.beginMinutes_ == timeInterval.endMinutes_;
}

inline bool dayIntervalIsAny(const DayInterval& dayInterval) 
{
    return dayInterval.isEnabledAllDays() && timeIntervalIsAny(dayInterval.time_);
}

void showNotice(const std::string& msg, nana::window wd)
{
    NoticeForm noticeForm(msg, wd);
    noticeForm.show();
    noticeForm.modality();
}

NoticeForm::NoticeForm(const std::string& message, nana::window wd, const ::nana::size& sz, const nana::appearance& apr)
    : message_(message)
    , nana::form(wd, sz, apr)
{
    init_();
}

NoticeForm::~NoticeForm()
{

}

void NoticeForm::init_()
{
    _place.div("margin=5 <margin=5 gap=2 field1>");
    caption("sleep my PC - notice");
    
    // wLabel
    wLabel_.create(*this);
    _place["field1"] << wLabel_;
    wLabel_.typeface(nana::paint::font("", 12, { 400, false, false, false }));
    wLabel_.transparent(true);
    wLabel_.text_align(static_cast<nana::align>(1), static_cast<nana::align_v>(1));

    wLabel_.caption(message_);

    _place.collocate();
}

WarnForm::WarnForm(nana::window wd, const nana::size& sz, const nana::appearance& apr) 
    : nana::form(wd, sz, apr)
{
    init_();
}

WarnForm::~WarnForm()
{

}

void WarnForm::updateWarningCaption(size_t remainingTime, const std::string& option)
{
    std::string warnMsg = "The PC will ";

    warnMsg += "[" + option + "] after ";
    warnMsg += "[" + std::to_string(remainingTime) + "] seconds ";

    wLabel.caption(warnMsg);
}

void WarnForm::init_()
{
    _place.div("margin=5 <margin=5 gap=2 field1>");
    caption("sleep my PC - warn");
    bgcolor(nana::color(255, 0, 0));
    // wLabel
    wLabel.create(*this);
    _place["field1"] << wLabel;
    wLabel.typeface(nana::paint::font("", 14, { 400, false, false, false }));
    wLabel.caption("PC shuts down after 5 seconds!");
    wLabel.transparent(true);
    wLabel.text_align(static_cast<nana::align>(1), static_cast<nana::align_v>(1));

    _place.collocate();
}

HistoryForm::HistoryForm(nana::window wd, const nana::size& sz /*= { 640, 480 }*/, const nana::appearance& apr /*= { true, true, false, false, false, false, false }*/)
    : nana::form(wd, sz, apr)
{
    init_();
}

HistoryForm::~HistoryForm()
{

}

std::string& HistoryForm::getFileName()
{
    static std::string name("sleepmypc.hist");
    return name;
}

void HistoryForm::init_()
{
    caption("sleep my PC - History");
    place_.div("<><weight=94% vertical<><weight=90% vertical <vertical gap=4 textboxs arrange=[400]>  <weight=25 gap=10 buttons> ><>><>");
    place_.field("textboxs") << wBoxLog_;
    place_.field("buttons") << wButtClear_;
    place_.collocate();

    wBoxLog_.editable(false);
    wBoxLog_.enable_caret();

    wButtClear_.events().click([&]()
    {
        // Close opened 
        if (fs_.is_open()) {
            fs_.close();
        }

        // Truncate and close
        fs_.open(getFileName(), std::fstream::out | std::fstream::trunc);
        if (fs_.is_open()) {
            fs_.close();
        }

        loadFile();
    });

    loadFile();
}

void HistoryForm::loadFile()
{    
    wBoxLog_.reset();

    fs_.open(getFileName(), std::fstream::in | std::ios::app);

    if (fs_.is_open()) {
        std::string line;
        while (std::getline(fs_, line)) {
            wBoxLog_.append(line + "\r\n", true);
        }

        fs_.close();
    }
    else {
        showNotice("Unable to open history file.", *this);
    }
}

DayIntervalForm::DayIntervalForm(const DayInterval& initDayInterval, nana::window wd, const nana::size& sz, const nana::appearance& apr)
    : dayInterval_(initDayInterval)
    , nana::form(wd, sz, apr)
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

    wComboBeginHours_.create(*this);
    wComboBeginMinutes_.create(*this);
    wComboEndHours_.create(*this);
    wComboEndMinutes_.create(*this);

    for (unsigned char i = 0; i < Days::DAY_MAX; ++i) {
        auto& day = weekCheckBoxes_[i];
        day.create(*this);
        day.caption(Days::getDayShort(i));
        day.transparent(true);
    }

    wButtSet_.create(*this);
    wButtSet_.caption("Set");
    wButtSet_.bgcolor(nana::colors::light_green);

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
    wComboBeginHours_.option(dayInterval_.time_.beginHours_);
    wComboBeginMinutes_.option(dayInterval_.time_.beginMinutes_);
    wComboEndHours_.option(dayInterval_.time_.endHours_);
    wComboEndMinutes_.option(dayInterval_.time_.endMinutes_);

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

    place_["intervalBegin"] << wComboBeginHours_ << wComboBeginMinutes_;
    place_["intervalSeparator"] << intervalSeparatorLabel_;
    place_["intervalEnd"] << wComboEndHours_ << wComboEndMinutes_;
    place_["week3"] << weekCheckBoxes_[0] << weekCheckBoxes_[1] << weekCheckBoxes_[2];
    place_["week4"] << weekCheckBoxes_[3] << weekCheckBoxes_[4] << weekCheckBoxes_[5] << weekCheckBoxes_[6];
    place_["setTime"] << wButtSet_;
    place_["anyTime"] << anyTimeButton_;
    place_["anyDays"] << anyDaysButton_;
    place_["everTime"] << everTimeButton_;

    place_.collocate();

}

void DayIntervalForm::eventHandler()
{
    wButtSet_.events().click([&]()
    {
        dayInterval_.disableAllDays();

        for (unsigned char i = 0; i < Days::DAY_MAX; ++i) {
            if (weekCheckBoxes_[i].checked()) {
                dayInterval_.enableDay(i);
            }
        }

        dayInterval_.time_.beginHours_ = (unsigned char)wComboBeginHours_.option();
        dayInterval_.time_.beginMinutes_ = (unsigned char)wComboBeginMinutes_.option();
        dayInterval_.time_.endHours_ = (unsigned char)wComboEndHours_.option();
        dayInterval_.time_.endMinutes_ = (unsigned char)wComboEndMinutes_.option();

        close();
    });

    wComboBeginHours_.events().selected([this]() { updateTimes(); });
    wComboBeginMinutes_.events().selected([this]() { updateTimes(); });
    wComboEndHours_.events().selected([this]() { updateTimes(); });
    wComboEndMinutes_.events().selected([this]() { updateTimes(); });

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
    wComboBeginHours_.option(0);
    wComboBeginMinutes_.option(0);
    wComboEndHours_.option(0);
    wComboEndMinutes_.option(0);
}

void DayIntervalForm::updateTimes()
{
    static const nana::color defColor;

    if (wComboBeginHours_.option() == 0
        && wComboBeginMinutes_.option() == 0
        && wComboEndHours_.option() == 0
        && wComboEndMinutes_.option() == 0) {
        wComboBeginHours_.fgcolor(nana::colors::light_gray);
        wComboBeginMinutes_.fgcolor(nana::colors::light_gray);
        wComboEndHours_.fgcolor(nana::colors::light_gray);
        wComboEndMinutes_.fgcolor(nana::colors::light_gray);
    }
    else {  
        wComboBeginHours_.fgcolor(defColor);
        wComboBeginMinutes_.fgcolor(defColor);
        wComboEndHours_.fgcolor(defColor);
        wComboEndMinutes_.fgcolor(defColor);
    }
}

FaceFrom::FaceFrom(nana::window wd, const nana::size& sz /*= { 260, 200 }*/, const nana::appearance& apr /*= { true, true, false, false, false, false, false }*/)
    : nana::form(wd, sz, apr)
{
    init_();


    //<*ctor

    //*>
}

FaceFrom::~FaceFrom()
{
    //<*dtor

    //*>
}

void FaceFrom::showWarning(bool val)
{
    if (val) {
        if (!warnForm_) {
            warnForm_ = std::make_unique<WarnForm>(*this);
            warnForm_->show();
        }
    }
    else {
        if (warnForm_) {
            warnForm_->close();
            warnForm_.reset();
        }
    }
}

bool FaceFrom::isShowWarning() const
{
    return warnForm_.get();
}

void FaceFrom::updateProgState(ECurrentStatus currentStatus)
{
    switch (currentStatus)
    {
        case FaceFrom::ECurrentStatus::Active:
            progValueLab_.caption("Active");
            progValueLab_.bgcolor(nana::colors::yellow_green);

            break;
        case FaceFrom::ECurrentStatus::Inactive:
            progValueLab_.caption("Idle");
            progValueLab_.bgcolor(nana::colors::orange);

            break;
        case FaceFrom::ECurrentStatus::Disabled:
        default:
            progValueLab_.caption("Disabled");
            progValueLab_.bgcolor(nana::colors::light_slate_gray);
            break;
    }
}

void FaceFrom::updateWarningCaption(size_t remainingTime)
{
    if (warnForm_) {
        warnForm_->updateWarningCaption(remainingTime, actionCombo_.caption());
    }
}

void FaceFrom::processTriggeredAction()
{
    FILE* file = fopen(HistoryForm::getFileName().c_str(), "a+");

    std::string writeMsg;

    auto nowTimeT = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    const char* date = std::ctime(&nowTimeT);

    // Remove endline
    writeMsg.append(date, strlen(date) - 1); 
    writeMsg += "\t";
    writeMsg += actionCombo_.caption();
    writeMsg += "\r\n";

    fwrite(writeMsg.c_str(), 1, writeMsg.size(), file);

    fclose(file);

#ifdef _DEBUG
    showNotice("Action triggered!", *this);
#else

    const EAction action = (EAction)actionCombo_.option();
	
    switch (action)
    {
        // Shutdown
        case EAction::Shutdown:
        {
            system("shutdown /s /f /t 1");			
            break;
        }
        // Restart
        case EAction::Restart:
        {
            system("shutdown /r /f /t 1");
            break;
        }
        // Log off
        case EAction::Logoff:
        {
            system("shutdown /l /f");
            break;
        }
        // Hibernation 
        case EAction::Hibernation:
        {
            system("shutdown /h /f");
            break;
        }
        case EAction::No:
        default:
        {
            showNotice("Triggered an invalid action!", *this);
     
            break;
        }
    }
	
#endif
}

bool FaceFrom::isAnyDayInterval() const
{
    return dayIntervalIsAny(config_.dayInterval_);
}

void FaceFrom::init_()
{
    try
    {
        config_.load();
    }
    catch (const ConfigException & e)
    {        
        showNotice(std::string("Error reading configuration file - ") + e.what(), *this);
    }

    // Set ico to form from exe path
    std::wstring app_path(1024, '\0');
    size_t applicationFilePath = (size_t)GetModuleFileNameW(0, &app_path.front(), (DWORD)app_path.size());
    if (applicationFilePath > 0) {
        app_path.resize(applicationFilePath);
        icon(nana::paint::image(app_path));
    }
    else {
        showNotice("Unable to set icon for form", *this);
    }

    static const nana::paint::font descriptionsFont("", 12, { 400, true, true, false });
    static const nana::paint::font definitionFont("", 11, { 400, false, false, false });
    static const nana::paint::font buttonFont("", 12, { 400, false, false, false });

    caption("sleep my PC");
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
    actionCombo_.push_back("No");
    actionCombo_.push_back("Shutdown");
    actionCombo_.push_back("Restart");
    actionCombo_.push_back("Log off");
    actionCombo_.push_back("Hibernation");
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

    additionalOptionsLab_.create(*this);
    additionalOptionsLab_.caption("Additional options");
    additionalOptionsLab_.typeface(descriptionsFont);
    additionalOptionsLab_.transparent(true);
    additionalOptionsLab_.text_align(static_cast<nana::align>(1), static_cast<nana::align_v>(1));

    mouseMoveButton_.create(*this);
    mouseMoveButton_.caption("Check mouse movement");
    mouseMoveButton_.typeface(buttonFont);
    mouseMoveButton_.enable_pushed(true);
    mouseMoveButton_.events().click([&]()
    {
        updateConfigState();
    });


    progValueLab_.create(*this);
    progValueLab_.typeface(nana::paint::font("", 11, { 400, false, false, false }));
    progValueLab_.text_align(static_cast<nana::align>(1), static_cast<nana::align_v>(1));

    historyButton_.create(*this);
    historyButton_.typeface(buttonFont);
    historyButton_.caption("History");
    historyButton_.events().click([&]()
    {
        logForm_.reset();
        logForm_ = std::make_unique<HistoryForm>(*this);
        logForm_->show();
    });

    saveButton_.create(*this);
    saveButton_.typeface(buttonFont);
    saveButton_.events().click([&]()
    {
        config_.action_ = (EAction)actionCombo_.option();
        config_.inactive_ = idleTimerSpin_.to_int();
        config_.warn_ = warnTimerSpin_.to_int();
        config_.checkMouseMovement_ = (mouseMoveButton_.pushed() ? 1 : 0);
        if (dayIntervalForm_) {
            config_.dayInterval_ = dayIntervalForm_->getDayInterval();
        }

        config_.save();

        updateConfigState();
    });

    actionCombo_.option((size_t)config_.action_);
    idleTimerSpin_.value(std::to_string(config_.inactive_));
    warnTimerSpin_.value(std::to_string(config_.warn_));
    updateDayIntervalCaption(config_.dayInterval_);
    mouseMoveButton_.pushed(config_.checkMouseMovement_);

    updateWarnSpinboxRange();

    updateConfigState();
    updateProgState(ECurrentStatus::Disabled);

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
    place_["additionalOptionsLab"] << additionalOptionsLab_;
    place_["mouseMoveButton"] << mouseMoveButton_;
    place_["progValueLab"] << progValueLab_;
    place_["historyButton"] << historyButton_;
    place_["saveButton"] << saveButton_;

    place_.div(""
        "margin=4 vert "
        "< executeLab> "
        " <weight=2%> "
        "< gap=4 grid=[2,3] actionGrid weight=25%> "
        " <weight=6%> "
        "< <periodLab vert weight=30%> <periodCombo> weight=10%  > "
        " <weight=4%> "
         "< <vert weight=8%> <mouseMoveButton > <vert weight=8%> weight=10% > "
        " <weight=5%> "
        "< progValueLab weight=10% > "
        " <weight=4%> "
        "< <historyButton> <vert weight=2%> <saveButton> >"
        "");

    place_.collocate();
}

void FaceFrom::updateConfigState()
{
    if (actionCombo_.option() == (size_t)config_.action_
        && idleTimerSpin_.to_int() == config_.inactive_
        && warnTimerSpin_.to_int() == config_.warn_
        && mouseMoveButton_.pushed() == config_.checkMouseMovement_
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

void FaceFrom::updateWarnSpinboxRange()
{
    int lastVal = warnTimerSpin_.to_int();
    warnTimerSpin_.range(0, (idleTimerSpin_.to_int() * 60) - 1, 1);

    if (warnTimerSpin_.range_int().second >= lastVal) {
        warnTimerSpin_.value(std::to_string(lastVal));
    }
}

void FaceFrom::updateDayIntervalCaption(const DayInterval& dayInterval)
{
    if (dayIntervalIsAny(dayInterval)) {
        dayIntervalCombo_.caption("Ever");
    }
    else {
        std::string caption;
        if (timeIntervalIsAny(dayInterval.time_)) {
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
