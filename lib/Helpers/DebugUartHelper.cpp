#include "DebugUartHelper.h"

DebugUartHelper::DebugUartHelper(int gpio_uart_tx, int gpio_uart_rx, TimeHelper *timeHelper) : _uart(gpio_uart_rx, gpio_uart_tx), _timeHelper(timeHelper)
{
#ifdef DEBUG
    _printfBuffer = (char *)malloc(_printfBufferSize);
    _uart.begin(UART_DEBUG_BAUD);
    do
    {
        delay(1000);
    } while (!_uart);
    _uart.print("\n\
 ██████╗████████╗███████╗\n\
██╔════╝╚══██╔══╝██╔════╝\n\
██║  ███╗  ██║   █████╗  \n\
██║   ██║  ██║   ██╔══╝  \n\
╚██████╔╝  ██║   ███████╗\n\
 ╚═════╝   ╚═╝   ╚══════╝  By MØWUT\n\n\
");
#endif
}

int DebugUartHelper::print(const char *str)
{
#ifdef DEBUG
    print_timestamp();
    return _uart.print(str);
#else
    return 0;
#endif
}

int DebugUartHelper::println(const char *str)
{
#ifdef DEBUG
    print_timestamp();
    return _uart.println(str);
#else
    return 0;
#endif
}

int DebugUartHelper::printf(const char *format, ...)
{
#ifdef DEBUG
    va_list args;
    print_timestamp();
    va_start(args, format);
    int retval = vsnprintf(_printfBuffer, _printfBufferSize, format, args);
    va_end(args);
    _uart.print(_printfBuffer);
    return retval;
#else
    return 0;
#endif
}

void DebugUartHelper::print_timestamp()
{
    _timeHelper->print_time(&_uart);
}
