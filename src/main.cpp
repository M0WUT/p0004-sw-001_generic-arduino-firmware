#include "main.h"

ServiceTracker *serviceTracker;
TimeHelper timeHelper = TimeHelper();
DebugUartHelper debugUartHelper = DebugUartHelper(gpio_uart_debug_tx, gpio_uart_debug_rx, &timeHelper);
StatusHelper statusHelper = StatusHelper(&debugUartHelper, gpio_led_red, gpio_led_green, gpio_led_blue);
I2CBus i2c_internal = I2CBus(gpio_i2c_int_scl, gpio_i2c_int_sda);
EepromHelper eepromHelper = EepromHelper(EEPROM24AA256UID(&i2c_internal, int_eeprom_i2c_addr), &debugUartHelper);
ServiceContext serviceContext = {&timeHelper, &debugUartHelper, &eepromHelper, &statusHelper};
void setup()
{
  serviceTracker = new ServiceTracker(NULL, 0);
  serviceTracker->initialiseServices(&serviceContext);
  int warning1Id = statusHelper.register_warning("Warning1");
  int warning2Id = statusHelper.register_warning("Warning2");
  statusHelper.clear_warning(warning1Id);
  delay(5000);
  statusHelper.clear_warning(warning2Id);
}

void loop() { delay(10); }
