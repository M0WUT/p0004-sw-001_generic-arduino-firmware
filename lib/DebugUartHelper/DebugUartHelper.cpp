#include "DebugUartHelper.h"

DebugUartHelper::DebugUartHelper(int gpio_uart_tx, int gpio_uart_rx, TimeHelper *timeHelper) : _uart(gpio_uart_rx, gpio_uart_tx), _timeHelper(timeHelper)
{
#ifdef DEBUG
    _uart.begin(UART_DEBUG_BAUD);
    do
    {
        delay(1000);
    } while (!_uart);
    this->println("Started");
#endif
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
    va_list args;
    char buffer[256];

    _timeHelper->print_time(&_uart);
    va_start(args, format);
    int retval = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    _uart.print(buffer);
    return retval;
#else
    return 0;
#endif
}
