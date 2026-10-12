#ifndef SERVICECONTEXT_H
#define SERVICECONTEXT_H

#include "TimeHelper.h"
#include "DebugUartHelper.h"
#include "EepromHelper.h"
#include "StatusHelper.h"

struct ServiceContext
{
    TimeHelper *timeHelper;
    DebugUartHelper *debugUartHelper;
    EepromHelper *eepromHelper;
    StatusHelper *statusHelper;
};

#endif // SERVICECONTEXT_H
