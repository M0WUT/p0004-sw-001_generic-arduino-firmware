#ifndef SERVICE_H
#define SERVICE_H

#include <Arduino.h>
#include <enum.h>
#include "debug.h"
#include "eepromHandler.hpp"

// Definitions here to exact numbers are important and must not be changed
// List must be continuous 0 -> N. Additional services must take out a number in this Enum
// This Enum is expected to be bounded to fit in a uint8_t elsewhere -if I've written more than
// 256 services, this has gone much better (or worse!) than planned
BETTER_ENUM(ServiceID, uint8_t, CORE = 0, POWER, THERMAL, LCD, ETHERNET, USB_HOST, USB_DEVICE, SD_CARD, UI, SCPI, HOME_ASSISTANT)

typedef struct ServiceContext
{
    EepromHandler *eepromHandler;
} ServiceContext;

class Service
{
public:
    Service(ServiceID serviceID);
    void initialise(ServiceContext *ctx);
    void tick();

private:
    ServiceID _serviceID;
    ServiceContext *_ctx;
    void _debug_print_service_id();
    void _debug_print(const char *str);
    void _debug_printf(const char *fmt, ...);
    void _debug_println(const char *str);
};

class ServiceTracker
{
public:
    ServiceTracker(Service *extraServiceList, size_t len);
    void initialiseServices(ServiceContext *ctx);

private:
    Service _coreService = Service(ServiceID::CORE);
    Service _powerService = Service(ServiceID::POWER);
    Service _thermalService = Service(ServiceID::THERMAL);
    Service _lcdService = Service(ServiceID::LCD);
    Service _ethernetService = Service(ServiceID::ETHERNET);
    Service **_serviceList; // List containing pointers to all running services
    int _numServices;       // We'll iterate over the _serviceList many times. Save index to avoid recalculations
};

#endif // SERVICE_H
