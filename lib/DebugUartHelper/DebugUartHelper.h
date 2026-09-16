#ifndef DEBUGUARTHELPER_H
#define DEBUGUARTHELPER_H

#include <Arduino.h>
#include "TimeHelper.h"
#include "pinout.h"

#define DEBUG

class DebugUartHelper
{
public:
    DebugUartHelper(int gpio_uart_tx, int gpio_uart_rx, TimeHelper *timeHelper);
    int print(const char *str);
    int println(const char *str);
    int printf(const char *format, ...);

private:
    HardwareSerial _uart;
    TimeHelper *_timeHelper;
};

#endif // DEBUGUARTHELPER_H
