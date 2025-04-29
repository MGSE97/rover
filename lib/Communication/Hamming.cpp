
#include "Hamming.h"

// Function to encode 4-bit data into 7-bit Hamming code
u8 hamming_encode(u8 data) {
    u8 d1 = (data >> 3) & 1;
    u8 d2 = (data >> 2) & 1;
    u8 d3 = (data >> 1) & 1;
    u8 d4 = (data >> 0) & 1;

    u8 p1 = d1 ^ d2 ^ d4;
    u8 p2 = d1 ^ d3 ^ d4;
    u8 p3 = d2 ^ d3 ^ d4;

    return (p1 << 6) | (p2 << 5) | (d1 << 4) | (p3 << 3) | (d2 << 2) | (d3 << 1) | d4;
}

// Function to decode 7-bit Hamming code back to 4-bit data
u8 hamming_decode(u8 code, u8 *error_pos) {
    u8 p1 = (code >> 6) & 1;
    u8 p2 = (code >> 5) & 1;
    u8 d1 = (code >> 4) & 1;
    u8 p3 = (code >> 3) & 1;
    u8 d2 = (code >> 2) & 1;
    u8 d3 = (code >> 1) & 1;
    u8 d4 = (code >> 0) & 1;

    u8 s1 = p1 ^ d1 ^ d2 ^ d4;
    u8 s2 = p2 ^ d1 ^ d3 ^ d4;
    u8 s3 = p3 ^ d2 ^ d3 ^ d4;

    *error_pos = (s3 << 2) | (s2 << 1) | s1;
    
    if (*error_pos) {
        code ^= (1 << (7 - *error_pos));
        return hamming_decode(code, error_pos);
    }

    return (d1 << 3) | (d2 << 2) | (d3 << 1) | d4;
}

// Function to encode a variable-length input up to 32 bits
u32 encode_data(u32 input, u8 bit_length, u8* output_length) {
    u32 encoded = 0;
    int chunk_count = (bit_length + 3) / 4;
    *output_length = chunk_count * 7;
    
    for (int i = 0; i < chunk_count; i++) {
        u8 encoded_chunk = hamming_encode((input >> (4 * (chunk_count - 1 - i))) & 0xF);
        encoded |= ((u32)encoded_chunk << (7 * (chunk_count - 1 - i)));
    }
    return encoded;
}

// Function to decode Hamming-encoded data
u32 decode_data(u32 encoded, u8 bit_length) {
    u32 output = 0;
    u8 chunk_count = (bit_length + 3) / 4;
    for (u8 i = 0; i < chunk_count; i++) {
        u8 error_pos;
        u8 chunk = (encoded >> (7 * (chunk_count - 1 - i))) & 0x7F;
        u8 decoded = hamming_decode(chunk, &error_pos);
        output |= ((u32)decoded << (4 * (chunk_count - 1 - i)));
    }
    return output;
}
