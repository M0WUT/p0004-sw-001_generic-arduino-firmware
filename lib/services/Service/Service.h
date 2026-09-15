#ifndef SERVICE_H
#define SERVICE_H

#include <Arduino.h>
#include <enum.h>

#include "helpers/TimeHelper/TimeHelper.h"
#include "helpers/DebugUartHelper/DebugUartHelper.h"
#include "helpers/EepromHelper/EepromHelper.h"

// Definitions here to exact numbers are important and must not be changed
// List must be continuous 0 -> N. Additional services must take out a number in this Enum
// This Enum is expected to be bounded to fit in a uint8_t elsewhere -if I've written more than
// 256 services, this has gone much better (or worse!) than planned
BETTER_ENUM(ServiceID, uint8_t, CORE = 0, POWER, THERMAL, LCD, ETHERNET, USB_HOST, USB_DEVICE, SD_CARD, UI, SCPI, HOME_ASSISTANT, APPLICATION)

typedef struct ServiceContext
{
    TimeHelper *timeHelper;
    DebugUartHelper *debugUartHelper;
    EepromHelper *eepromHelper;
} ServiceContext;

class Service
{
public:
    Service(ServiceID serviceID);
    virtual void initialise(ServiceContext *ctx);
    virtual void tick();

protected:
    ServiceID _serviceID;
    ServiceContext *_ctx;
    void _debug_print(const char *str);
    void _debug_printf(const char *fmt, ...);
    void _debug_println(const char *str);
};

#endif // SERVICE_H
