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

std::unique_ptr<NoticeForm> showNotice(const std::string& msg, nana::window wd)
{
    auto noticeForm = std::make_unique<NoticeForm>(msg, wd);
    noticeForm->show();
    noticeForm->modality();
    return noticeForm;
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

    intervalSeparatorLabel_.create(*this);
    intervalSeparatorLabel_.typeface(labelsFont);
    intervalSeparatorLabel_.caption(" - ");
    intervalSeparatorLabel_.transparent(true);
    intervalSeparatorLabel_.text_align(static_cast<nana::align>(1), static_cast<nana::align_v>(1));

    wComboBeginHours_.create(*this);
    wComboBeginMinutes_.create(*this);
    wComboEndHours_.create(*this);
    wComboEndMinutes_.create(*this);

    static const char weekDesc[Days::DAY_MAX][4] = { "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun" };

    for (unsigned char i = 0; i < Days::DAY_MAX; ++i) {
        auto& day = weekCheckBoxes_[i];
        day.create(*this);
        day.caption(weekDesc[i]);
    }

    wButtSet_.create(*this);
    wButtSet_.caption("Set");
    wButtSet_.bgcolor(nana::colors::light_green);
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

    wButtAnyTime_.create(*this);
    wButtAnyTime_.caption("Any time");
    wButtAnyTime_.events().click([&]()
    {
        dayInterval_.enableAllDays();
        dayInterval_.time_.beginHours_ = 0;
        dayInterval_.time_.beginMinutes_ = 0;
        dayInterval_.time_.endHours_ = 0;
        dayInterval_.time_.endMinutes_ = 0;
        close();
    });

    // Init values
    for (unsigned char i = 0; i < Days::DAY_MAX; ++i) {
        weekCheckBoxes_[i].check(dayInterval_.hasDay(i));
    }
    wComboBeginHours_.option(dayInterval_.time_.beginHours_);
    wComboBeginMinutes_.option(dayInterval_.time_.beginMinutes_);
    wComboEndHours_.option(dayInterval_.time_.endHours_);
    wComboEndMinutes_.option(dayInterval_.time_.endMinutes_);


    /// Collate
    place_.div("margin=4 vert"
        "< weight=25% <intervalBegin> <vert intervalSeparator weight=20> <intervalEnd> >"
        "<weight=10%> "
        "< weight=20% <week3> >"
        "< weight=20% <week4> >"
        "<weight=10%> "
        "< <anyTime> <setTime> >");

    place_["intervalBegin"] << wComboBeginHours_ << wComboBeginMinutes_;
    place_["intervalSeparator"] << intervalSeparatorLabel_;
    place_["intervalEnd"] << wComboEndHours_ << wComboEndMinutes_;
    place_["week3"] << weekCheckBoxes_[0] << weekCheckBoxes_[1] << weekCheckBoxes_[2];
    place_["week4"] << weekCheckBoxes_[3] << weekCheckBoxes_[4] << weekCheckBoxes_[5] << weekCheckBoxes_[6];
    place_["setTime"] << wButtSet_;
    place_["anyTime"] << wButtAnyTime_;

    place_.collocate();

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
            wLabProgVal_.caption("Active");
            wLabProgVal_.bgcolor(nana::colors::yellow_green);

            break;
        case FaceFrom::ECurrentStatus::Inactive:
            wLabProgVal_.caption("Idle");
            wLabProgVal_.bgcolor(nana::colors::yellow);

            break;
        case FaceFrom::ECurrentStatus::Disabled:
        default:
            wLabProgVal_.caption("Disabled");
            wLabProgVal_.bgcolor(nana::colors::button_face);
            break;
    }
}

void FaceFrom::updateWarningCaption(size_t remainingTime)
{
    if (warnForm_) {
        warnForm_->updateWarningCaption(remainingTime, wComboAction_.caption());
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
    writeMsg += wComboAction_.caption();
    writeMsg += "\r\n";

    fwrite(writeMsg.c_str(), 1, writeMsg.size(), file);

    fclose(file);

#ifdef _DEBUG
    showNotice("Action triggered!", *this);
#else

    const EAction action = (EAction)wComboAction_.option();
	
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

    static const nana::paint::font labelsFont("", 11, { 400, true, true, false });
    static const nana::color labelsFgColor(22, 22, 22);

    // Set ico to form
    std::wstring app_path(1024, '\0');
    size_t applicationFilePath = (size_t)GetModuleFileNameW(0, &app_path.front(), (DWORD)app_path.size());
    if (applicationFilePath > 0) {
        app_path.resize(applicationFilePath);
        this->icon(nana::paint::image(app_path));
    }
    else {
        showNotice("Unable to set icon for form", *this);
    }

    this->caption("sleep my PC");
    this->bgcolor(nana::color(234, 243, 255));

    place_.div("margin=8 <grid=[2,9] margin=4 gap=5 wGrid>");
    
    // Fill empty places
    static nana::panel<true> wEmptyPlace;
    wEmptyPlace.create(*this);
    wEmptyPlace.transparent(true);

    // wLabAction_
    wLabAction_.create(*this);
    place_["wGrid"] << wLabAction_;
    wLabAction_.bgcolor(nana::color(212, 111, 111));
    wLabAction_.typeface(labelsFont);
    wLabAction_.caption("Action:");
    wLabAction_.transparent(true);
    wLabAction_.fgcolor(labelsFgColor);
    wLabAction_.text_align(static_cast<nana::align>(1), static_cast<nana::align_v>(1));

    // wComboAction_
    wComboAction_.create(*this);
    place_["wGrid"] << wComboAction_;
    wComboAction_.bgcolor(nana::color(255, 255, 155));
    wComboAction_.push_back("No");
    wComboAction_.push_back("Shutdown");
    wComboAction_.push_back("Restart");
    wComboAction_.push_back("Log off");
    wComboAction_.push_back("Hibernation");
    wComboAction_.option((size_t)EAction::No);
    wComboAction_.events().selected([&]()
    {
        updateConfigState();
    });

    // wLabIdleTimer_
    wLabIdleTimer_.create(*this);
    place_["wGrid"] << wLabIdleTimer_;
    wLabIdleTimer_.typeface(labelsFont);
    wLabIdleTimer_.fgcolor(labelsFgColor);
    wLabIdleTimer_.caption("When inactive:");
    wLabIdleTimer_.transparent(true);
    wLabIdleTimer_.text_align(static_cast<nana::align>(1), static_cast<nana::align_v>(1));

    // wSpinIdleTimer_
    wSpinIdleTimer_.create(*this);
    place_["wGrid"] << wSpinIdleTimer_;
    wSpinIdleTimer_.modifier("", " min");
    wSpinIdleTimer_.range(1, 460800, 1); // 320 months 
    wSpinIdleTimer_.events().text_changed([&]()
    {
        updateWarnSpinboxRange();

        updateConfigState();
    });

    // wLabWarnTimer
    wLabWarnTimer_.create(*this);
    place_["wGrid"] << wLabWarnTimer_;
    wLabWarnTimer_.typeface(labelsFont);
    wLabWarnTimer_.fgcolor(labelsFgColor);
    wLabWarnTimer_.caption("Warn when left:");
    wLabWarnTimer_.transparent(true);
    wLabWarnTimer_.text_align(static_cast<nana::align>(1), static_cast<nana::align_v>(1));

    // wSpinWarnTimer_
    wSpinWarnTimer_.create(*this);
    place_["wGrid"] << wSpinWarnTimer_;
    wSpinWarnTimer_.modifier("", " secs");
    wSpinWarnTimer_.range(1, ((wSpinIdleTimer_.range_int().second * 60) - 1), 1); // like idle timer -1
    wSpinWarnTimer_.events().text_changed([&]()
    {
        updateConfigState();
    });

    // wLabTimeIntervalDesc_
    dayIntervalLabel_.create(*this);
    place_["wGrid"] << dayIntervalLabel_;
    dayIntervalLabel_.typeface(labelsFont);
    dayIntervalLabel_.fgcolor(labelsFgColor);
    dayIntervalLabel_.caption("In period:");
    dayIntervalLabel_.transparent(true);
    dayIntervalLabel_.text_align(static_cast<nana::align>(1), static_cast<nana::align_v>(1));

    // wComboTimeInterval_
    dayIntervalCombo_.create(*this);
    place_["wGrid"] << dayIntervalCombo_;
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


    // wLabProgDesc
    wLabProgDesc_.create(*this);
    place_["wGrid"] << wLabProgDesc_;
    wLabProgDesc_.typeface(labelsFont);
    wLabProgDesc_.fgcolor(labelsFgColor);
    wLabProgDesc_.caption("Current status:");
    wLabProgDesc_.transparent(true);
    wLabProgDesc_.text_align(static_cast<nana::align>(1), static_cast<nana::align_v>(1));

    // wLabProgVal
    wLabProgVal_.create(*this);
    place_["wGrid"] << wLabProgVal_;
    wLabProgVal_.typeface(nana::paint::font("", 11, { 400, false, false, false }));
    wLabProgVal_.text_align(static_cast<nana::align>(1), static_cast<nana::align_v>(1));

    place_["wGrid"] << wEmptyPlace;

    // wCheckMouseMove
    wCheckMouseMove_.create(*this);
    wCheckMouseMove_.transparent(true);
    place_["wGrid"] << wCheckMouseMove_;
    wCheckMouseMove_.caption("Check mouse");
    wCheckMouseMove_.typeface(nana::paint::font("", 11, { 400, false, false, false }));
    wCheckMouseMove_.fgcolor(labelsFgColor);
    wCheckMouseMove_.events().checked([&]()
    {
        updateConfigState();
    });

    place_["wGrid"] << wEmptyPlace;

    // wButtHistory
    wButtHistory_.create(*this);
    place_["wGrid"] << wButtHistory_;
    wButtHistory_.typeface(nana::paint::font("", 12, { 400, false, false, false }));
    wButtHistory_.caption("History");
    wButtHistory_.events().click([&]()
    {
        logForm_.reset();
        logForm_ = std::make_unique<HistoryForm>(*this);
        logForm_->show();
    });


    // Spaces
    place_["wGrid"] << wEmptyPlace;
    place_["wGrid"] << wEmptyPlace;

    // wLabCop
    wLabCop_.create(*this);
    place_["wGrid"] << wLabCop_;
    wLabCop_.typeface(nana::paint::font("", 9, { 400, false, true, false }));
    wLabCop_.caption("rescr1pt@ya.ru (c)");
    wLabCop_.transparent(true);
    wLabCop_.text_align(static_cast<nana::align>(1), static_cast<nana::align_v>(1));

    // wButtSave		
    wButtSave_.create(*this);
    place_["wGrid"] << wButtSave_;
    wButtSave_.typeface(nana::paint::font("", 12, { 400, false, false, false }));
    wButtSave_.events().click([&]()
    {
        config_.action_ = (EAction)wComboAction_.option();
        config_.inactive_ = wSpinIdleTimer_.to_int();
        config_.warn_ = wSpinWarnTimer_.to_int();
        config_.checkMouseMovement_ = (wCheckMouseMove_.checked() ? 1 : 0);
        if (dayIntervalForm_) {
            config_.dayInterval_ = dayIntervalForm_->getDayInterval();
        }

        config_.save();

        updateConfigState();
    });

    // First values
    wComboAction_.option((size_t)config_.action_);
    wSpinIdleTimer_.value(std::to_string(config_.inactive_));
    wSpinWarnTimer_.value(std::to_string(config_.warn_));
    updateDayIntervalCaption(config_.dayInterval_);
    wCheckMouseMove_.check(config_.checkMouseMovement_);

    updateWarnSpinboxRange();

    updateConfigState();
    updateProgState(ECurrentStatus::Disabled);

    place_.collocate();
}

void FaceFrom::updateConfigState()
{
    if (wComboAction_.option() == (size_t)config_.action_
        && wSpinIdleTimer_.to_int() == config_.inactive_
        && wSpinWarnTimer_.to_int() == config_.warn_
        && wCheckMouseMove_.checked() == config_.checkMouseMovement_
        && (!dayIntervalForm_ || dayIntervalForm_->getDayInterval().isEq(config_.dayInterval_)))
    {
        wButtSave_.bgcolor(nana::color(30, 240, 30));
        wButtSave_.caption("Saved");
        wButtSave_.enabled(false);
    }
    else {
        wButtSave_.bgcolor(nana::color(255, 0, 0));
        wButtSave_.caption("Save*");
        wButtSave_.enabled(true);
    }
}

void FaceFrom::updateWarnSpinboxRange()
{
    int lastVal = wSpinWarnTimer_.to_int();
    wSpinWarnTimer_.range(0, (wSpinIdleTimer_.to_int() * 60) - 1, 1);

    if (wSpinWarnTimer_.range_int().second >= lastVal) {
        wSpinWarnTimer_.value(std::to_string(lastVal));
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

        caption += ", ";
        if (dayInterval.isEnabledAllDays()) {
            caption += "any days";
        }
        else {
            // First enabled day
            for (unsigned char day = 0; day < Days::DAY_MAX; ++day) {
                if (dayInterval.hasDay(day)) {
                    caption += Days::getDayShort(day);
                    caption += "...";
                    break;
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
