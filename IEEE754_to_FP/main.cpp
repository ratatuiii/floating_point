#include <iostream>
#include <iomanip>
#include <cmath>
#include <limits>
#include <bitset>
#include <cstdint>
#include <cstring>
#include "fucntions.hpp"

void print_separator() {
    std::cout << "-----------------------------\n";
}

void test_value(float value) {
    std::cout << "Testing float: " << std::setprecision(10) << value << "\n";

    // === FP16 ===
    uint16_t fp16 = float_to_fp16(value);
    float fp16_back = fp16_to_float(fp16);
    std::cout << "  fp16 -> bits: " << std::bitset<16>(fp16)
              << " -> back: " << fp16_back << "\n";

    // === FP32 ===
    uint32_t bits32 = float_to_bits(value);
    float back32 = bits_to_float(bits32);
    std::cout << "  fp32 -> bits: " << std::hex << bits32 << std::dec
              << " -> back: " << back32 << "\n";

    // === FP64 ===
    double value64 = static_cast<double>(value);
    uint64_t bits64 = double_to_bits(value64);
    double back64 = bits_to_double(bits64);
    std::cout << "  fp64 -> bits: " << std::hex << bits64 << std::dec
              << " -> back: " << back64 << "\n";

    // === FP80 ===
    long double value80 = static_cast<long double>(value);
    uint8_t bytes80[10];
    long_double_to_bytes(value80, bytes80);
    long double back80 = bytes_to_long_double(bytes80);
    std::cout << "  fp80 -> bytes: ";
    for (int i = 0; i < 10; ++i)
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)bytes80[i] << " ";
    std::cout << std::dec << "-> back: " << back80 << "\n";

    print_separator();
}

int main() {
    std::cout << std::fixed;

    // === Corner-case values for testing ===
    test_value(0.0f);                                       // +0
    test_value(-0.0f);                                      // -0
    test_value(std::numeric_limits<float>::infinity());     // +inf
    test_value(-std::numeric_limits<float>::infinity());    // -inf
    test_value(std::numeric_limits<float>::quiet_NaN());    // NaN
    test_value(1.0f);                                       // Normal number
    test_value(-1.0f);                                      // Normal negative
    test_value(1e-8f);                                      // Subnormal range (denormalized)
    test_value(std::numeric_limits<float>::max());          // Max float
    test_value(std::numeric_limits<float>::min());          // Min normalized float
    test_value(std::numeric_limits<float>::denorm_min());   // Min subnormal float

    return 0;
}
