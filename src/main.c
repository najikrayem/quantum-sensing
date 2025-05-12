#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <math.h>

#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19


#define SAMPLING_FREQ 16
#define TX_FREQ 1
#define NUM_PERIODS 5
/*
CH0 = COS_N
CH1 = COS_P
CH2 = SIN_P
CH3 = SIN_N
*/


#define PI 3.14159265358979323846f

const uint32_t samp_period = 1000 / SAMPLING_FREQ;   //ms
const uint16_t real_samp_freq = 1000 / samp_period; //Hz


// const uint16_t block_size = (uint32_t)((SAMPLING_FREQ / TX_FREQ) * NUM_PERIODS); // Number of samples to process in one block

// uint16_t channel0[block_size] = {0};
// uint16_t channel1[block_size] = {0};
// uint16_t channel2[block_size] = {0};
// uint16_t channel3[block_size] = {0};
// uint16_t current_sample = 0;



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

typedef struct {
    float real;
    float imag;
    float magnitude;
} GoertzelResult;


void goertzel_filter(uint16_t *block, uint16_t block_size, float sample_rate, float target_freq, GoertzelResult *ret) {
    int32_t k = (int32_t)(0.5 + ((block_size * target_freq) / sample_rate));
    float w = (2.0f * PI / block_size) * k;
    float cos_w = cosf(w);
    float sin_w = sinf(w);
    float coeff = 2.0f * cos_w;

    float q1 = 0.0f;
    float q2 = 0.0f;

    for (uint16_t i = 0; i < block_size; i++) {
        float q0 = coeff * q1 - q2 + (float)block[i];
        q2 = q1;
        q1 = q0;
    }

    float real = (q1 - q2 * cos_w);
    float imag = (q2 * sin_w);
    float magnitude = sqrtf(real * real + imag * imag);

    ret->real = real;
    ret->imag = imag;
    ret->magnitude = magnitude;
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



// this main simply samples the ADC and prints the values to the USB serial port
// int main() {
//     stdio_usb_init();
//     stdio_init_all();
//     init_spi();

//     // sleep 6 seconds to allow the USB connection to stabilize
//     sleep_ms(6000);

//     uint64_t start_time;
//     uint64_t elapsed_time;

//     printf("Sampling Frequency: %d Hz\r\n", real_samp_freq);

//     while (1) {
//         start_time = to_ms_since_boot(get_absolute_time());

//         uint64_t adc_value = read_adc();
//         printf("%llu, %llu, %llu, %llu\r\n",
//             (adc_value >> 48) & 0xFFFF,
//             (adc_value >> 32) & 0xFFFF,
//             (adc_value >> 16) & 0xFFFF,
//             adc_value & 0xFFFF);
//         fflush(stdout);
//         stdio_flush();

//         // Calculate elapsed time and adjust delay
//         elapsed_time = to_ms_since_boot(get_absolute_time()) - start_time;
//         if (elapsed_time < samp_period) {
//             sleep_ms(samp_period - elapsed_time);
//         }
//     }
// }



// this main only prints no noise magnet
int main() {
    stdio_usb_init();
    stdio_init_all();
    init_spi();

    // sleep 6 seconds to allow the USB connection to stabilize
    sleep_ms(6000);

    uint64_t start_time;
    uint64_t elapsed_time;

    printf("Sampling Frequency: %d Hz\r\n", real_samp_freq);

    while (1) {
        start_time = to_ms_since_boot(get_absolute_time());

        uint64_t adc_value = read_adc();
        uint16_t ch0 = (uint16_t)((adc_value >> 48) & 0xFFFF);
        uint16_t ch1 = (uint16_t)((adc_value >> 32) & 0xFFFF);
        uint16_t ch2 = (uint16_t)((adc_value >> 16) & 0xFFFF);
        uint16_t ch3 = (uint16_t)(adc_value & 0xFFFF);



        int x = ch1 - ch0;
        int y = ch2 - ch3;

        // float angle = atan2f((float)y, (float)x);
        
        // float angle_degrees = angle * (180.0f / PI);
        // // angle_degrees = ((int)(angle_degrees + 360.0)) % 360;
        // // angle_degrees = ((int)(angle_degrees + 180.0)) % 360;

        // printf("Angle: %.2f degrees\r\n", angle_degrees);

        printf("%d,%d\n",x ,y);


        fflush(stdout);
        stdio_flush();

        // Calculate elapsed time and adjust delay
        elapsed_time = to_ms_since_boot(get_absolute_time()) - start_time;
        if (elapsed_time < samp_period) {
            sleep_ms(samp_period - elapsed_time);
        }
    }
}