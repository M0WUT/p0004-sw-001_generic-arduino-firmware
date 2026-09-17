#include "main.h"

ServiceTracker *serviceTracker;
TimeHelper timeHelper = TimeHelper();
DebugUartHelper debugUartHelper = DebugUartHelper(gpio_uart_debug_tx, gpio_uart_debug_rx, &timeHelper);
I2CBus i2c_internal = I2CBus(gpio_i2c_int_scl, gpio_i2c_int_sda);
EepromHelper eepromHelper = EepromHelper(EEPROM24AA256UID(&i2c_internal, int_eeprom_i2c_addr), &debugUartHelper);

void setup()
{

  ServiceContext serviceContext = {
      &timeHelper, &debugUartHelper, &eepromHelper};

  serviceTracker = new ServiceTracker(NULL, 0);
  serviceTracker->initialiseServices(&serviceContext);
}

void loop()
{
}
