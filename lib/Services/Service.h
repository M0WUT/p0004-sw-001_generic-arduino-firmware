#ifndef SERVICE_H
#define SERVICE_H

#include <Arduino.h>
#include <variant>
#include "ServiceId.h"
#include "ServiceContext.h"
#include "EepromStruct.h"

class Service
{
public:
    Service(ServiceId serviceId);
    virtual void initialise(ServiceContext *ctx);
    virtual void tick();

protected:
    ServiceId _serviceId;
    ServiceContext *_ctx;
    void _debug_print(const char *str);
    void _debug_printf(const char *fmt, ...);
    void _debug_println(const char *str);
    std::variant<EthernetServiceEepromStruct> _eepromStruct;
    int load_eeprom_data();

private:
    char *_printfBuffer;
    const int _printfBufferSize = 128 * sizeof(char);
};

#endif // SERVICE_H
