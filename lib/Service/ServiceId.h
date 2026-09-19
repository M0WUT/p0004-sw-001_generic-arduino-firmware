#ifndef SERVICEID_H
#define SERVICEID_H

#include <enum.h>

// Definitions here to exact numbers are important and must not be changed
// List must be continuous 0 -> N. Additional services must take out a number in this Enum
// This Enum is expected to be bounded to fit in a uint8_t elsewhere -if I've written more than
// 256 services, this has gone much better (or worse!) than planned
BETTER_ENUM(ServiceId, uint8_t, CORE = 0, POWER, THERMAL, LCD, ETHERNET, USB_HOST, USB_DEVICE, SD_CARD, UI, SCPI, HOME_ASSISTANT, APPLICATION)

#endif // SERVICEID_H
