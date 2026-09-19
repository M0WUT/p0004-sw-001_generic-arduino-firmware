

#ifndef TIMEHELPER_H
#define TIMEHELPER_H

#include <STM32RTC.h>

class TimeHelper
{
public:
    TimeHelper();
    void print_time(Uart *outputStream);

private:
    STM32RTC *_rtc;
    bool _synced = false;
    uint8_t _calendar_year_to_rtc_year(uint16_t year);
    uint16_t _rtc_year_to_calendar_year(uint8_t year);
    // Align with Unix epoch. STM32 RTC year starts at 1 so epoch of
    // 1969 means that time starts on 1/1/1970
    const int _year_epoch_conversion = 1969;
};

#endif // TIMEHELPER_H
