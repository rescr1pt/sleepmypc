#include "DashboardForm.h"
#include "FormTools.h"

#include "DetailsForm.h"

WarnForm::WarnForm(nana::window wd, EAction action)
    : nana::form(wd, { 340, 140 }, { true, true, false, false, false, false, false })
    , action_(action)
{
    init_();
}

WarnForm::~WarnForm()
{

}

void WarnForm::updateWarningCaption(size_t remainingTime)
{
    std::string warnMsg = "The PC will ";

    warnMsg += "[";
    warnMsg += getActionName(action_); 
    warnMsg += "] after ";
    warnMsg += "[" + std::to_string(remainingTime) + "] seconds ";

    messageLabel_.caption(warnMsg);
}

void WarnForm::init_()
{
    place_.div("margin=5 <margin=5 gap=2 field1>");
    caption("sleep my PC - warn");
    bgcolor(nana::color(255, 0, 0));
    // wLabel
    messageLabel_.create(*this);
    place_["field1"] << messageLabel_;
    messageLabel_.typeface(nana::paint::font("", 14, { 400, false, false, false }));
    messageLabel_.caption("PC shuts down after 5 seconds!");
    messageLabel_.transparent(true);
    messageLabel_.text_align(static_cast<nana::align>(1), static_cast<nana::align_v>(1));

    place_.collocate();
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
    place_.field("textboxs") << logTextBox_;
    place_.field("buttons") << clearButton_;
    place_.collocate();

    logTextBox_.editable(false);
    logTextBox_.enable_caret();

    clearButton_.events().click([&]()
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
    logTextBox_.reset();

    fs_.open(getFileName(), std::fstream::in | std::ios::app);

    if (fs_.is_open()) {
        std::string line;
        while (std::getline(fs_, line)) {
            logTextBox_.append(line + "\r\n", true);
        }

        fs_.close();
    }
    else {
        formtools::showNotice("Unable to open history file.", *this);
    }
}

DashboardForm::DashboardForm(nana::window wd)
    : nana::form(wd, { 350, 290 }, { true, true, false, false, true /*min*/, false, false })
{
    init_();
}

DashboardForm::~DashboardForm()
{

}

const bool DashboardForm::findObservedConfig(unsigned char day, unsigned char hour, unsigned char minutes, ObservedConfig& observedConfig) const
{
    if (!dashboardConfig_.activeDetails_.hasDetails()) {
        return false;
    }

    for (unsigned char i = 0; i < DASHOBARD_DETAIL_COUNT; ++i) {
        if (!actions_[i].detailsConfig_->isInitialized()) {
            false;
        }

        const auto& interval = actions_[i].detailsConfig_->dayInterval_;

        if (!interval.isEnabledAllDays() || !interval.hasDay(day)) {
            continue;
        }

        if (!interval.time_.isAny() || !interval.time_.isInInterval(hour, minutes)) {
            continue;
        }

        const bool isInterval = (interval.isAny()) ||  (interval.hasDay(day) && interval.time_.isInInterval(hour, minutes));

        if (!isInterval) {
            continue;
        }

        observedConfig.triggerActionWhendleSecs_ = actions_[i].detailsConfig_->inactive_;
        observedConfig.warnActionWhenIdleSecs_ = actions_[i].detailsConfig_->warn_;
        observedConfig.triggerActionIndex_ = i;


        return true;
    }

    return false;
}

void DashboardForm::updateWarningCaption(size_t)
{
/*
    if (warnForm_) {
        warnForm_->updateWarningCaption(remainingTime);
    }*/
}

void DashboardForm::showWarning(bool)
{
/*
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
    }*/
}

void DashboardForm::processTriggeredAction(unsigned char i)
{
    FILE* file = fopen(HistoryForm::getFileName().c_str(), "a+");

    std::string writeMsg;

    auto nowTimeT = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    const char* date = std::ctime(&nowTimeT);

    // Remove end line
    writeMsg.append(date, strlen(date) - 1);
    writeMsg += "\t";
    writeMsg +=  getActionName((EAction)actions_[i].detailsConfig_->action_);
    writeMsg += "\r\n";

    fwrite(writeMsg.c_str(), 1, writeMsg.size(), file);

    fclose(file);

#ifdef _DEBUG
    formtools::showNotice("Action triggered!", *this);
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

void DashboardForm::updateProgState(ECurrentStatus currentStatus)
{
    switch (currentStatus)
    {
        case ECurrentStatus::Active:
            progValueLab_.caption("Active");
            progValueLab_.bgcolor(nana::colors::yellow_green);

            break;
        case ECurrentStatus::Inactive:
            progValueLab_.caption("Idle");
            progValueLab_.bgcolor(nana::colors::orange);

            break;
        case ECurrentStatus::Disabled:
        default:
            progValueLab_.caption("Disabled");
            progValueLab_.bgcolor(nana::colors::light_slate_gray);
            break;
    }
}

void DashboardForm::setNearDetail(size_t detailIndex)
{
    actions_->group_.bgcolor(nana::colors::light_sea_green);
}

void DashboardForm::resetNearDetail()
{
    actions_->group_.bgcolor(nana::colors::white_smoke);
}

void DashboardForm::init_()
{
    formtools::initIco(*this);
    bgcolor(nana::colors::white_smoke);

    try
    {
        dashboardConfig_.load();
    }
    catch (const DetailsConfig::Exception & e)
    {
        formtools::showNotice(std::string("Error reading dashboard configuration file - ") + e.what(), *this);
    }

    mouseMoveButton_.create(*this);
    mouseMoveButton_.caption("Check mouse movement");
    mouseMoveButton_.enable_pushed(true);
    mouseMoveButton_.pushed(dashboardConfig_.checkMouseMovement_);
    mouseMoveButton_.events().click([&]()
    {
        dashboardConfig_.checkMouseMovement_ = mouseMoveButton_.pushed();
        dashboardConfig_.save();
    });

    progValueLab_.create(*this);
    progValueLab_.typeface(nana::paint::font("", 11, { 400, false, false, false }));
    progValueLab_.text_align(static_cast<nana::align>(1), static_cast<nana::align_v>(1));
    updateProgState(ECurrentStatus::Disabled);

    historyButton_.create(*this);
    historyButton_.typeface(nana::paint::font("", 11, { 400, false, false, false }));
    historyButton_.caption("History");

    historyButton_.events().click([&]()
    {
        logForm_.reset();
        logForm_ = std::make_unique<HistoryForm>(*this);
        logForm_->show();
    });

    for (unsigned char i = 0; i < DASHOBARD_DETAIL_COUNT; ++i) {
        auto& act = actions_[i];
        act.number_ = i;

        act.detailsConfig_ = std::make_unique<DetailsConfig>(i);

        try
        {
            act.detailsConfig_->load();
        }
        catch (const DetailsConfig::Exception & e)
        {
            formtools::showNotice(std::string("Error reading details configuration " + std::to_string(i) + " file - ") + e.what(), *this);
        }

        act.group_.create(*this);
        
        act.descLabel_.create(act.group_);
        act.descLabel_.transparent(true);
        act.descLabel_.format(true);
        
        act.detailButton_.create(act.group_);
        act.detailButton_.caption("Details");
        
        act.disableButton_.create(act.group_);
        act.disableButton_.enable_pushed(true);
        act.disableButton_.pushed(!dashboardConfig_.activeDetails_.hasDetail(i));

        updateActionDescription(act);
        actionEventHandle(act);

        auto& group = act.group_;

        group["descLabel"] << act.descLabel_;
        group["detailButton"] << act.detailButton_;
        group["disableButton"] << act.disableButton_;

        group.div(""
            "vert margin=5 "
            "<descLabel weight=80%> "
            "< <detailButton> <vert weight=10> <disableButton>> "
            "");

        place_["actionGrid"] << group;
    }

    resetNearDetail();

    place_["mouseMoveButton"] << mouseMoveButton_;
    place_["idleIndicatorLabel"] << progValueLab_;
    place_["historyButton"] << historyButton_;

    place_.div(""
        "margin=4 vert "
        "< gap=4 grid=[2,2] actionGrid weight=85%> "
        " <weight=2%> "
        " < <vert weight=40%> <idleIndicatorLabel> <vert weight=5>  <historyButton> > "
        "");

    place_.collocate();
}

void DashboardForm::updateActionDescription(ActionGroup& act)
{
    // Update button state
    if (act.disableButton_.pushed()) {
        act.disableButton_.caption("Disabled");
    }
    else {
        act.disableButton_.caption("Enabled");
    }

    // Update description
    if (!act.detailsConfig_->isInitialized()) {
        act.descLabel_.caption("No configuration. Click <bold>Details</> to create.");
    }
    else {
        if (act.disableButton_.pushed()) {
            act.descLabel_.caption("Disabled. Click <bold>Disabled</> to enable.");
        }
        else if (act.detailsConfig_->action_ == EAction::No) {
            act.descLabel_.caption("The events are not tracked. Click <bold>Details</> for more details.");
        }
        else {
            sprintf(descriptionBuffer_,
                "<bold>%s</> the PC when the user is idle for <bold>%u</> minutes in a time interval ['<bold>%s</>']"
                , "Shutdown", rand() % 400, "23:12 - 05:12 Monday, ..."
            );

            act.descLabel_.caption(descriptionBuffer_);
        }
    }
}


void DashboardForm::actionEventHandle(ActionGroup& act)
{
    act.detailButton_.events().click([&]()
    {
        DetailsForm confForm(*this, *act.detailsConfig_);
        confForm.show();
        confForm.modality();
        updateActionDescription(act);
    });


    act.disableButton_.events().click([&]()
    {
        if (act.disableButton_.pushed()) {
            dashboardConfig_.activeDetails_.disableDetail(act.number_);
        }
        else {
            dashboardConfig_.activeDetails_.enableDetail(act.number_);
        }

        dashboardConfig_.save();

        updateActionDescription(act);
    });
}
