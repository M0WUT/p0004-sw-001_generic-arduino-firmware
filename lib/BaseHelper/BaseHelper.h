#ifndef BASEHELPER_H
#define BASEHELPER_H

#include "HelperContext.h"
#include "NamedTask.h"

constexpr int HELPER_PRINT_BUFFER_SIZE = 128 * sizeof(char);

class BaseHelper : public NamedTask
{
public:
    BaseHelper(HelperContext *ctx, const String helperName);

private:
    HelperContext *_ctx;
    const String _helperName;
    LoggingHelper *_getLoggingHelper();
    const char *_getName();
};

#endif // BASEHELPER_H
