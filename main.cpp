#include "src/estimate.hpp"
#include <random>
#include <iostream>

constexpr int N = 10000;

auto main() -> int {
    std::random_device rd;
    std::mt19937 gen(rd());

    auto phi = [](double x) {
        auto S = 100 * std::exp((0.05 - 0.2 * 0.2 / 2) + 0.2 * x);
        auto result = -std::exp(0.05) * 10.7;
        if (110 < S)
            return result;
        return 110 - S + result;
    };

    std::normal_distribution<> d(0., 1.);

    auto real_d = [&phi, &d](std::mt19937 & g) {
        return phi(d(g));
    };

    auto cvar = stochastic_gradient(0.995, N, steps::inverse_pow()).compute(real_d, gen).second;
    std::cout << cvar << std::endl;

    auto IS_cvar = importance_sampling(0.995, 1., N, phi, steps::inverse_pow()).compute(d, gen).second;
    std::cout << IS_cvar << std::endl;

    return 0;
}
