#pragma once

#include <cstdint>

// FP16
float fp16_to_float(uint16_t bits);
uint16_t float_to_fp16(float value);

// FP32
uint32_t float_to_bits(float value);
float bits_to_float(uint32_t bits);

// FP64
double bits_to_double(uint64_t bits);
uint64_t double_to_bits(double value);

// FP80 (long double — 10 байт)
void long_double_to_bytes(long double value, uint8_t bytes[10]);
long double bytes_to_long_double(const uint8_t bytes[10]);