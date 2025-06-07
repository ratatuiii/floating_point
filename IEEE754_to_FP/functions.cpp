#include "fucntions.hpp"
#include <cmath>
#include <limits>
#include <cstring>

float fp16_to_float(uint16_t bits)
{
    uint16_t sign = (bits >> 15) & 0x1;
    uint16_t exponent = (bits >> 10) & 0x1F;
    uint16_t fraction = bits & 0x3FF;

    float result;
    if (exponent == 0)
    {
        if (fraction == 0)
        {
            // ±0
            result = 0.0f;
        }
        else
        {
            // Denormalized number
            result = std::ldexp(static_cast<float>(fraction), -24); // 2^-10 * 2^-14 = 2^-24
        }
    }
    else if (exponent == 0x1F)
    {
        if (fraction == 0)
        {
            // ±Inf
            result = std::numeric_limits<float>::infinity();
        }
        else
        {
            // NaN
            result = std::numeric_limits<float>::quiet_NaN();
        }
    }
    else
    {
        // Normalized number
        float mantissa = 1.0f + static_cast<float>(fraction) / 1024.0f;
        int exp = static_cast<int>(exponent) - 15;
        result = std::ldexp(mantissa, exp);
    }

    return sign ? -result : result;
}

uint16_t float_to_fp16(float value)
{
    union
    {
        float f;
        uint32_t u;
    } f32;
    f32.f = value;

    uint32_t sign = (f32.u >> 31) & 0x1;
    int32_t exponent = ((f32.u >> 23) & 0xFF) - 127;
    uint32_t mantissa = f32.u & 0x7FFFFF;

    uint16_t result = 0;

    if (exponent == 128)
    {
        if (mantissa == 0)
        {
            result = (sign << 15) | (0x1F << 10); // Inf
        }
        else
        {
            result = (sign << 15) | (0x1F << 10) | 0x200; // NaN
        }
    }
    else if (exponent > 15)
    {
        result = (sign << 15) | (0x1F << 10); // Overflow to Inf
    }
    else if (exponent < -14)
    {
        if (exponent < -24)
        {
            result = (sign << 15); // Underflow to zero
        }
        else
        {
            uint16_t sub_mantissa = (mantissa | 0x800000) >> (1 - exponent - 14 + 13);
            result = (sign << 15) | sub_mantissa;
        }
    }
    else
    {
        uint16_t exp = exponent + 15;
        uint16_t frac = mantissa >> 13;
        result = (sign << 15) | (exp << 10) | frac;
    }

    return result;
}

uint32_t float_to_bits(float value)
{
    union
    {
        float f;
        uint32_t u;
    } conv;
    conv.f = value;
    return conv.u;
}

float bits_to_float(uint32_t bits)
{
    union
    {
        uint32_t u;
        float f;
    } conv;
    conv.u = bits;
    return conv.f;
}

double bits_to_double(uint64_t bits)
{
    union
    {
        uint64_t u;
        double d;
    } conv;
    conv.u = bits;
    return conv.d;
}

uint64_t double_to_bits(double value)
{
    union
    {
        uint64_t u;
        double d;
    } conv;
    conv.d = value;
    return conv.u;
}

void long_double_to_bytes(long double value, uint8_t bytes[10])
{
    std::memcpy(bytes, &value, 10);
}

long double bytes_to_long_double(const uint8_t bytes[10])
{
    long double value;
    std::memcpy(&value, bytes, 10);
    return value;
}
