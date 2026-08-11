#ifndef DEBUG_H
#define DEBUG_H

#include <Arduino.h>
#include <STM32RTC.h>

#define DEBUG

// All nasty macro hacking but DEBUG_PRINT is a bit too magic
// to manually implement a function with all the right type definitions
#ifdef DEBUG
#include "pinout.h"
#define DEBUG_PRINT(...)        \
    statusMonitor.print_time(); \
    statusMonitor.serial.print(__VA_ARGS__)
#define DEBUG_PRINTF(...)       \
    statusMonitor.print_time(); \
    statusMonitor.serial.printf(__VA_ARGS__)
#define DEBUG_PRINTLN(...)      \
    statusMonitor.print_time(); \
    statusMonitor.serial.println(__VA_ARGS__)
#else
#define DEBUG_PRINT(...) // now defines a blank line
#define DEBUG_PRINTF(...)
#define DEBUG_PRINTLN(...)
#endif

class StatusMonitor
{
public:
    StatusMonitor();
    void print_time();
    HardwareSerial serial;

private:
    STM32RTC *_rtc;
    bool _synced = false;
    uint8_t _calendar_year_to_rtc_year(uint16_t year);
    uint16_t _rtc_year_to_calendar_year(uint8_t year);
    // Align with Unix epoch. STM32 RTC year starts at 1 so epoch of
    // 1969 means that time starts on 1/1/1970
    const int _year_epoch_conversion = 1969;
};

extern StatusMonitor statusMonitor;

#endif