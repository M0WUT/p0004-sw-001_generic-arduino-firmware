#ifndef INC_MAIN_H
#define INC_MAIN_H

#include <Arduino.h>
#include <lvgl.h>
#include <ArduinoHA.h>

#include "pinout.h"
#include "TimeHelper.h"
#include "DebugUartHelper.h"
#include "EepromHelper.h"
#include "ServiceContext.h"
#include "ServiceTracker.h"
#include "StatusHelper.h"
#include "tft_handler.h"

#define GNSS_MAX_SATS 32
#define GNSS_TOP_SATS 8

typedef struct
{
    uint8_t prn;
    float azimuth;   /* Degrees clockwise from north */
    float elevation; /* Degrees above horizon */
    float cn0;       /* C/N0 in dB-Hz */
    bool tracked;    /* Satellite signal is tracked */
    bool used;       /* Used in navigation solution */
} gnss_sat_t;

typedef struct
{
    char utc[9];        /* HH:MM:SS */
    uint8_t sat_count;  /* Number of entries in sats[] */
    uint8_t used_count; /* Satellites used in solution */
    float hdop;
    gnss_sat_t sats[GNSS_MAX_SATS];
} gnss_data_t;

void gnss_tracker_create(void);
void gnss_tracker_update(const gnss_data_t *data);

#endif // INC_MAIN_H