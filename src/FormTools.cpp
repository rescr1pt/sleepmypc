#include "FormTools.h"

#include <nana/gui/place.hpp>
#include <nana/gui/widgets/label.hpp>

#include <Windows.h>

class NoticeForm : public nana::form
{
public:
    enum class Action
    {
        DebugTriggeredAction,
        TriggeredInvalidAction,
    };
public:
    NoticeForm(const std::string& message, nana::window wd, const ::nana::size& sz = { 340, 200 }, const nana::appearance& apr = { true, true, false, false, false, false, false });
    ~NoticeForm();

private:
    void init_();


protected:
    std::string message_;
    nana::place place_{ *this };
    nana::label messageLab_;
};


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
    place_.div("margin=5 <margin=5 gap=2 field1>");
    caption("sleep my PC - notice");

    // wLabel
    messageLab_.create(*this);
    place_["field1"] << messageLab_;
    messageLab_.typeface(nana::paint::font("", 12, { 400, false, false, false }));
    messageLab_.transparent(true);
    messageLab_.text_align(static_cast<nana::align>(1), static_cast<nana::align_v>(1));

    messageLab_.caption(message_);

    place_.collocate();
}

std::wstring& formtools::getAppPath()
{
    struct Result
    {
        explicit Result()
        {
            appPath_.resize(1024);

            size_t applicationFilePath = (size_t)GetModuleFileNameW(0, &appPath_.front(), (DWORD)appPath_.size());
            if (applicationFilePath > 0) {
                appPath_.resize(applicationFilePath);
            }
            else {
                appPath_.clear();
            }
        }

        std::wstring appPath_;
    };

    static Result result;
    return result.appPath_;
}

void formtools::showNotice(const std::string& msg, nana::window wd)
{
    NoticeForm noticeForm(msg, wd);
    noticeForm.show();
    noticeForm.modality();
}

void formtools::initIco(nana::form& form)
{
    auto& appPath = formtools::getAppPath();
    if (!appPath.empty()) {
        form.icon(nana::paint::image(formtools::getAppPath()));
    }
    else {
        showNotice("Unable to set icon to the Form", form);
    }
}
