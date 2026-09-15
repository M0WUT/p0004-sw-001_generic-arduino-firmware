#include "TimeHelper.h"

TimeHelper::TimeHelper()
{
    _rtc = &STM32RTC::getInstance();
    _rtc->begin(HOUR_FORMAT_24);
}

void TimeHelper::print_time(Stream *outputStream)
{
    outputStream->printf("%04d-%02d-%02dT%02d:%02d:%02d\t", _rtc_year_to_calendar_year(_rtc->getYear()), _rtc->getMonth(), _rtc->getDay(), _rtc->getHours(), _rtc->getMinutes(), _rtc->getSeconds());
}
