#include "src/var.hpp"
#include "src/cvar.hpp"
#include <iostream>
#include <random>

auto main() -> int {
    std::random_device rd;
    std::mt19937 gen(rd());
    
    std::exponential_distribution<> d(1);

    auto var = var::sequential(0.5, 1000000).compute(d, gen);
    std::cout << var << std::endl;

    auto cvar1 = cvar::sequential(0.5, 1000000).compute(d, gen);
    std::cout << cvar1 << std::endl;

    auto cvar2 = cvar::monte_carlo(0.5, var, 1000000).compute(d, gen);
    std::cout << cvar2 << std::endl;

    return 0;
}
