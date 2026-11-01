#ifndef SERVICECONTEXT_H
#define SERVICECONTEXT_H

#include "TimeHelper.h"
#include "LoggingHelper.h"
#include "EepromHelper.h"

// Forward declaration
class ServiceHelper;

struct ServiceContext
{
    TimeHelper *timeHelper;
    LoggingHelper *loggingHelper;
    EepromHelper *eepromHelper;
    ServiceHelper *serviceHelper;
};

#endif // SERVICECONTEXT_H
