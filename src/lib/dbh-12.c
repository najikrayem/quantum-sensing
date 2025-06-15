#include "dbh-12.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"
#include <math.h>


uint16_t init_pwm_slice(uint8_t slice, uint32_t freq) {
    uint32_t sys_clk_hz = clock_get_hz(clk_sys);

    uint16_t wrap = (sys_clk_hz / freq) - 1;

    pwm_set_wrap(slice, wrap);
    pwm_set_enabled(slice, true);

    return wrap;
}


void initialize_dbh12v(DBH_12V* controller, uint8_t in1_pin, uint8_t in2_pin, uint8_t in3_pin, uint8_t in4_pin){
    // Initialize direction and PWM pins
    // 1
    controller->in1_pin = in1_pin;
    controller->in1_slice = pwm_gpio_to_slice_num(in1_pin);
    controller->in1_chan = pwm_gpio_to_channel(in1_pin);
    gpio_init(in1_pin);
    gpio_set_function(in1_pin, GPIO_FUNC_PWM);
    controller->wrap = init_pwm_slice(controller->in1_slice, PWM_FREQ);

    //2
    controller->in2_pin = in2_pin;
    controller->in2_slice = pwm_gpio_to_slice_num(in2_pin);
    controller->in2_chan = pwm_gpio_to_channel(in2_pin);
    gpio_init(in2_pin);
    gpio_set_function(in2_pin, GPIO_FUNC_PWM);
    init_pwm_slice(controller->in2_slice, PWM_FREQ);

    //3
    controller->in3_pin = in3_pin;
    controller->in3_slice = pwm_gpio_to_slice_num(in3_pin);
    controller->in3_chan = pwm_gpio_to_channel(in3_pin);
    gpio_init(in3_pin);
    gpio_set_function(in3_pin, GPIO_FUNC_PWM);
    init_pwm_slice(controller->in3_slice, PWM_FREQ);

    //4
    controller->in4_pin = in4_pin;
    controller->in4_slice = pwm_gpio_to_slice_num(in4_pin);
    controller->in4_chan = pwm_gpio_to_channel(in4_pin);
    gpio_init(in4_pin);
    gpio_set_function(in4_pin, GPIO_FUNC_PWM);
    init_pwm_slice(controller->in4_slice, PWM_FREQ);
}


static void _dbh12v_set_dutycycle(uint8_t slice1, uint8_t channel1, uint8_t slice2, uint8_t channel2, uint16_t wrap, float value){
    if (value > 98.0){
        value = 98.0;
    } else if (value < -98.0) {
        value = -98.0;
    }

    bool forward = value >= 0.0f;
    uint16_t duty = fabsf(value)/100 * wrap;
    
    if (forward) {
        pwm_set_chan_level(slice1, channel1, duty);
        pwm_set_chan_level(slice2, channel2, 0);
    } else {
        pwm_set_chan_level(slice1, channel1, 0);
        pwm_set_chan_level(slice2, channel2, duty);
    }
}

// Motor A 
void dbh12v_set_A_dutycycle(DBH_12V* controller, float value) {

    uint16_t slice1 = controller->in1_slice;
    uint16_t channel1 = controller->in1_chan;

    uint16_t slice2 = controller->in2_slice;
    uint16_t channel2 = controller->in2_chan;

    uint16_t wrap = controller->wrap;

    _dbh12v_set_dutycycle(slice1, channel1, slice2, channel2, wrap, value);
}


// Motor B
void dbh12v_set_B_dutycycle(DBH_12V* controller, float value) {

    uint16_t slice1 = controller->in3_slice;
    uint16_t channel1 = controller->in3_chan;

    uint16_t slice2 = controller->in4_slice;
    uint16_t channel2 = controller->in4_chan;

    uint16_t wrap = controller->wrap;

    _dbh12v_set_dutycycle(slice1, channel1, slice2, channel2, wrap, value);

}