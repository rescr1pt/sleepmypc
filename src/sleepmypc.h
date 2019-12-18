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
        bool checkMouseMovement_ = true;

    private:
        std::fstream fs_;
    };

public:
    FaceFrom(nana::window wd, const ::nana::size& sz = { 260, 230 }, const nana::appearance& apr = { true, true, false, false, true /*min*/, false, false });

    ~FaceFrom();

    void showWarning(bool val);
    bool isShowWarning() const;

    void updateProgState(bool active);
    void updateWarningCaption(size_t remainingTime);

    void processTriggeredAction();

    size_t getConfInactive() const { return config_.inactive_; }
    size_t getConfWarn() const { return config_.warn_; }
    bool actionIsNone() const { return config_.action_ == EAction::No; }
    bool isCheckMouseMovement() const { return config_.checkMouseMovement_; }

private:
    void init_();

    void updateConfigState();
    void updateWarnSpinboxRange();

    static std::string& getProgActiveVal();
    static std::string& getProgIdleVal();

protected:
    nana::place place_ { *this };
    nana::label wComboLab_;
    nana::combox wCombo_;
    nana::label wLabIdleTimer_;
    nana::spinbox wSpinIdle_;
    nana::label wLabWarnTimer_;
    nana::spinbox wSpinWarn_;
    nana::label wLabProgDesc_;
    nana::label wLabProgVal_;
    nana::button wButtOptions_;
    nana::button wButtHistory_;
    nana::panel<true> wEmptyPlace;
    //nana::panel<true> wPlace2_;
    //nana::panel<true> wPlace3_;
    nana::label wLabCop_;
    nana::checkbox wCheckMouseMove_;
    nana::button wButtSave_;
    std::unique_ptr<HistoryForm> logForm_;
    std::unique_ptr<WarnForm> warnForm_;
    std::unique_ptr<NoticeForm> noticeForm_;

    Config config_;
};


#endif