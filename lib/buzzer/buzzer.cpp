#include "buzzer.h"

Buzzer::Buzzer(const int gpioPin)
{
    _gpioPin = gpioPin;
    pinMode(_gpioPin, OUTPUT);
}

void Buzzer::play_tone(const int frequency_hz, const int duration_ms)
{
    tone(_gpioPin, frequency_hz, duration_ms);
}

void Buzzer::play_dot_sound()
{
    play_tone(_default_tone_freq_hz, _default_dot_duration_ms);
    // play_tone is async so need to wait for note to send and inter-note spacing
    delay(2 * _default_dot_duration_ms);
}

void Buzzer::play_dash_sound()
{
    play_tone(_default_tone_freq_hz, 3 * _default_dot_duration_ms);
    delay(4 * _default_dot_duration_ms);
}

void Buzzer::play_startup_sound()
{
    // Send ? in CW

    // Two dots
    for (int i = 0; i < 2; i++)
    {
        play_dot_sound();
    }

    // Two dashes
    for (int i = 0; i < 2; i++)
    {
        play_dash_sound();
    }

    // Two dots
    for (int i = 0; i < 2; i++)
    {
        play_dot_sound();
    }
    noTone(_gpioPin);
}

void Buzzer::play_keypress_sound()
{
    play_dot_sound();
}
