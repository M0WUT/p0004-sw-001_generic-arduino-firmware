#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>

class Buzzer
{
public:
    Buzzer(const int gpioPin);
    void play_tone(const int frequency_hz, const int duration_ms);
    void play_dot_sound();
    void play_dash_sound();
    void play_startup_sound();
    void play_keypress_sound();

private:
    int _gpioPin;
    int _default_tone_freq_hz = 600;
    // In Morse, dot duration (in ms) is 1200 / wpm
    int _default_dot_duration_ms = 1200 / 25;
};

#endif // BUZZER_H