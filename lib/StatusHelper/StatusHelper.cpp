#include "StatusHelper.h"

StatusHelper::StatusHelper(DebugUartHelper *uart, int gpioLedRed, int gpioLedGreen, int gpioLedBlue)
{
    _uart = uart;
    _gpioLedRed = gpioLedRed;
    _gpioLedGreen = gpioLedGreen;
    _gpioLedBlue = gpioLedBlue;
    int gpioPins[3] = {_gpioLedRed, _gpioLedGreen, _gpioLedBlue};
    for (int i = 0; i < 3; i++)
    {
        pinMode(gpioPins[i], OUTPUT);
        digitalWrite(gpioPins[i], HIGH);
    }
}

void StatusHelper::register_error(const char *errorStr)
{

    while (1)
    {
        static bool x = 0;
        _print_error(errorStr);
        x = !x;
        _setLedColour(x ? LED_RED : LED_OFF);
        delay(1000);
    }
}

int StatusHelper::register_warning(const char *warningStr)
{
    int warningId = _getWarningId();
    _print_warning(warningStr);
    _warningList.push_back(warning_t{warningId, warningStr});
    _setLedColour(LED_ORANGE);
    if (_warningList.size() > _maxWarnings)
    {
        register_error("Too many warnings. Giving up");
    }
    return warningId;
}

void StatusHelper::clear_warning(int warningId)
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
    register_error("Attempted to remove unknown (or previously cleared) warning");
}

void StatusHelper::register_waiting_for_user(const char *userStr)
{
}

void StatusHelper::_print_error(const char *errorStr)
{
    _printStatusMessage(errorStr, RED);
}

void StatusHelper::_print_warning(const char *warningStr)
{
    _printStatusMessage(warningStr, YELLOW);
}

void StatusHelper::_printStatusMessage(const char *messageStr, TextColour colour)
{
    const String _colourCommands[2] = {"\x1b[31m", "\x1b[33m"};
    const String _clearColourCommand = "\x1b[0m";

    _uart->_uart.print(_colourCommands[colour].c_str());
    _uart->print_timestamp();
    _uart->_uart.println(messageStr);
    _uart->_uart.print(_clearColourCommand.c_str());
}

int StatusHelper::_getWarningId()
{
    return ++_lastWarningId;
}

void StatusHelper::_setLedColour(ledColour_t colour)
{
    // LED is active low
    digitalWrite(_gpioLedRed, !colour.red);
    digitalWrite(_gpioLedGreen, !colour.green);
    digitalWrite(_gpioLedBlue, !colour.blue);
}
