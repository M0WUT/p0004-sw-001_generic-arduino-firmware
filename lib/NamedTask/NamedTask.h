#ifndef NAMEDTASK_H
#define NAMEDTASK_H

#include "LoggingHelper.h"

constexpr int PRINT_BUFFER_SIZE = 128 * sizeof(char);

class NamedTask
{
public:
    NamedTask();

protected:
    char _printBuffer[PRINT_BUFFER_SIZE];
    virtual LoggingHelper *_getLoggingHelper() = 0;
    virtual const char *_getName() = 0;

    void _error(const char *errorStr);
    void _errorf(const char *fmt, ...);
    // Returns unique id of the warning so it can be cleared later
    int _warning(const char *warningStr);
    int _warningf(const char *fmt, ...);
    void _info(const char *infoStr);
    void _infof(const char *fmt, ...);
    void _debug(const char *debugStr);
    void _debugf(const char *fmt, ...);
};

#endif // NAMEDTASK_H
