#include <vector>
#include <cstdint>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <cstring>
#include <limits>

template <int MantissaBits = 64, int ExponentBits = 16>
struct my_float {
    static constexpr int BitsPerWord = 64;

    std::vector<uint64_t> mantissa;
    std::vector<uint64_t> exponent;
    bool sign;

    static constexpr int MantissaWords = (MantissaBits + BitsPerWord - 1) / BitsPerWord;
    static constexpr int ExponentWords = (ExponentBits + BitsPerWord - 1) / BitsPerWord;

    my_float() : mantissa(MantissaWords, 0), exponent(ExponentWords, 0), sign(false) {}

    void clear() {
        std::fill(mantissa.begin(), mantissa.end(), 0);
        std::fill(exponent.begin(), exponent.end(), 0);
        sign = false;
    }

    static void set_bit_in_vector(std::vector<uint64_t>& vec, int bit_index, bool val) {
        int word = bit_index / BitsPerWord;
        int bit = bit_index % BitsPerWord;
        if (word >= static_cast<int>(vec.size())) return;
        if (val)
            vec[word] |= (uint64_t(1) << bit);
        else
            vec[word] &= ~(uint64_t(1) << bit);
    }

    static bool get_bit_from_vector(const std::vector<uint64_t>& vec, int bit_index) {
        int word = bit_index / BitsPerWord;
        int bit = bit_index % BitsPerWord;
        if (word >= static_cast<int>(vec.size())) return false;
        return (vec[word] >> bit) & 1;
    }

    void set_mantissa_bit(int i, bool val) {
        set_bit_in_vector(mantissa, i, val);
    }

    bool get_mantissa_bit(int i) const {
        return get_bit_from_vector(mantissa, i);
    }

    void set_exponent_bit(int i, bool val) {
        set_bit_in_vector(exponent, i, val);
    }

    bool get_exponent_bit(int i) const {
        return get_bit_from_vector(exponent, i);
    }

    void set_sign(bool s) {
        sign = s;
    }

    bool get_sign() const {
        return sign;
    }

    explicit my_float(double d) : mantissa(MantissaWords, 0), exponent(ExponentWords, 0), sign(false) {
        from_double(d);
    }

    void from_double(double d) {
        clear();

        if (d == 0.0) {
            return;
        }

        uint64_t dbits;
        static_assert(sizeof(double) == sizeof(uint64_t), "Double must be 64 bits");
        std::memcpy(&dbits, &d, sizeof(double));

        sign = (dbits >> 63) & 1;
        uint64_t exp_bits = (dbits >> 52) & 0x7FF;
        uint64_t mant_bits = dbits & ((1ULL << 52) - 1);

        // Handle special values
        if (exp_bits == 0) {
            // Treat denormals as zero
            return;
        }

        if (exp_bits == 0x7FF) {
            // Inf or NaN
            for (int i = 0; i < ExponentBits; ++i) {
                set_exponent_bit(i, true);
            }
            if (mant_bits == 0) {
                // Infinity
                return;
            } else {
                // NaN - set first mantissa bit
                set_mantissa_bit(0, true);
                return;
            }
        }

        // Calculate actual exponent and bias
        int64_t actual_exp = static_cast<int64_t>(exp_bits) - 1023;
        uint64_t bias = (1ULL << (ExponentBits - 1)) - 1;
        int64_t biased_exp = actual_exp + static_cast<int64_t>(bias);

        // Handle exponent range
        if (biased_exp < 0) {
            clear();
            return;
        }

        if (static_cast<uint64_t>(biased_exp) >= (1ULL << ExponentBits)) {
            for (int i = 0; i < ExponentBits; ++i) {
                set_exponent_bit(i, true);
            }
            return;
        }

        // Store exponent
        for (int i = 0; i < ExponentBits; ++i) {
            set_exponent_bit(i, (biased_exp >> i) & 1);
        }

        // Store mantissa bits
        int bits_to_store = std::min(52, MantissaBits);
        for (int i = 0; i < bits_to_store; ++i) {
            bool bit = (mant_bits >> (51 - i)) & 1;
            set_mantissa_bit(i, bit);
        }
    }

    double to_double() const {
        // Check for zero
        bool all_exponent_zero = true;
        for (int i = 0; i < ExponentBits; ++i) {
            if (get_exponent_bit(i)) {
                all_exponent_zero = false;
                break;
            }
        }
        if (all_exponent_zero) {
            return sign ? -0.0 : 0.0;
        }

        // Check for Inf/NaN
        bool all_exponent_one = true;
        for (int i = 0; i < ExponentBits; ++i) {
            if (!get_exponent_bit(i)) {
                all_exponent_one = false;
                break;
            }
        }
        if (all_exponent_one) {
            bool is_nan = false;
            for (int i = 0; i < MantissaBits; ++i) {
                if (get_mantissa_bit(i)) {
                    is_nan = true;
                    break;
                }
            }
            if (is_nan) {
                return std::numeric_limits<double>::quiet_NaN();
            } else {
                return sign ? -std::numeric_limits<double>::infinity() 
                            : std::numeric_limits<double>::infinity();
            }
        }

        // Extract exponent
        uint64_t biased_exp = 0;
        for (int i = 0; i < ExponentBits; ++i) {
            if (get_exponent_bit(i)) {
                biased_exp |= (uint64_t(1) << i);
            }
        }

        uint64_t bias = (1ULL << (ExponentBits - 1)) - 1;
        int64_t exp_val = static_cast<int64_t>(biased_exp) - static_cast<int64_t>(bias);

        // Extract mantissa
        int bits_to_extract = std::min(MantissaBits, 53);
        uint64_t mantissa_val = 0;
        for (int i = 0; i < bits_to_extract; ++i) {
            if (get_mantissa_bit(i)) {
                mantissa_val |= (static_cast<uint64_t>(1) << (bits_to_extract - 1 - i));
            }
        }

        // Calculate fractional part and final value
        double frac = 1.0 + static_cast<double>(mantissa_val) / (1ULL << bits_to_extract);
        double result = std::ldexp(frac, static_cast<int>(exp_val));
        return sign ? -result : result;
    }

    explicit operator double() const {
        return to_double();
    }

    friend std::ostream& operator<<(std::ostream& os, const my_float& f) {
        os << static_cast<double>(f);
        return os;
    }

    bool operator==(const my_float& other) const {
        return this->to_double() == other.to_double();
    }

    bool operator!=(const my_float& other) const {
        return !(*this == other);
    }

    bool operator<(const my_float& other) const {
        return this->to_double() < other.to_double();
    }

    bool operator<=(const my_float& other) const {
        return *this < other || *this == other;
    }

    bool operator>(const my_float& other) const {
        return !(*this <= other);
    }

    bool operator>=(const my_float& other) const {
        return !(*this < other);
    }

    // Arithmetic operators with precision warning
    my_float operator+(const my_float& other) const {
        if (MantissaBits > 53 || ExponentBits > 11) {
            std::cerr << "WARNING: Using double-precision arithmetic. "
                      << "Implement custom operations for full " 
                      << MantissaBits << "-bit precision.\n";
        }
        return my_float(this->to_double() + other.to_double());
    }

    my_float operator-(const my_float& other) const {
        if (MantissaBits > 53 || ExponentBits > 11) {
            std::cerr << "WARNING: Using double-precision arithmetic. "
                      << "Implement custom operations for full " 
                      << MantissaBits << "-bit precision.\n";
        }
        return my_float(this->to_double() - other.to_double());
    }

    my_float operator*(const my_float& other) const {
        if (MantissaBits > 53 || ExponentBits > 11) {
            std::cerr << "WARNING: Using double-precision arithmetic. "
                      << "Implement custom operations for full " 
                      << MantissaBits << "-bit precision.\n";
        }
        return my_float(this->to_double() * other.to_double());
    }

    my_float operator/(const my_float& other) const {
        if (other.to_double() == 0.0) {
            throw std::runtime_error("Division by zero");
        }
        if (MantissaBits > 53 || ExponentBits > 11) {
            std::cerr << "WARNING: Using double-precision arithmetic. "
                      << "Implement custom operations for full " 
                      << MantissaBits << "-bit precision.\n";
        }
        return my_float(this->to_double() / other.to_double());
    }

    my_float& operator+=(const my_float& other) {
        *this = *this + other;
        return *this;
    }

    my_float& operator-=(const my_float& other) {
        *this = *this - other;
        return *this;
    }

    my_float& operator*=(const my_float& other) {
        *this = *this * other;
        return *this;
    }

    my_float& operator/=(const my_float& other) {
        *this = *this / other;
        return *this;
    }
};