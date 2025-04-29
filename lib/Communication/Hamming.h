#ifndef HAMMING
#define HAMMING

#include "../Shared/Shared.h"

// Function to encode 4-bit data into 7-bit Hamming code
u8 hamming_encode(u8 data);

// Function to decode 7-bit Hamming code back to 4-bit data
u8 hamming_decode(u8 code, u8 *error_pos);

// Function to encode a variable-length input up to 32 bits
u32 encode_data(u32 input, u8 bit_length, u8* output_length);

// Function to decode Hamming-encoded data
u32 decode_data(u32 encoded, u8 bit_length);

#endif