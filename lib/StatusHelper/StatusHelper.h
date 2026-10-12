
#ifndef STATUSHELPER_H
#define STATUSHELPER_H

#include <Arduino.h>
#include "DebugUartHelper.h"

class StatusHelper
{
public:
    StatusHelper(DebugUartHelper *uart, int gpioLedRed, int gpioLedGreen, int gpioLedBlue);
    void register_error(const char *errorStr);
    // Returns unique id of the warning so it can be cleared later
    int register_warning(const char *warningStr);
    void clear_warning(int warningId);
    void register_waiting_for_user(const char *userStr);

private:
    typedef struct warning_t
    {
        int id;
        const char *description;
    } warning_t;

    typedef struct ledColour_t
    {
        bool red;
        bool green;
        bool blue;
    } ledColour_t;

    const ledColour_t LED_OFF = {0, 0, 0};
    const ledColour_t LED_BLUE = {0, 0, 1};
    const ledColour_t LED_GREEN = {0, 1, 0};
    const ledColour_t LED_TURQUOISE = {0, 1, 1};
    const ledColour_t LED_RED = {1, 0, 0};
    const ledColour_t LED_PURPLE = {1, 0, 1};
    const ledColour_t LED_ORANGE = {1, 1, 0};
    const ledColour_t LED_WHITE = {1, 1, 1};

    enum TextColour
    {
        RED,
        YELLOW
    };
    DebugUartHelper *_uart;
    int _gpioLedRed, _gpioLedGreen, _gpioLedBlue;
    uint32_t _lastWarningId = 0;
    std::vector<warning_t> _warningList;
    // Maximum number of concurrent warnings before an error is thrown
    const int _maxWarnings = 8;
    void _print_error(const char *errorStr);
    void _print_warning(const char *warningStr);
    void _printStatusMessage(const char *messageStr, TextColour colour);
    int _getWarningId();
    void _setLedColour(ledColour_t colour);
};

#endif // STATUSHELPER_H
