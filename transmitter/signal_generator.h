#pragma once

#include "pico/stdlib.h"
#include <stdint.h>

#define PI 3.14159265

struct Signal{
    float frequency;  // Frequency of the signal in Hz
    float phase;      // Phase shift of the signal in radians
} typedef Signal;

/**
 * Samples the frequency.
 */
float sample_signal(Signal* signal, uint32_t current_time);

/**
 * Same as sample_signal but does not require current time argument.
 */
float sample_signal_now(Signal* signal);