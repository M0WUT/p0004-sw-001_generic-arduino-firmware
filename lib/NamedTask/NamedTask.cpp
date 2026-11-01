#include "NamedTask.h"

NamedTask::NamedTask()
{
    ;
}

void NamedTask::_error(const char *errorStr)
{
    _getLoggingHelper()->error(get_name(), errorStr);
}

void NamedTask::_errorf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vsnprintf(_printBuffer, PRINT_BUFFER_SIZE, fmt, args);
    va_end(args);
    _getLoggingHelper()->error(get_name(), _printBuffer);
}

int NamedTask::_warning(const char *warningStr)
{
    return _getLoggingHelper()->warning(get_name(), warningStr);
}

int NamedTask::_warningf(const char *fmt, ...)
{
    int warningId;
    va_list args;
    va_start(args, fmt);
    vsnprintf(_printBuffer, PRINT_BUFFER_SIZE, fmt, args);
    va_end(args);
    return _getLoggingHelper()->warning(get_name(), _printBuffer);
}

void NamedTask::_info(const char *infoStr)
{
    _getLoggingHelper()->info(get_name(), infoStr);
}

void NamedTask::_infof(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vsnprintf(_printBuffer, PRINT_BUFFER_SIZE, fmt, args);
    va_end(args);
    _getLoggingHelper()->info(get_name(), _printBuffer);
}

void NamedTask::_debug(const char *debugStr)
{
    _getLoggingHelper()->debug(get_name(), debugStr);
}

void NamedTask::_debugf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vsnprintf(_printBuffer, PRINT_BUFFER_SIZE, fmt, args);
    va_end(args);
    _getLoggingHelper()->debug(get_name(), _printBuffer);
}
