/*
   author: rescr1pt@.ya.ru
*/

#include "sleepmypc.h"
#include <ctime>

#include <windows.h>

bool timeIntervalIsAny(const TimeInterval& timeInterval) 
{
    return timeInterval.beginHours_ == timeInterval.endHours_
        && timeInterval.beginMinutes_ == timeInterval.endMinutes_;
}

NoticeForm::NoticeForm(Action action, nana::window wd, const ::nana::size& sz, const nana::appearance& apr)
    : action_(action)
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
    wLabel.create(*this);
    _place["field1"] << wLabel;
    wLabel.typeface(nana::paint::font("", 12, { 400, false, false, false }));
    wLabel.transparent(true);
    wLabel.text_align(static_cast<nana::align>(1), static_cast<nana::align_v>(1));

    switch (action_)
    {
        case NoticeForm::Action::DebugTriggeredAction:
            wLabel.caption("Action triggered!");
            break;
        case NoticeForm::Action::TriggeredInvalidAction:
            wLabel.caption("Triggered an invalid action!");
            break;
        default:
            break;
    }

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
        wBoxLog_.append("Cannot open a history file!\r\n", true);
    }
}

TimeIntervalForm::TimeIntervalForm(const TimeInterval& initTimeInterval, nana::window wd, const nana::size& sz, const nana::appearance& apr)
    : timeInterval(initTimeInterval)
    , nana::form(wd, sz, apr)
{
    init_();
}

TimeIntervalForm::~TimeIntervalForm()
{

}

void TimeIntervalForm::init_()
{
    caption("Time interval selector");

    static const nana::paint::font labelsFont("", 11, { 400, true, true, false });

    wLabBegin_.create(*this);
    wLabBegin_.typeface(labelsFont);
    wLabBegin_.caption("Begin:");
    wLabBegin_.transparent(true);
    wLabBegin_.text_align(static_cast<nana::align>(1), static_cast<nana::align_v>(1));

    wComboBeginHours_.create(*this);

    wComboBeginMinutes_.create(*this);

    wLabEnd_.create(*this);
    wLabEnd_.typeface(labelsFont);
    wLabEnd_.caption("End:");
    wLabEnd_.transparent(true);
    wLabEnd_.text_align(static_cast<nana::align>(1), static_cast<nana::align_v>(1));

    wComboEndHours_.create(*this);
    wComboEndMinutes_.create(*this);

    wButtSet_.create(*this);
    wButtSet_.caption("Set");
    wButtSet_.bgcolor(nana::colors::light_green);
    wButtSet_.events().click([&]()
    {
        timeInterval.beginHours_ = (unsigned short)wComboBeginHours_.option();
        timeInterval.beginMinutes_ = (unsigned short)wComboBeginMinutes_.option();
        timeInterval.endHours_ = (unsigned short)wComboEndHours_.option();
        timeInterval.endMinutes_ = (unsigned short)wComboEndMinutes_.option();
        close();
    });

    wButtAnyTime_.create(*this);
    wButtAnyTime_.caption("Any time");
    //wButtAnyTime_.bgcolor(nana::colors::light_green);
    wButtAnyTime_.events().click([&]()
    {
        timeInterval.beginHours_ = 0;
        timeInterval.beginMinutes_ = 0;
        timeInterval.endHours_ = 0;
        timeInterval.endMinutes_ = 0;
        close();
    });

    // Init values
    wComboBeginHours_.option(timeInterval.beginHours_);
    wComboBeginMinutes_.option(timeInterval.beginMinutes_);
    wComboEndHours_.option(timeInterval.endHours_);
    wComboEndMinutes_.option(timeInterval.endMinutes_);


    /// Collate
    place_.div("vert <>"
        "<vert <beginTime> <endTime> weight=40% gap=4 margin=4> "
        "<<> <setTime gap=4 margin=4> weight=20%>  "
        "<weight=8%> "
        "<anyTime weight=20% gap=4 margin=4> "
        "<>");

    place_["beginTime"] << wLabBegin_;
    place_["beginTime"] << wComboBeginHours_;
    place_["beginTime"] << wComboBeginMinutes_;
    place_["endTime"] << wLabEnd_;
    place_["endTime"] << wComboEndHours_;
    place_["endTime"] << wComboEndMinutes_;
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
    noticeForm_ = std::make_unique<NoticeForm>(NoticeForm::Action::DebugTriggeredAction, *this);
    noticeForm_->show();
    noticeForm_->modality();
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
            noticeForm_ = std::make_unique<NoticeForm>(NoticeForm::Action::TriggeredInvalidAction, *this);
            noticeForm_->show();
            noticeForm_->modality();
     
            break;
        }
    }
	
#endif
}

bool FaceFrom::isAnyTimeInterval() const
{
    return timeIntervalIsAny(config_.timeInterval_);
}

void FaceFrom::init_()
{
    config_.load();

    static const nana::paint::font labelsFont("", 11, { 400, true, true, false });
    static const nana::color labelsFgColor(22, 22, 22);

    // Set ico to form
    std::wstring app_path(1024, '\0');
    size_t applicationFilePath = (size_t)GetModuleFileNameW(0, &app_path.front(), (DWORD)app_path.size());
    if (applicationFilePath > 0) {
        app_path.resize(applicationFilePath);
        this->icon(nana::paint::image(app_path));
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
    wLabTimeInterval_.create(*this);
    place_["wGrid"] << wLabTimeInterval_;
    wLabTimeInterval_.typeface(labelsFont);
    wLabTimeInterval_.fgcolor(labelsFgColor);
    wLabTimeInterval_.caption("In time interval:");
    wLabTimeInterval_.transparent(true);
    wLabTimeInterval_.text_align(static_cast<nana::align>(1), static_cast<nana::align_v>(1));

    // wComboTimeInterval_
    wComboTimeInterval_.create(*this);
    place_["wGrid"] << wComboTimeInterval_;
    wComboTimeInterval_.bgcolor(nana::color(255, 255, 155));
    wComboTimeInterval_.events().click([&]()
    {
        if (timeIntervalForm_) {
            timeIntervalForm_ = std::make_unique<TimeIntervalForm>(timeIntervalForm_->getTimeInterval() ,*this);
        }
        else {
            timeIntervalForm_ = std::make_unique<TimeIntervalForm>(config_.timeInterval_, *this);
        }

        timeIntervalForm_->show();
        timeIntervalForm_->modality();

        updateTimeIntervalCaption(timeIntervalForm_->getTimeInterval());
        
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
        if (timeIntervalForm_) {
            config_.timeInterval_ = timeIntervalForm_->getTimeInterval();
        }

        config_.save();

        updateConfigState();
    });

    // First values
    wComboAction_.option((size_t)config_.action_);
    wSpinIdleTimer_.value(std::to_string(config_.inactive_));
    wSpinWarnTimer_.value(std::to_string(config_.warn_));
    updateTimeIntervalCaption(config_.timeInterval_);
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
        && (!timeIntervalForm_ || timeIntervalForm_->getTimeInterval().isEq(config_.timeInterval_)))
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

void FaceFrom::updateTimeIntervalCaption(const TimeInterval& timeInterval)
{
    if (timeIntervalIsAny(timeInterval)) {
        wComboTimeInterval_.caption("Any time");
    }
    else {
        wComboTimeInterval_.caption(timeInterval.getInTextFormat());
    }
}

