#include "debug.h"

StatusMonitor::StatusMonitor() : serial(gpio_uart_debug_rx, gpio_uart_debug_tx)
{
    _rtc = &STM32RTC::getInstance();
    _rtc->begin(HOUR_FORMAT_24);

#ifdef DEBUG
    serial.begin(UART_DEBUG_BAUD);
    do
    {
        delay(1000);
    } while (!serial);
    DEBUG_PRINTLN("Started");
#endif
}

void StatusMonitor::print_time()
{
    serial.printf("%04d-%02d-%02dT%02d:%02d:%02d\t", _rtc_year_to_calendar_year(_rtc->getYear()), _rtc->getMonth(), _rtc->getDay(), _rtc->getHours(), _rtc->getMinutes(), _rtc->getSeconds());
}

uint8_t StatusMonitor::_calendar_year_to_rtc_year(uint16_t year)
{
    return (uint8_t)(year - _year_epoch_conversion);
}

uint16_t StatusMonitor::_rtc_year_to_calendar_year(uint8_t year)
{
    return (uint16_t)year + _year_epoch_conversion;
}
