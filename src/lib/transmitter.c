#include "transmitter.h"

void init_tx_antenna(
        TX_Antenna* tx_ant,
        Signal* sig,
        Motor_Control_Function mot_func,
        DBH_12V* DBH12V_Controller,
        bool* msg_symbols,
        size_t num_syms,
        float baud_rate)
    {

        tx_ant->sig = sig;
        tx_ant->mot_func = mot_func;
        tx_ant->DBH12V_Controller = DBH12V_Controller;
        tx_ant->msg_symbols = msg_symbols;
        tx_ant->num_syms = num_syms;
        tx_ant->baud_rate = baud_rate;
        tx_ant->current_sym = 0;
        tx_ant->start_time_ms = 0;
        tx_ant->_baud_period_ms = (uint32_t)(1000/tx_ant->baud_rate);
}


void begin_tx_antenna(TX_Antenna* tx_ant){
    absolute_time_t now = get_absolute_time();
    tx_ant->start_time_ms = to_ms_since_boot(now);
    modulate_ant(tx_ant);
}


bool modulate_ant(TX_Antenna* tx_ant){
    absolute_time_t now = get_absolute_time();
    uint32_t current_time = to_ms_since_boot(now) - tx_ant->start_time_ms;
    tx_ant->current_sym = current_time / tx_ant->_baud_period_ms;

    if (tx_ant->current_sym >= tx_ant->num_syms){
        tx_ant->mot_func(tx_ant->DBH12V_Controller, 0);
        return false;
    }

    float val = 0.0f;
    if (tx_ant->msg_symbols[tx_ant->current_sym]){
        val = sample_signal_now(tx_ant->sig);
    }
    tx_ant->mot_func(tx_ant->DBH12V_Controller, 100.0 * val);

    return true;
}


