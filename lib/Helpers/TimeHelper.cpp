#include "TimeHelper.h"

TimeHelper::TimeHelper()
{
    _rtc = &STM32RTC::getInstance();
    _rtc->begin();
}

void TimeHelper::print_time(Stream *outputStream)
{
    outputStream->printf("%04d-%02d-%02dT%02d:%02d:%02d\t", _rtc_year_to_calendar_year(_rtc->getYear()), _rtc->getMonth(), _rtc->getDay(), _rtc->getHours(), _rtc->getMinutes(), _rtc->getSeconds());
}

uint8_t TimeHelper::_calendar_year_to_rtc_year(uint16_t year)
{
    return (uint8_t)(year - _year_epoch_conversion);
}

uint16_t TimeHelper::_rtc_year_to_calendar_year(uint8_t year)
{
    return (uint16_t)year + _year_epoch_conversion;
}
