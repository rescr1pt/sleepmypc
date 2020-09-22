/*
   author: rescr1pt@.ya.ru
*/


#ifndef SLEEPMYPC_ACTION_ENUMS
#define SLEEPMYPC_ACTION_ENUMS

enum class EAction : size_t
{
    No,
    Shutdown,
    Restart,
    Logoff,
    Hibernation
};

static const char* getActionName(EAction action)
{
    switch (action)
    {
        case EAction::No:
            return "No";
        case EAction::Shutdown:
            return "Shutdown";
        case EAction::Restart:
            return "Restart";
        case EAction::Logoff:
            return "Logoff";
        case EAction::Hibernation:
            return "Hibernation";
        default:
            break;
    }

    return "";
}

#endif