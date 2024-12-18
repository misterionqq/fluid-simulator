#pragma once


#include <cinttypes>
#include <type_traits>
#include <array>
#include <iostream>
#include "fluid.h"


#define FLOAT 1
#define DOUBLE 2
#define FIXED(n, k) ((n) * 1000 + (k))
#define FAST_FIXED(n, k) ((n) * 100000 + (k))
#define BASESIZE(n, m) std::pair<int, int>((n), (m))
#ifndef DTYPES
#error "Types are not definded"
#endif
#ifndef DSIZES
#define DSIZES BASESIZE(-1, -1)
#endif

namespace Pepega {
    template<int n>
    struct get_type_inner {
        using type = std::conditional_t<n == 1, float, std::conditional_t<n == 2, double,
        std::conditional_t<(n > 100000), Fixed<n / 100000, n % 100000, true>,
        std::conditional_t<(n > 1000), Fixed<n / 1000, n % 1000>, void>>>>;
    };

    template<int n>
    using get_type = get_type_inner<n>::type;

    constexpr auto create_variatons() {
        constexpr std::pair<int, int> givenSizes[] = {DSIZES, {-1, -1}};
        constexpr int sizesCnt = sizeof(givenSizes) / sizeof(std::pair<int, int>);
        constexpr auto givenTypes = std::array{DTYPES};
        constexpr std::array<std::pair<int, int>, sizesCnt> sizes = {std::pair<int, int>(-1, -1), DSIZES};

        std::array<std::tuple<int, int, int, int, int>,
                givenTypes.size() * givenTypes.size() * givenTypes.size() * sizesCnt> res = {};

        int index = 0;
        for (int pType: givenTypes) {
            for (int vType: givenTypes) {
                for (int vfType: givenTypes) {
                    for (auto field: sizes) {
                        res[index] = {pType, vType, vfType, field.first, field.second};
                        ++index;
                    }
                }
            }
        }
        return res;
    }

    constexpr auto variations = create_variatons();
    std::array<std::shared_ptr<Pepega::fluid_base>(*)(), variations.size()> generateEngine;

    template<int index>
    struct fluid_idx {
        fluid_idx<index - 1> x;

        fluid_idx() {
            generateEngine[index - 1] = generate;
        }

        static std::shared_ptr<Pepega::fluid_base> generate() {
            return std::make_shared<Pepega::fluid<get_type<std::get<0>(variations[index - 1])>,
                    get_type<std::get<1>(variations[index - 1])>,
                    get_type<std::get<2>(variations[index - 1])>,
                    std::get<3>(variations[index - 1]),
                    std::get<4>(variations[index - 1])>>();
        }
    };

    template<>
    struct fluid_idx<0> {
        fluid_idx() = default;
    };

    [[maybe_unused]] fluid_idx<variations.size()> generator{};
}

std::shared_ptr<Pepega::fluid_base> create_fluid(int pType, int vType, int vfType, int n, int m) {
    auto itr = std::find(Pepega::variations.begin(),
                         Pepega::variations.end(),
                         std::tuple(pType, vType, vfType, n, m));
    std::shared_ptr<Pepega::fluid_base> fluid;
    if (itr != Pepega::variations.end()) {
        fluid = Pepega::generateEngine[itr - Pepega::variations.begin()]();
    } else {
        itr = std::find(Pepega::variations.begin(),
                             Pepega::variations.end(),
                             std::tuple(pType, vType, vfType, -1, -1));
        if (itr == Pepega::variations.end()) {
            throw std::invalid_argument("Unknown types useds");
        }
        fluid = Pepega::generateEngine[itr - Pepega::variations.begin()]();
    }
    return fluid;
}

