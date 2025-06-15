#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "pico/stdlib.h"

/**
 * Converts an ASCII string into a differential manchester encoding booleans 
 */
void str_to_diff_man(const char* input, size_t input_size, bool* output, size_t output_size);