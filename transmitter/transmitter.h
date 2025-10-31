/**
 * Executes the slice scheduler
 */

#pragma once

#include "signal_generator.h"
#include "dbh-12.h"

struct TX_Antenna {
    Signal* sig;
    Motor_Control_Function mot_func;
    DBH_12V* DBH12V_Controller;
    bool* msg_symbols;
    size_t num_syms;
    float baud_rate;    // in symbols per second
    size_t current_sym;
    uint32_t start_time_ms;
    uint32_t _baud_period_ms;
} typedef TX_Antenna;


void init_tx_antenna(
        TX_Antenna* tx_ant,
        Signal* sig,
        Motor_Control_Function mot_func,
        DBH_12V* DBH12V_Controller,
        bool* msg_symbols,
        size_t num_syms,
        float baud_rate);


void begin_tx_antenna(TX_Antenna* tx_ant);


bool modulate_ant(TX_Antenna* tx_ant);