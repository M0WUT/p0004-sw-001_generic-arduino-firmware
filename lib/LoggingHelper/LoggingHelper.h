#ifndef LOGGINGHELPER_H
#define LOGGINGHELPER_H

#include <Arduino.h>
#include "TimeHelper.h"
#include "pinout.h"
#include "version.h"

constexpr int DEBUG_UART_BUFFER_SIZE = 128 * sizeof(char);
constexpr int MAX_WARNINGS_BEFORE_ERROR = 8;

typedef struct warning_t
{
    int id;
    const char *description;
} warning_t;

// Can expand to 8 bit colour later
typedef struct ledColour_t
{
    bool red;
    bool green;
    bool blue;
} ledColour_t;

// Increasing number = increasing severity
enum class LoggingLevel
{
    DEBUG = 0,
    INFO = 1,
    WARNING = 2,
    ERROR = 3,
    CRITICAL = 4,
};

class LoggingHelper
{
    // LED Colour definitions
    const ledColour_t LED_OFF = {0, 0, 0};
    const ledColour_t LED_BLUE = {0, 0, 1};
    const ledColour_t LED_GREEN = {0, 1, 0};
    const ledColour_t LED_TURQUOISE = {0, 1, 1};
    const ledColour_t LED_RED = {1, 0, 0};
    const ledColour_t LED_PURPLE = {1, 0, 1};
    const ledColour_t LED_ORANGE = {1, 1, 0};
    const ledColour_t LED_WHITE = {1, 1, 1};

public:
    LoggingHelper(int gpio_uart_tx, int gpio_uart_rx, TimeHelper *timeHelper, int gpioLedRed, int gpioLedGreen, int gpioLedBlue, LoggingLevel loggingLevel);
    void error(const char *serviceName, const char *errorStr);
    void errorf(const char *serviceName, const char *fmt, ...);
    // Returns unique id of the warning so it can be cleared later
    int warning(const char *serviceName, const char *warningStr);
    int warningf(const char *serviceName, const char *fmt, ...);
    void info(const char *serviceName, const char *infoStr);
    void infof(const char *serviceName, const char *fmt, ...);
    void debug(const char *serviceName, const char *debugStr);
    void debugf(const char *serviceName, const char *fmt, ...);
    void clear_warning(int warningId);

private:
    Uart _uart;
    TimeHelper *_timeHelper;
    void _print_timestamp();

    int _gpioLedRed, _gpioLedGreen, _gpioLedBlue;
    void _setLedColour(ledColour_t colour);
    uint32_t _lastWarningId = 0;
    std::vector<warning_t> _warningList;
    int _getWarningId();

    LoggingLevel _loggingLevel;

    char _printBuffer[DEBUG_UART_BUFFER_SIZE];
    void _printStatusMessage(const char *serviceName, const char *messageStr, LoggingLevel loggingLevel);
};

#endif // LOGGINGHELPER_H
