#ifndef BASESERVICE_H
#define BASESERVICE_H

#include <Arduino.h>
#include <variant>
#include "ServiceId.h"
#include "ServiceContext.h"
#include "EepromStruct.h"
#include "NamedTask.h"

constexpr int SERVICE_PRINT_BUFFER_SIZE = 128 * sizeof(char);

class BaseService : public NamedTask
{
public:
    BaseService(ServiceId serviceId);
    virtual void initialise(ServiceContext *ctx) = 0;
    virtual void tick();

protected:
    ServiceId _serviceId;
    ServiceContext *_ctx;
    void initialise_service(ServiceContext *ctx);
    EepromStruct _eepromStruct;
    int load_eeprom_data();

private:
    LoggingHelper *_getLoggingHelper();
    const char *_getName();
};

#endif // BASESERVICE_H
