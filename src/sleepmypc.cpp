/*
   author: rescr1pt@.ya.ru
*/

#include "sleepmypc.h"
#include <ctime>   

WarnForm::WarnForm(nana::window wd, const nana::size& sz /*= { 340, 120 }*/, const nana::appearance& apr /*= { true, true, false, false, false, false, false }*/) 
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

    fs_.open(getFileName(), std::fstream::in);

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

void FaceFrom::updateProgState(bool active)
{
    if (active) {
        wLabProgVal_.caption(getProgActiveVal());
        wLabProgVal_.bgcolor(nana::color(140, 140, 140));
    }
    else {
        wLabProgVal_.caption(getProgIdleVal());
        wLabProgVal_.bgcolor(nana::color(255, 227, 50));
    }
}

void FaceFrom::updateWarningCaption(size_t remainingTime)
{
    if (warnForm_) {
        warnForm_->updateWarningCaption(remainingTime, wCombo_.caption());
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
    writeMsg += wCombo_.caption();
    writeMsg += "\r\n";

    fwrite(writeMsg.c_str(), 1, writeMsg.size(), file);

    fclose(file);
}

void FaceFrom::init_()
{
    config_.init();

    nana::paint::font labelsFont("", 11, { 400, true, true, false });
    nana::color labelsFgColor(22, 22, 22);

    this->caption("sleep my PC");
    this->bgcolor(nana::color(234, 243, 255));

    place_.div("margin=8 <grid=[2,7] margin=4 gap=6 wGrid>");
    
    // wComboLab
    wComboLab_.create(*this);
    place_["wGrid"] << wComboLab_;
    wComboLab_.bgcolor(nana::color(212, 111, 111));
    wComboLab_.typeface(labelsFont);
    wComboLab_.caption("Action:");
    wComboLab_.transparent(true);
    wComboLab_.fgcolor(labelsFgColor);
    wComboLab_.text_align(static_cast<nana::align>(1), static_cast<nana::align_v>(1));

    // wCombo
    wCombo_.create(*this);
    place_["wGrid"] << wCombo_;
    wCombo_.bgcolor(nana::color(255, 255, 155));
    wCombo_.push_back("Shutdown");
    wCombo_.push_back("Restart");
    wCombo_.push_back("Logout");
    wCombo_.option(0);
    wCombo_.events().selected([&]()
    {
        updateConfigState();
    });

    // wLabIdleTimer
    wLabIdleTimer_.create(*this);
    place_["wGrid"] << wLabIdleTimer_;
    wLabIdleTimer_.typeface(labelsFont);
    wLabIdleTimer_.fgcolor(labelsFgColor);
    wLabIdleTimer_.caption("When inactive:");
    wLabIdleTimer_.transparent(true);
    wLabIdleTimer_.text_align(static_cast<nana::align>(1), static_cast<nana::align_v>(1));

    // wSpinIdle
    wSpinIdle_.create(*this);
    place_["wGrid"] << wSpinIdle_;
    wSpinIdle_.modifier("", " min");
    wSpinIdle_.range(1, 460800, 1); // 320 months 
    wSpinIdle_.value(std::to_string(60)); // 1 hours
    wSpinIdle_.events().text_changed([&]()
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

    // wSpinWarn
    wSpinWarn_.create(*this);
    place_["wGrid"] << wSpinWarn_;
    wSpinWarn_.modifier("", " secs");
    wSpinWarn_.range(1, ((wSpinIdle_.range_int().second * 60) - 1), 1); // like idle timer -1
    wSpinWarn_.value(std::to_string(120)); // 120 seconds
    wSpinWarn_.events().text_changed([&]()
    {
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

    // wPlace1
    wPlace1_.create(*this);
    wPlace1_.transparent(true);
    place_["wGrid"] << wPlace1_;

    // wButtHistory
    wButtHistory_.create(*this);
    place_["wGrid"] << wButtHistory_;
    wButtHistory_.typeface(nana::paint::font("", 12, { 400, false, false, false }));
    wButtHistory_.caption("History");
    HistoryForm* h = new HistoryForm{ *this };
    wButtHistory_.events().click([&]()
    {
        logForm_.reset();
        logForm_ = std::make_unique<HistoryForm>(*this);
        logForm_->show();
    });


    // wPlace2
    wPlace2_.create(*this);
    wPlace2_.transparent(true);
    place_["wGrid"] << wPlace2_;

    // wPlace3
    wPlace3_.create(*this);
    wPlace3_.transparent(true);
    place_["wGrid"] << wPlace3_;

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
        config_.action_ = wCombo_.option();
        config_.inactive_ = wSpinIdle_.to_int();
        config_.warn_ = wSpinWarn_.to_int();
        config_.save();
        updateConfigState();
    });


    wCombo_.option(config_.action_);
    wSpinIdle_.value(std::to_string(config_.inactive_));
    wSpinWarn_.value(std::to_string(config_.warn_));
    updateWarnSpinboxRange();

    updateConfigState();
    updateProgState(true);

    place_.collocate();
}

void FaceFrom::updateConfigState()
{
    if (wCombo_.option() == config_.action_
        && wSpinIdle_.to_int() == config_.inactive_
        && wSpinWarn_.to_int() == config_.warn_)
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
    int lastVal = wSpinWarn_.to_int();
    wSpinWarn_.range(0, (wSpinIdle_.to_int() * 60) - 1, 1);

    if (wSpinWarn_.range_int().second >= lastVal) {
        wSpinWarn_.value(std::to_string(lastVal));
    }
}

std::string& FaceFrom::getProgActiveVal()
{
    static std::string val("Active");
    return val;
}

std::string& FaceFrom::getProgIdleVal()
{
    static std::string val("Idle");
    return val;
}

FaceFrom::Config::Config()
{

}

FaceFrom::Config::~Config()
{
    fs_.close();
}

void FaceFrom::Config::init()
{
    fs_.open("sleepmypc.ini", std::fstream::in);

    if (fs_.is_open()) {
        std::string line;
        std::getline(fs_, line);
        action_ = std::stoull(line);
        std::getline(fs_, line);
        inactive_ = std::stoull(line);
        std::getline(fs_, line);
        warn_ = std::stoull(line);
    }

    fs_.close();
}

void FaceFrom::Config::save()
{
    if (fs_.is_open()) {
        fs_.close();
    }

    fs_.open("sleepmypc.ini", std::fstream::out | std::ios::trunc);
    fs_ << action_ << std::endl << inactive_ << std::endl << warn_ << std::endl;
    fs_.flush();
    fs_.close();
}

