#pragma once


#include <array>
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <limits>
#include <random>
#include <tuple>

namespace Pepega {
    template <int N, bool isFast>
    struct real_type {
        using type = std::conditional_t<
        N <= 8, int_fast8_t,
        std::conditional_t<
                N <= 16, int_fast16_t,
                std::conditional_t<
                N <= 32, int_fast32_t,
                std::conditional_t<N <= 64, int_fast64_t, void>>>>;
    };

    template <int N, bool isFast>
    using real_type_t = typename real_type<N, isFast>::type;

    template <int N>
    struct real_type<N, false> {
        using type = std::conditional_t<
        N == 8, int8_t,
        std::conditional_t<
                N == 16, int16_t,
                std::conditional_t<
                N == 32, int32_t,
                std::conditional_t<N == 64, int64_t, void>>>>;
    };


    template <int N, int K, bool isFast = false>
    struct Fixed {
        using value_t = real_type_t<N, isFast>;

        constexpr static value_t scale = 1ll << K;
        value_t v = 0;

        static const int k = K;

        template <int otherN, int otherK, bool otherIsFast>
        constexpr Fixed(const Fixed<otherN, otherK, otherIsFast>& other) {
            if constexpr (otherK > K) {
                v = other.v >> (otherK - K);
            } else {
                v = other.v << (K - otherK);
            }
        }

        constexpr Fixed(int64_t v) : v(v << K) {}

        constexpr Fixed(float f) : v(f * Fixed::scale) {}

        constexpr Fixed(double f) : v(f * Fixed::scale) {}

        constexpr Fixed() : v(0) {}

        static constexpr Fixed from_raw(int x) {
            Fixed ret{};
            ret.v = x;
            return ret;
        }

        auto operator<=>(const Fixed&) const = default;
        bool operator==(const Fixed&) const = default;

        explicit constexpr operator float() { return float(v) / (1LL << K); }

        explicit constexpr operator double() { return double(v) / (1LL << K); }

        friend Fixed operator+(Fixed a, Fixed b) {
            return Fixed::from_raw(a.v + b.v);
        }

        friend Fixed operator-(Fixed a, Fixed b) {
            return Fixed::from_raw(a.v - b.v);
        }

        friend Fixed operator*(Fixed a, Fixed b) {
            return Fixed::from_raw((static_cast<int64_t>(a.v) * b.v) >> K);
        }

        friend Fixed operator/(Fixed a, Fixed b) {
            return Fixed::from_raw((static_cast<int64_t>(a.v) << K) / b.v);
        }

        friend Fixed& operator+=(Fixed& a, Fixed b) { return a = a + b; }

        friend Fixed& operator-=(Fixed& a, Fixed b) { return a = a - b; }

        friend Fixed& operator*=(Fixed& a, Fixed b) { return a = a * b; }

        friend Fixed& operator/=(Fixed& a, Fixed b) { return a = a / b; }

        friend Fixed operator-(Fixed x) { return Fixed::from_raw(-x.v); }

        friend Fixed fabs(Fixed x) {
            if (x.v < 0) {
                x.v = -x.v;
            }
            return x;
        }

        friend std::ostream& operator<<(std::ostream& out, Fixed x) {
            return out << x.v / (double)(1 << K);
        }
    };

}
