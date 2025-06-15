#pragma once

#include "pico/stdlib.h"

// in khz
#define PWM_FREQ 150.0f


struct DBH_12V {
    uint16_t wrap;

    uint8_t in1_pin;
    uint8_t in1_slice;
    uint8_t in1_chan;

    uint8_t in2_pin;
    uint8_t in2_slice;
    uint8_t in2_chan;
    
    uint8_t in3_pin;
    uint8_t in3_slice;
    uint8_t in3_chan;
    
    uint8_t in4_pin;
    uint8_t in4_slice;
    uint8_t in4_chan;
} typedef DBH_12V;


void initialize_dbh12v(DBH_12V* controller, uint8_t in1_pin, uint8_t in2_pin, uint8_t in3_pin, uint8_t in4_pin);

void dbh12v_set_A_dutycycle(DBH_12V* controller, float value);
void dbh12v_set_B_dutycycle(DBH_12V* controller, float value);
// Define a function type
typedef void (*Motor_Control_Function)(DBH_12V* controller, float value);