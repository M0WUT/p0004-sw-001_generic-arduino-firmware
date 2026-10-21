#include "LoggingHelper.h"

LoggingHelper::LoggingHelper(int gpio_uart_tx, int gpio_uart_rx, TimeHelper *timeHelper, int gpioLedRed, int gpioLedGreen, int gpioLedBlue, LoggingLevel loggingLevel) : _uart(gpio_uart_rx, gpio_uart_tx)
{

    delay(2000);
    _uart.begin(UART_DEBUG_BAUD);
    _uart.print("\r\n\
 ██████╗████████╗███████╗\r\n\
██╔════╝╚══██╔══╝██╔════╝\r\n\
██║  ███╗  ██║   █████╗  \r\n\
██║   ██║  ██║   ██╔══╝  \r\n\
╚██████╔╝  ██║   ███████╗\r\n\
 ╚═════╝   ╚═╝   ╚══════╝  By MØWUT\r\n\
");
    _uart.printf("Version: %s (%s)\r\n\r\n", VERSION_STRING, BUILD_TIMESTAMP);
    _timeHelper = timeHelper;
    _gpioLedRed = gpioLedRed;
    _gpioLedGreen = gpioLedGreen;
    _gpioLedBlue = gpioLedBlue;
    _loggingLevel = loggingLevel;
}

void LoggingHelper::error(const char *serviceName, const char *errorStr)
{
    while (1)
    {
        static bool x = 0;
        _printStatusMessage(serviceName, errorStr, LoggingLevel::ERROR);
        x = !x;
        _setLedColour(x ? LED_RED : LED_OFF);
        delay(1000);
    }
}

int LoggingHelper::warning(const char *serviceName, const char *warningStr)
{
    int warningId = _getWarningId();
    _printStatusMessage(serviceName, warningStr, LoggingLevel::WARNING);
    _warningList.push_back(warning_t{warningId, warningStr});
    _setLedColour(LED_ORANGE);
    if (_warningList.size() > MAX_WARNINGS_BEFORE_ERROR)
    {
        error("Logging", "Too many warnings. Giving up");
    }
    return warningId;
}

void LoggingHelper::info(const char *serviceName, const char *infoStr)
{
    _printStatusMessage(serviceName, infoStr, LoggingLevel::INFO);
}

void LoggingHelper::debug(const char *serviceName, const char *debugStr)
{
    _printStatusMessage(serviceName, debugStr, LoggingLevel::DEBUG);
}

void LoggingHelper::clear_warning(int warningId)
{
    for (int i = 0; i < _warningList.size(); i++)
    {
        if (_warningList[i].id == warningId)
        {
            _warningList.erase(_warningList.begin() + i);
            if (_warningList.size() == 0)
            {
                _setLedColour(LED_OFF);
            }
            return;
        }
    }
    error("Logging", "Attempted to remove unknown (or previously cleared) warning");
}

void LoggingHelper::_print_timestamp()
{
    _timeHelper->print_time(&_uart);
}

void LoggingHelper::_setLedColour(ledColour_t colour)
{
    // LED is active low
    digitalWrite(_gpioLedRed, !colour.red);
    digitalWrite(_gpioLedGreen, !colour.green);
    digitalWrite(_gpioLedBlue, !colour.blue);
}

int LoggingHelper::_getWarningId()
{
    return ++_lastWarningId;
}

void LoggingHelper::_printStatusMessage(const char *serviceName, const char *messageStr, LoggingLevel loggingLevel)
{
    if (loggingLevel < _loggingLevel)
    {
        return;
    }

    String colourStr = "";
    switch (loggingLevel)
    {
    case LoggingLevel::ERROR:
        // Red
        colourStr = "\x1b[31m";
        break;
    case LoggingLevel::WARNING:
        // Yellow
        colourStr = "\x1b[33m";
        break;
    default:
        break;
    };
    _uart.print(colourStr);

    _print_timestamp();
    _uart.print("[");
    _uart.print(serviceName);
    _uart.print("]\t");
    _uart.println(messageStr);
    if (colourStr)
    {
        _uart.print("\x1b[0m");
    }
    _uart.flush();
}
