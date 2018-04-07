#include "src/var.hpp"
#include "src/cvar.hpp"
#include <iostream>
#include <random>

constexpr int N = 1000000;

auto main() -> int {
    std::random_device rd;
    std::mt19937 gen(rd());

    std::exponential_distribution<> d(1);

    auto var = var::stochastic_approx(0.5, N).compute(d, gen);
    std::cout << var << std::endl;

    auto cvar = cvar::stochastic_approx(0.5, N).compute(d, gen);
    std::cout << cvar << std::endl;

    auto avg_cvar = cvar::stochastic_approx(
        0.5,
        N,
        Averaging::Yes,
        steps::inverse_pow<double>(0.75)
    ).compute(d, gen);
    std::cout << avg_cvar << std::endl;

    auto mc_cvar = cvar::monte_carlo(0.5, var, N).compute(d, gen);
    std::cout << mc_cvar << std::endl;

    return 0;
}
