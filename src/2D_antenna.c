#include "pico/stdlib.h"
#include <stdio.h>
#include "lib/dbh-12.h"
#include "lib/signal_generator.h"
#include "lib/manchester_encoding.h"
#include "lib/transmitter.h"

// Transmitter A
#define IN1_PIN 2
#define IN2_PIN 3

// Transmitter B
#define IN3_PIN 5
#define IN4_PIN 6


int main() {
    stdio_init_all();
    sleep_ms(6000); // Wait for serial connection

    DBH_12V DBH12V_Controller;
    initialize_dbh12v(&DBH12V_Controller, IN1_PIN, IN2_PIN, IN3_PIN, IN4_PIN);

    printf("dbh12v initialized\n");

    // 4Hz
    Signal sig_A;
    sig_A.frequency = 4.3f;
    sig_A.phase = 0.0f;

    // test manchester
    const char* msg = "~~Awaiting response from Unit Zaatar...~~";
    //const char* msg = "HI";
    uint16_t inputlen = 42;
    bool man_bits[inputlen*2*8];

    str_to_diff_man(msg, inputlen, man_bits, inputlen*2*8);
    for (size_t i = 0; i < inputlen * 8 * 2; i++) {
        printf("%d", man_bits[i]);
        if (i % 16 == 15) printf(" ");
    }
    printf("\n");


    TX_Antenna ant_A;

    init_tx_antenna(&ant_A, &sig_A, &dbh12v_set_B_dutycycle, &DBH12V_Controller, man_bits, inputlen*2*8, 0.5f);
    begin_tx_antenna(&ant_A);

    while(modulate_ant(&ant_A)){
        modulate_ant(&ant_A);
    }

    printf("DONE!");
    

    // while(true){
    //     float sig = sample_signal_now(&sig_A);
    //     dbh12v_set_B_dutycycle(&DBH12V_Controller, 100.0*sig);
    //     printf("%f\n", (sig*100.0) + 200);
    //     sleep_ms(10);
    // }
    return 0;
}