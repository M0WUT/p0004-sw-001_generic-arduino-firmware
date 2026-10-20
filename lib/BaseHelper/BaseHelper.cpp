#include "BaseHelper.h"

BaseHelper::BaseHelper(HelperContext *ctx, const String helperName) : _helperName(helperName)
{
    _ctx = ctx;
}

LoggingHelper *BaseHelper::_getLoggingHelper()
{
    return _ctx->loggingHelper;
}

const char *BaseHelper::_getName()
{
    return _helperName.c_str();
}