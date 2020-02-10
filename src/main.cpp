/*
   author: rescr1pt@.ya.ru
*/

#include "sleepmypc.h"

#include <windows.h>

class ExternalEventHook
{
public:
    ExternalEventHook() 
    {
    }

    ~ExternalEventHook()
    {
        if (mouseHook_) {
            UnhookWindowsHookEx(mouseHook_);
        }

        if (keyboardHook_) {
            UnhookWindowsHookEx(keyboardHook_);
        }
    }

    static ExternalEventHook& instance()
    {
        static ExternalEventHook me;
        return me;
    }

    void init(FaceFrom& face, HINSTANCE hInstance)
    {
        face_ = &face;
        hInstance_ = hInstance;
    }

    void execTimer()
    {
        bool isInTimeInterval = true;

        if (!face_->isAnyTimeInterval()) {
            time_t currentTime;
            struct tm* localTime;
            time(&currentTime);
            localTime = localtime(&currentTime);

            const auto& interval = face_->getTimeInterval();
            isInTimeInterval = interval.isInInterval((unsigned short)localTime->tm_hour, (unsigned short)localTime->tm_min);
        }

        // Disabled
        if (!isInTimeInterval || face_->getConfInactive() == 0 || face_->actionIsNone()) {
            if (wasEnabled_) {
                wasEnabled_ = false;

                face_->updateProgState(FaceFrom::ECurrentStatus::Disabled);

                UnhookWindowsHookEx(mouseHook_);
                UnhookWindowsHookEx(keyboardHook_);
            }

            return;
        }

        const static size_t inactiveCursorMovePerSec = 10;

        auto now = SteadyClock::now();

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

            mouseInfo_.reset(now);
            prevIsActive_ = true;

            idleTimer_ = now;
            progStateTimer_ = now;
        }

        // User active detector
        bool nextActive;

        if (prevIsActive_) {
            nextActive = true;

            if (face_->isCheckMouseMovement()) {
                if (nextActive && mouseInfo_.movedCount_ == 0) {
                    nextActive = false;
                }
            }

            if (nextActive && !mouseInfo_.wasAction_) {
                nextActive = false;
            }

            if (nextActive && keyboardInfo_.pressed_) {
                nextActive = true;
            }

            mouseInfo_.reset(now);
            keyboardInfo_.reset();
        }
        else {
            nextActive = false;

            // Mouse move event
            if (face_->isCheckMouseMovement()) {
                auto idleTimerDiff = std::chrono::duration_cast<std::chrono::milliseconds>(now - mouseInfo_.lastMoveTimer_);
                if (!nextActive && idleTimerDiff.count() > 500) {
                    if ((mouseInfo_.movedCount_ > inactiveCursorMovePerSec)) {
                        nextActive = true;
                    }
                    mouseInfo_.updateTimer(now);
                }
            }

            if (!nextActive && mouseInfo_.wasAction_) {
                nextActive = true;
            }

            // Keyboard event 
            if (!nextActive && keyboardInfo_.pressed_) {
                nextActive = true;
            }

            if (nextActive) {
                mouseInfo_.reset(now);
                keyboardInfo_.reset();
            }
        }

        // Active
        if (nextActive) {
            // Wakeup
            idleTimer_ = now;
            progStateTimer_ = now;

            if (!prevIsActive_) {
                face_->showWarning(false);
                face_->updateProgState(FaceFrom::ECurrentStatus::Active);
            }
        }
        // Idle
        else {
            auto idleTimerDiff = std::chrono::duration_cast<std::chrono::seconds>(now - idleTimer_);
            const size_t maxInactiveSeconds = face_->getConfInactive() * 60;
                        
            if ((size_t)idleTimerDiff.count() >= maxInactiveSeconds) {
                face_->processTriggeredAction();
                face_->close();
                return;
            }

            // Warning window processing
            if (face_->getConfWarn() > 0) {
                size_t remainingSecs = maxInactiveSeconds - idleTimerDiff.count();

                if (remainingSecs < face_->getConfWarn()) {
                    if (!face_->isShowWarning()) {
                        face_->showWarning(true);
                    }

                    face_->updateWarningCaption(remainingSecs);
                }
            }
            
            // Progress label processing 
            auto progTimerDiff = std::chrono::duration_cast<std::chrono::seconds>(now - progStateTimer_);
            if (progTimerDiff.count() > 2) {
                face_->updateProgState(FaceFrom::ECurrentStatus::Inactive);
                progStateTimer_ = now;
            }
        }

        prevIsActive_ = nextActive;
    }

private:
    static LRESULT CALLBACK mouseHookProcess(int nCode, WPARAM wParam, LPARAM lParam)
    {
        auto& mouseInfo = instance().mouseInfo_;

        switch (wParam)
        {
            case WM_MOUSEMOVE:
            {
                ++mouseInfo.movedCount_;
      
                break;
            }
            default:
            {
                mouseInfo.wasAction_ = true;
                break;
            }
        }

        return CallNextHookEx(NULL, nCode, wParam, lParam);
    }
    
    static LRESULT CALLBACK keyboardHookProcess(int nCode, WPARAM wParam, LPARAM lParam)
    {
        auto& keyboardInfo = instance().keyboardInfo_;

        switch (wParam)
        {
            case WM_KEYDOWN:
            {
                keyboardInfo.pressed_ = true;

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
    struct MouseInfo
    {
        size_t movedCount_ = 0;
        bool wasAction_ = false;

        SteadyClock::time_point lastMoveTimer_;

        void updateTimer(const SteadyClock::time_point& time)
        {
            lastMoveTimer_ = time;
        }

        void reset(const SteadyClock::time_point& time)
        {
            wasAction_ = false;
            movedCount_ = 0;
            updateTimer(time);
        }
    } mouseInfo_;

    struct KeyboardInfo
    {
        bool pressed_ = false;

        void reset()
        {
            pressed_ = false;
        }
    } keyboardInfo_;

private:
    FaceFrom* face_ = nullptr;
    HINSTANCE hInstance_ = nullptr;

    HHOOK mouseHook_ = nullptr;
    HHOOK keyboardHook_ = nullptr;

    SteadyClock::time_point idleTimer_;
    SteadyClock::time_point progStateTimer_;

    bool wasEnabled_ = false;
    bool prevIsActive_ = true;
    bool showWarn_ = false;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nShowCmd*/)
{
    HANDLE duplicateInstanceMutex = CreateMutex(NULL, TRUE, L"rescr1pt_sleepmypc");
    if (duplicateInstanceMutex == NULL || ERROR_ALREADY_EXISTS == GetLastError()) {
        return 1;
    }

    FaceFrom face(0);

    auto& externalEventHook = ExternalEventHook::instance();
    externalEventHook.init(face, hInstance);

    nana::timer timer{ std::chrono::milliseconds{250} };
    timer.elapse([&externalEventHook] {
        externalEventHook.execTimer();
    });
    timer.start();

    face.show();

    nana::exec();
    timer.stop();

    ReleaseMutex(duplicateInstanceMutex);
    CloseHandle(duplicateInstanceMutex);

    return 0;
}
