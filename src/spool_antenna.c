// // #include <stdio.h>
// // #include "pico/stdlib.h"
// // #include "hardware/pwm.h"

// // #define SERVO_PIN 0 // GPIO pin connected to the servo signal wire
// // #define WRAP_VALUE 20000 // Wrap value for 50Hz PWM frequency


// // void set_servo_speed(int8_t speed) {
// //     // Speed should be between -100 (full reverse) and 100 (full forward)
// //     if (speed < -100) speed = -100;
// //     if (speed > 100) speed = 100;

// //     // Map speed to PWM duty cycle (0-100% mapped to 0-65535)
// //     uint16_t duty_cycle = (uint16_t)((speed + 100) * (WRAP_VALUE / 200));
// //     printf("Setting duty cycle: %d\n", duty_cycle);
// //     pwm_set_gpio_level(SERVO_PIN, duty_cycle);
// // }

// // int main() {
// //     stdio_init_all();
    

// //     sleep_ms(6000); // Wait for serial connection

// //     printf("Servo control program started.\n");
// //     printf("Servo pin: %d\n", SERVO_PIN);

// //     // Initialize GPIO for PWM
// //     gpio_set_function(SERVO_PIN, GPIO_FUNC_PWM);

// //     // Configure PWM frequency (50Hz for servo control)
// //     uint slice_num = pwm_gpio_to_slice_num(SERVO_PIN);
// //     pwm_config config = pwm_get_default_config();
// //     pwm_config_set_clkdiv(&config, 150.0f); // 150 MHz / 150 = 1 MHz base clock
// //     pwm_config_set_wrap(&config, WRAP_VALUE);    // 1 MHz / WRAP_VALUE = 50 Hz
// //     pwm_init(slice_num, &config, true);

// //     // Set initial servo speed to 0 (stop)
// //     set_servo_speed(0);

// //     printf("Servo speed is set to 0.\n");

// //     while (true) {
// //         // printf("Enter speed (-100 to 100): ");
// //         // int speed;
// //         // scanf("%d", &speed);

// //         // set_servo_speed(slice_num, speed);
// //         printf("Setting servo speed to 100.\n");
// //         set_servo_speed(100);
// //         sleep_ms(2000); // Wait for 2 seconds
// //         printf("Setting servo speed to -100.\n");
// //         set_servo_speed(-100);
// //         sleep_ms(2000); // Wait for 2 seconds
// //         printf("Setting servo speed to 0.\n");
// //         set_servo_speed(0);
// //         sleep_ms(2000); // Wait for 2 seconds
// //         printf("Setting servo speed to 50.\n");
// //         set_servo_speed(50);
// //         sleep_ms(2000); // Wait for 2 seconds
// //         printf("Setting servo speed to -50.\n");
// //         set_servo_speed(-50);
// //         sleep_ms(2000); // Wait for 2 seconds
// //         printf("Setting servo speed to 20.\n");
// //         set_servo_speed(20);
// //         sleep_ms(2000); // Wait for 2 seconds
// //         printf("Setting servo speed to -20.\n");
// //         set_servo_speed(-20);
// //         sleep_ms(2000); // Wait for 2 seconds
// //     }

// //     return 0;
// // }






// // // /**
// // //  * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
// // //  *
// // //  * SPDX-License-Identifier: BSD-3-Clause
// // //  */

// // // // Fade an LED between low and high brightness. An interrupt handler updates
// // // // the PWM slice's output level each time the counter wraps.

// // // #include "pico/stdlib.h"
// // // #include <stdio.h>
// // // #include "pico/time.h"
// // // #include "hardware/irq.h"
// // // #include "hardware/pwm.h"

// // // #ifdef PICO_DEFAULT_LED_PIN
// // // void on_pwm_wrap() {
// // //     static int fade = 0;
// // //     static bool going_up = true;
// // //     // Clear the interrupt flag that brought us here
// // //     pwm_clear_irq(pwm_gpio_to_slice_num(PICO_DEFAULT_LED_PIN));

// // //     if (going_up) {
// // //         ++fade;
// // //         if (fade > 255) {
// // //             fade = 255;
// // //             going_up = false;
// // //         }
// // //     } else {
// // //         --fade;
// // //         if (fade < 0) {
// // //             fade = 0;
// // //             going_up = true;
// // //         }
// // //     }
// // //     // Square the fade value to make the LED's brightness appear more linear
// // //     // Note this range matches with the wrap value
// // //     pwm_set_gpio_level(PICO_DEFAULT_LED_PIN, fade * fade);
// // // }
// // // #endif

// // // int main() {
// // // #ifndef PICO_DEFAULT_LED_PIN
// // // #warning pwm/led_fade example requires a board with a regular LED
// // // #else
// // //     // Tell the LED pin that the PWM is in charge of its value.
// // //     gpio_set_function(PICO_DEFAULT_LED_PIN, GPIO_FUNC_PWM);
// // //     // Figure out which slice we just connected to the LED pin
// // //     uint slice_num = pwm_gpio_to_slice_num(PICO_DEFAULT_LED_PIN);

// // //     // Mask our slice's IRQ output into the PWM block's single interrupt line,
// // //     // and register our interrupt handler
// // //     pwm_clear_irq(slice_num);
// // //     pwm_set_irq_enabled(slice_num, true);
// // //     irq_set_exclusive_handler(PWM_DEFAULT_IRQ_NUM(), on_pwm_wrap);
// // //     irq_set_enabled(PWM_DEFAULT_IRQ_NUM(), true);

// // //     // Get some sensible defaults for the slice configuration. By default, the
// // //     // counter is allowed to wrap over its maximum range (0 to 2**16-1)
// // //     pwm_config config = pwm_get_default_config();
// // //     // Set divider, reduces counter clock to sysclock/this value
// // //     pwm_config_set_clkdiv(&config, 4.f);
// // //     // Load the configuration into our PWM slice, and set it running.
// // //     pwm_init(slice_num, &config, true);

// // //     // Everything after this point happens in the PWM interrupt handler, so we
// // //     // can twiddle our thumbs
// // //     while (1)
// // //         tight_loop_contents();
// // // #endif
// // // }






// #include "pico/stdlib.h"
// #include "hardware/pwm.h"

// #define SERVO_PIN 0

// void set_servo_pulse_us(uint slice, uint channel, uint32_t pulse_width_us) {
//     // At 50Hz, wrap = 20000 us / clock_period (e.g., 125MHz → divider needed)
//     // We'll use wrap = 20000 (20ms period), so pulse_width_us directly maps to level
//     pwm_set_chan_level(slice, channel, pulse_width_us);
// }

// int main() {
//     stdio_init_all();

//     gpio_set_function(SERVO_PIN, GPIO_FUNC_PWM);
//     uint slice_num = pwm_gpio_to_slice_num(SERVO_PIN);
//     uint channel = pwm_gpio_to_channel(SERVO_PIN);

//     pwm_set_wrap(slice_num, 20000); // 20 ms period
//     pwm_set_clkdiv(slice_num, 150.0f); // 125 MHz / 125 = 1 MHz → 1 tick = 1 µs

//     pwm_set_enabled(slice_num, true);

//     set_servo_pulse_us(slice_num, channel, 1600); // Initial position (1500us)
//     sleep_ms(200);
//     set_servo_pulse_us(slice_num, channel, 1550);


//     while (true) {
//         tight_loop_contents(); // Keep the CPU busy
//         // Fade forward (1500us → 2000us)
//         // for (int pulse = 1500; pulse <= 2000; pulse += 10) {
//         //     set_servo_pulse_us(slice_num, channel, pulse);
//         //     sleep_ms(20);
//         // }

//         // // Fade back to stop
//         // for (int pulse = 2000; pulse >= 1500; pulse -= 10) {
//         //     set_servo_pulse_us(slice_num, channel, pulse);
//         //     sleep_ms(20);
//         // }

//         // // Fade reverse (1500us → 1000us)
//         // for (int pulse = 1500; pulse >= 1000; pulse -= 10) {
//         //     set_servo_pulse_us(slice_num, channel, pulse);
//         //     sleep_ms(20);
//         // }

//         // // Fade back to stop
//         // for (int pulse = 1000; pulse <= 1500; pulse += 10) {
//         //     set_servo_pulse_us(slice_num, channel, pulse);
//         //     sleep_ms(20);
//         // }
//     }
// }






