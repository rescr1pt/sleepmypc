/*
   author: rescr1pt@.ya.ru
*/

#include "DashboardForm.h"

#include <nana/gui/timer.hpp>

#include <windows.h>

typedef std::chrono::steady_clock SteadyClock;

class UserEventEngine
{
public:
    UserEventEngine() 
    {
    }

    ~UserEventEngine()
    {
        if (mouseHook_) {
            UnhookWindowsHookEx(mouseHook_);
        }

        if (keyboardHook_) {
            UnhookWindowsHookEx(keyboardHook_);
        }
    }

    static UserEventEngine& instance()
    {
        static UserEventEngine me;
        return me;
    }

    void init(DashboardForm& dashboard, HINSTANCE hInstance)
    {
        dashboard_ = &dashboard;
        hInstance_ = hInstance;
    }

    void disable()
    {
        if (wasEnabled_) {
            wasEnabled_ = false;

            UnhookWindowsHookEx(mouseHook_);
            UnhookWindowsHookEx(keyboardHook_);
        }
    }

    void execStep()
    {
        time_t currentTime;
        struct tm* localTime;
        time(&currentTime);
        localTime = localtime(&currentTime);

        bool foundObservedConfig = dashboard_->findObservedConfig((unsigned char)localTime->tm_wday - 1, (unsigned char)localTime->tm_hour, (unsigned char)localTime->tm_min, observedConfig_);

        if (!foundObservedConfig) {
            disable();
            return;
        }

        auto now = SteadyClock::now();

        // Reset hooks and timers
        if (!wasEnabled_) {
            wasEnabled_ = true;

            /// Activate key hooks
            mouseHook_ = SetWindowsHookEx(
                WH_MOUSE_LL,
                mouseHookProcess,
                hInstance_,
                0);

            keyboardHook_ = SetWindowsHookEx(
                WH_KEYBOARD_LL,
                keyboardHookProcess,
                hInstance_,
                0);

            lastActionTime_ = now;
        }

        auto idleTimerDiff = std::chrono::duration_cast<std::chrono::seconds>(now - lastActionTime_);

        if ((size_t)idleTimerDiff.count() >= observedConfig_.triggerActionWhendleSecs_) {
            dashboard_->processTriggeredAction(observedConfig_.triggerActionIndex_);
            disable();
            return;
        }

        if ((size_t)idleTimerDiff.count() >= observedConfig_.warnActionWhenIdleSecs_) {
            dashboard_->updateWarningCaption(observedConfig_.warnActionIndex_);
        }

    }

private:
    static LRESULT CALLBACK mouseHookProcess(int nCode, WPARAM wParam, LPARAM lParam)
    {
        switch (wParam)
        {
            case WM_MOUSEMOVE:
            {

                break;
            }
            default:
            {
                instance().lastActionTime_ = SteadyClock::now();
                break;
            }
        }

        return CallNextHookEx(NULL, nCode, wParam, lParam);
    }
    
    static LRESULT CALLBACK keyboardHookProcess(int nCode, WPARAM wParam, LPARAM lParam)
    {
        switch (wParam)
        {
            case WM_KEYDOWN:
            {
                instance().lastActionTime_ = SteadyClock::now();

                break;
            }
            default:
            {
                break;
            }
        }

        return CallNextHookEx(NULL, nCode, wParam, lParam);
    }

private:

private:
    DashboardForm* dashboard_ = nullptr;
    DashboardForm::ObservedConfig observedConfig_;
    HINSTANCE hInstance_ = nullptr;

    HHOOK mouseHook_ = nullptr;
    HHOOK keyboardHook_ = nullptr;

    SteadyClock::time_point lastActionTime_;

    bool wasEnabled_ = false;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nShowCmd*/)
{
    HANDLE duplicateInstanceMutex = CreateMutex(NULL, TRUE, L"rescr1pt_sleepmypc");
    if (duplicateInstanceMutex == NULL || ERROR_ALREADY_EXISTS == GetLastError()) {
        return 1;
    }

    DashboardForm dashboard(0);

    auto& userEventEngine = UserEventEngine::instance();
    userEventEngine.init(dashboard, hInstance);

    nana::timer timer{ std::chrono::milliseconds{250} };
    timer.elapse([&userEventEngine] {
        userEventEngine.execStep();
    });
    timer.start();

    dashboard.show();

    nana::exec();
    timer.stop();
    

    ReleaseMutex(duplicateInstanceMutex);
    CloseHandle(duplicateInstanceMutex);

    return 0;
}