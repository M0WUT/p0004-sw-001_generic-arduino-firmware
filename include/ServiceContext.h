#ifndef SERVICECONTEXT_H
#define SERVICECONTEXT_H

#include "TimeHelper.h"
#include "LoggingHelper.h"
#include "EepromHelper.h"

class ServiceHelper;

struct ServiceContext
{
    TimeHelper *timeHelper;
    LoggingHelper *loggingHelper;
    EepromHelper *eepromHelper;
    ServiceHelper *serviceTracker;
};

#endif // SERVICECONTEXT_H
