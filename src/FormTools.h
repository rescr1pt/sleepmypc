/*
   author: rescr1pt@.ya.ru
*/

#ifndef SLEEPMYPC_FORM_TOOLS
#define SLEEPMYPC_FORM_TOOLS

#include <string>
#include <nana/gui.hpp>

namespace formtools
{
    static std::wstring& getAppPath();
    void showNotice(const std::string& msg, nana::window wd);
    void initIco(nana::form& form);
}

#endif