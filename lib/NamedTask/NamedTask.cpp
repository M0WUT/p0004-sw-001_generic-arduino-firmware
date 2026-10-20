#include "NamedTask.h"

NamedTask::NamedTask()
{
    ;
}

void NamedTask::_error(const char *errorStr)
{
    _getLoggingHelper()->error(_getName(), errorStr);
}

void NamedTask::_errorf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    snprintf(_printBuffer, PRINT_BUFFER_SIZE, fmt, args);
    _getLoggingHelper()->error(_getName(), _printBuffer);
}

int NamedTask::_warning(const char *warningStr)
{
    return _getLoggingHelper()->warning(_getName(), warningStr);
}

int NamedTask::_warningf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    snprintf(_printBuffer, PRINT_BUFFER_SIZE, fmt, args);
    return _getLoggingHelper()->warning(_getName(), _printBuffer);
}

void NamedTask::_info(const char *infoStr)
{
    _getLoggingHelper()->info(_getName(), infoStr);
}

void NamedTask::_infof(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    snprintf(_printBuffer, PRINT_BUFFER_SIZE, fmt, args);
    _getLoggingHelper()->info(_getName(), _printBuffer);
}

void NamedTask::_debug(const char *debugStr)
{
    _getLoggingHelper()->debug(_getName(), debugStr);
}

void NamedTask::_debugf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    snprintf(_printBuffer, PRINT_BUFFER_SIZE, fmt, args);
    _getLoggingHelper()->debug(_getName(), _printBuffer);
}
