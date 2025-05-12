#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/sync.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>



const uint  TX_GPIO_PIN     = 1;       // Change to your desired GPIO pin
const float TX_FREQUENCY    = 59.0f;
const float RX_FREQUENCY    = 126.0f;
const float BAUD_RATE       = 1.0f;



mutex_t mtx;


bool cur_bit = 0;
uint16_t cur_bit_idx = 0;
bool cur_symbol = 0;
uint16_t cur_sym_idx = 0;


void tx_gen_tone(){
    static const uint32_t half_period = (uint32_t)(1000000.0f / TX_FREQUENCY); // in microseconds
    static const uint16_t switches = (uint16_t)(TX_FREQUENCY / BAUD_RATE); 

    for (uint16_t i = 0; i < switches; i++) {
        gpio_put(TX_GPIO_PIN, 1); // Set pin high
        sleep_us(half_period); // Sleep for half the period
        gpio_put(TX_GPIO_PIN, 0); // Set pin low
        sleep_us(half_period); // Sleep for half the period
    }
    gpio_put(TX_GPIO_PIN, 0); // Set pin low

}

void tx_mod_symbol(){
    if (cur_symbol){
        // generate the tone
        tx_gen_tone();
    } else {
        // do nothing
        sleep_us((uint64_t)(BAUD_RATE * 1000000.0f));
    }
}

void tx_random_bit(){


    mutex_enter_blocking(&mtx);
    cur_bit_idx = cur_bit_idx + 1;
    // generate a random bool
    cur_bit = rand() % 2;
    if (!cur_bit){
        // change polarity for 0
        cur_symbol = !cur_symbol;
    }
    mutex_exit(&mtx);

    // First symbol
    tx_mod_symbol();

    // Second symbol
    mutex_enter_blocking(&mtx); // Lock the mutex
    cur_sym_idx = cur_sym_idx + 1;
    cur_symbol = !cur_symbol;
    mutex_exit(&mtx); // Unlock the mutex

    tx_mod_symbol();

}


void tx_main() {
    // Initialize the GPIO pin it laready assume to be initialized
    while (true) {
        tx_random_bit(); 
    }
}


// ---------------------------------------------------------------------------------------


uint16_t cur_sample_idx = 0;


#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19

void init_spi() {
    spi_init(SPI_PORT, 1000000); // Initialize SPI at 1 MHz
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1); // Set CS high
}

uint16_t read_adc_channel(uint8_t channel) {
    if (channel > 3) {
        return 0; // Invalid channel
    }

    // Construct the command byte
    uint8_t tx_buf[3] = {0};
    tx_buf[0] = 0b00000110;         // Start bit + single-ended mode + D2 (D2 is dont care)
    tx_buf[1] = (channel & 3) << 6; // Channel bits + padding zeros
    tx_buf[2] = 0;                  // Padding for the third byte

    uint8_t rx_buf[3] = {0};

    // Perform SPI transaction
    gpio_put(PIN_CS, 0); // Pull CS low to start communication
    spi_write_read_blocking(SPI_PORT, tx_buf, rx_buf, 3);
    gpio_put(PIN_CS, 1); // Pull CS high to end communication

    // Extract the 12-bit result from the response
    uint16_t result = ((rx_buf[1] & 0x0F) << 8) | rx_buf[2]; // Combine the last 12 bits
    return result;
}

uint64_t read_adc() {
    uint64_t result = 0;
    for (uint8_t i = 0; i < 4; i++) {
        uint16_t adc_value = read_adc_channel(i);
        result <<= 16; 
        result |= adc_value & 0xFFFF;
    }
    return result;
}

void rx_main() {
    static const uint32_t samp_period = 1000 / RX_FREQUENCY;

    uint64_t start_time;
    uint64_t elapsed_time;

    while (true) {
        start_time = to_ms_since_boot(get_absolute_time());

        uint64_t adc_value = read_adc();
        uint16_t ch0 = (uint16_t)((adc_value >> 48) & 0xFFFF);
        uint16_t ch1 = (uint16_t)((adc_value >> 32) & 0xFFFF);
        uint16_t ch2 = (uint16_t)((adc_value >> 16) & 0xFFFF);
        uint16_t ch3 = (uint16_t)(adc_value & 0xFFFF);

        int x = ch1 - ch0;
        int y = ch2 - ch3;

        uint16_t cur_bit_idx_copy;
        bool cur_bit_copy;
        uint16_t cur_sym_idx_copy;
        bool cur_symbol_copy;
        mutex_enter_blocking(&mtx);
        cur_bit_idx_copy = cur_bit_idx;
        cur_bit_copy = cur_bit;
        cur_sym_idx_copy = cur_sym_idx;
        cur_symbol_copy = cur_symbol;
        mutex_exit(&mtx);

        // [sample num],[x],[y],[cur bit idx],[cur bit],[cur sym idx],[cur sym]
        printf("%d,%d,%d,%d,%d,%d,%d\n", cur_sample_idx, x, y, cur_bit_idx_copy, cur_bit_copy, cur_sym_idx_copy, cur_symbol_copy);
        cur_sample_idx++;

        fflush(stdout);
        stdio_flush();

        // Calculate elapsed time and adjust delay
        elapsed_time = to_ms_since_boot(get_absolute_time()) - start_time;
        if (elapsed_time < samp_period) {
            sleep_ms(samp_period - elapsed_time);
        }
    }
}



int main() {
    
    stdio_usb_init();
    init_spi();
    gpio_init(TX_GPIO_PIN);
    gpio_set_dir(TX_GPIO_PIN, GPIO_OUT);
    gpio_put(TX_GPIO_PIN, 0); // Set pin low

    // Initialize the mutex
    mutex_init(&mtx);

    // Initialize the random number generator
    srand(time(NULL));

    // sleep 6 seconds to allow the USB connection to stabilize
    sleep_ms(6000);

    // Start the TX core
    multicore_launch_core1(tx_main);

    // Start the RX core
    rx_main();

    return 0;
}