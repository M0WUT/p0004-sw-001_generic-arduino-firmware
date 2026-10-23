#ifndef SERVICECONTEXT_H
#define SERVICECONTEXT_H

#include "TimeHelper.h"
#include "LoggingHelper.h"
#include "EepromHelper.h"

class TimeHelper;
class LoggingHelper;
class EepromHelper;

struct ServiceContext
{
    TimeHelper *timeHelper;
    LoggingHelper *loggingHelper;
    EepromHelper *eepromHelper;
};

#endif // SERVICECONTEXT_H
