/*
   author: rescr1pt@.ya.ru
*/

#ifndef SLEEPMYPC_DASHBOARD_FORM
#define SLEEPMYPC_DASHBOARD_FORM

#include <nana/gui.hpp>
#include <nana/gui/place.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/group.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/textbox.hpp>

#include "DetailsConfig.h"
#include "DashboardConfig.h"


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
    nana::textbox logTextBox_{ *this };
    nana::button clearButton_{ *this, "Clear" };

    std::fstream fs_;
};

class WarnForm : public nana::form
{
public:
    WarnForm(nana::window wd, EAction action);
    ~WarnForm();

    void updateWarningCaption(size_t remainingTime);

private:
    void init_();


protected:
    EAction action_;
    nana::place place_{ *this };
    nana::label messageLabel_;
};

class DashboardForm : public nana::form
{
private:
    struct ActionGroup
    {
        unsigned char number_ = 0;
        std::unique_ptr<DetailsConfig> detailsConfig_;
        nana::group group_;
        nana::label descLabel_;
        nana::button detailButton_;
        nana::button disableButton_;
    };

public:
    enum class ECurrentStatus
    {
        Disabled,
        Active,
        Inactive
    };

    struct ObservedConfig
    {
        unsigned char triggerActionIndex_ = 0;
        size_t triggerActionWhendleSecs_ = 0;
        unsigned char warnActionIndex_ = 0;
        size_t warnActionWhenIdleSecs_ = 0;
    };

public:
    DashboardForm(nana::window wd);
    ~DashboardForm();

    const bool findObservedConfig(unsigned char day, unsigned char hour, unsigned char minutes, ObservedConfig& observedConfig) const;

    void updateWarningCaption(size_t remainingTime);

    void showWarning(bool val);
    void processTriggeredAction(unsigned char i);
    void updateProgState(ECurrentStatus currentStatus);
    void setNearDetail(size_t detailIndex);
    void resetNearDetail();

private:
    void init_();
    void updateActionDescription(ActionGroup& act);
    void actionEventHandle(ActionGroup& act);

protected:
    DashboardConfig dashboardConfig_;

    nana::place place_{ *this };

    ActionGroup actions_[DASHOBARD_DETAIL_COUNT];

    nana::button mouseMoveButton_;
    nana::label progValueLab_;
    nana::button historyButton_;

    std::unique_ptr<HistoryForm> logForm_;
    std::unique_ptr<WarnForm> warnForm_;

    char descriptionBuffer_[512];
};

#endif