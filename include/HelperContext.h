#ifndef HELPERCONTEXT_H
#define HELPERCONTEXT_H

#include "TimeHelper.h"
#include "LoggingHelper.h"

struct HelperContext
{
    TimeHelper *timeHelper;
    LoggingHelper *loggingHelper;
};

#endif // HELPERCONTEXT_H
