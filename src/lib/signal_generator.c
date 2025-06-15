#include "signal_generator.h"
#include <stdio.h>
#include <math.h>

inline float sample_signal(Signal* signal, uint32_t current_time){
    float t = current_time / 1000.0f;
    float angle = (2 * PI * signal->frequency * t) + signal->phase;
    float ret = sinf(angle);
    return ret;
}

inline float sample_signal_now(Signal* signal){
    absolute_time_t now = get_absolute_time();
    uint32_t current_time = to_ms_since_boot(now);
    return sample_signal(signal, current_time);
}