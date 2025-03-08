
#include <Arduino.h>

// Function to encode 4-bit data into 7-bit Hamming code
uint8_t hamming_encode(uint8_t data) {
    uint8_t d1 = (data >> 3) & 1;
    uint8_t d2 = (data >> 2) & 1;
    uint8_t d3 = (data >> 1) & 1;
    uint8_t d4 = (data >> 0) & 1;

    uint8_t p1 = d1 ^ d2 ^ d4;
    uint8_t p2 = d1 ^ d3 ^ d4;
    uint8_t p3 = d2 ^ d3 ^ d4;

    return (p1 << 6) | (p2 << 5) | (d1 << 4) | (p3 << 3) | (d2 << 2) | (d3 << 1) | d4;
}

// Function to decode 7-bit Hamming code back to 4-bit data
uint8_t hamming_decode(uint8_t code, uint8_t *error_pos) {
    uint8_t p1 = (code >> 6) & 1;
    uint8_t p2 = (code >> 5) & 1;
    uint8_t d1 = (code >> 4) & 1;
    uint8_t p3 = (code >> 3) & 1;
    uint8_t d2 = (code >> 2) & 1;
    uint8_t d3 = (code >> 1) & 1;
    uint8_t d4 = (code >> 0) & 1;

    uint8_t s1 = p1 ^ d1 ^ d2 ^ d4;
    uint8_t s2 = p2 ^ d1 ^ d3 ^ d4;
    uint8_t s3 = p3 ^ d2 ^ d3 ^ d4;

    *error_pos = (s3 << 2) | (s2 << 1) | s1;
    
    if (*error_pos) {
        code ^= (1 << (7 - *error_pos));
        return hamming_decode(code, error_pos);
    }

    return (d1 << 3) | (d2 << 2) | (d3 << 1) | d4;
}

// Function to encode a variable-length input up to 32 bits
uint32_t encode_data(uint32_t input, uint8_t bit_length, char* output_length) {
    uint32_t encoded = 0;
    int chunk_count = (bit_length + 3) / 4;
    *output_length = chunk_count * 7;
    
    for (int i = 0; i < chunk_count; i++) {
        uint8_t encoded_chunk = hamming_encode((input >> (4 * (chunk_count - 1 - i))) & 0xF);
        encoded |= ((uint32_t)encoded_chunk << (7 * (chunk_count - 1 - i)));
    }
    return encoded;
}

// Function to decode Hamming-encoded data
uint32_t decode_data(uint32_t encoded, uint8_t bit_length) {
    uint32_t output = 0;
    uint8_t chunk_count = (bit_length + 3) / 4;
    for (uint8_t i = 0; i < chunk_count; i++) {
        uint8_t error_pos;
        uint8_t chunk = (encoded >> (7 * (chunk_count - 1 - i))) & 0x7F;
        uint8_t decoded = hamming_decode(chunk, &error_pos);
        output |= ((uint32_t)decoded << (4 * (chunk_count - 1 - i)));
    }
    return output;
}
