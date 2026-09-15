#include "DebugUartHelper.h"

DebugUartHelper::DebugUartHelper(int gpio_uart_tx, int gpio_uart_rx, TimeHelper *timeHelper) : _uart(gpio_uart_rx, gpio_uart_tx), _timeHelper(timeHelper)
{
}

int DebugUartHelper::print(const char *str)
{
#ifdef DEBUG
    _timeHelper->print_time(&_uart);
    return _uart.print(str);
#else
    return 0;
#endif
}

int DebugUartHelper::println(const char *str)
{
#ifdef DEBUG
    _timeHelper->print_time(&_uart);
    return _uart.println(str);
#else
    return 0;
#endif
}

int DebugUartHelper::printf(const char *format, ...)
{
#ifdef DEBUG
    _timeHelper->print_time(&_uart);

    va_list args;
    va_start(args, format);

    int bytesWritten = _uart.printf(format, args);
    va_end(args);
    return bytesWritten;
#else
    return 0;
#endif
}
