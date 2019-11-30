/*
   author: rescr1pt@.ya.ru
*/

#include "sleepmypc.h"

#include <windows.h>

class ExternalEventHooh
{
public:
    ExternalEventHooh(FaceFrom& face) : face_(face)
    {
    }

    void exec()
    {
        static unsigned long cursorAllowable = 32;

        if (face_.getConfInactive() == 0) {
            lastCursorPos_.reset();
            return;
        }

        auto now = SteadyClock::now();

        if (!lastCursorPos_) {
            lastCursorPos_ = std::make_unique<POINT>();
            GetCursorPos(lastCursorPos_.get());
            idleTimer_ = now;
            progStateTimer_ = now;
        }

        POINT cursorPos;
        GetCursorPos(&cursorPos);

        const unsigned long offsetX = abs(cursorPos.x - lastCursorPos_->x);
        const unsigned long offsetY = abs(cursorPos.y - lastCursorPos_->y);
        const bool mouseIsActive = (offsetX > cursorAllowable || offsetY > cursorAllowable);

        // Active
        if (mouseIsActive) {
            *lastCursorPos_ = cursorPos;
            face_.showWarning(false);
            face_.updateProgState(true);

            // Wakeup
            idleTimer_ = now;
            progStateTimer_ = now;
        }
        // Idle
        else {
            auto idleTimerDiff = std::chrono::duration_cast<std::chrono::seconds>(now - idleTimer_);
            const size_t maxInactiveSeconds = face_.getConfInactive() * 60;
                        
            if ((size_t)idleTimerDiff.count() >= maxInactiveSeconds) {
                face_.processTriggeredAction();

                // Cannot restart
                idleTimer_ = now;
            }

            if (face_.getConfWarn() > 0) {
                size_t remainingSecs = maxInactiveSeconds - idleTimerDiff.count();

                if (remainingSecs < face_.getConfWarn()) {
                    if (!face_.isShowWarning()) {
                        face_.showWarning(true);
                    }

                    face_.updateWarningCaption(remainingSecs);
                }
            }
            
            auto progimerDiff = std::chrono::duration_cast<std::chrono::seconds>(now - progStateTimer_);
            if (progimerDiff.count() > 2) {
                face_.updateProgState(false);
                progStateTimer_ = now;
            }
        }
    }

private:
    FaceFrom& face_;
    std::unique_ptr<POINT> lastCursorPos_;
    SteadyClock::time_point idleTimer_;
    SteadyClock::time_point progStateTimer_;

    bool showWarn_ = false;
};

int WINAPI WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nShowCmd*/) 
{
    FaceFrom face(0);
    ExternalEventHooh timing(face);

    nana::timer timer{ std::chrono::milliseconds{500} };
    timer.elapse([&timing] {
        timing.exec();
    });
    timer.start();

    face.show();
    nana::exec();
    timer.stop();

    return 0;
}
