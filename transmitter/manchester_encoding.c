#include "manchester_encoding.h"

/**
 * Takes a strings and returns an array of booleans representing its differential manchester 
 * encoding of its bits.
 */
void str_to_diff_man(const char* input, size_t input_size, bool* output, size_t output_size){
    if (output_size < (input_size * 8 * 2)) {
        printf("str_to_diff_man error 1\n");
		return;
    }

	uint8_t current_bit = 0;
	bool current_sym = false;
	size_t current_output_idx = 0;
	for (size_t c = 0; c < input_size; c++) {
		char ascii_char = input[c];

		for (uint8_t bit = 0; bit < 8; bit++){
			uint8_t mask = 0b10000000 >> bit;

			if (!(ascii_char & mask)){
				current_sym = !current_sym;
			}
			output[current_output_idx++] = current_sym;
			current_sym = !current_sym;
			output[current_output_idx++] = current_sym;
		}
	}
}