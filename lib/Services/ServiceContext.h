#ifndef SERVICECONTEXT_H
#define SERVICECONTEXT_H

#include "TimeHelper.h"
#include "DebugUartHelper.h"
#include "EepromHelper.h"

struct ServiceContext
{
    TimeHelper *timeHelper;
    DebugUartHelper *debugUartHelper;
    EepromHelper *eepromHelper;
};

#endif // SERVICECONTEXT_H
