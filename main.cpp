#include "src/var.hpp"
#include "src/cvar.hpp"
#include <iostream>
#include <random>

auto main() -> int {
    std::random_device rd;
    std::mt19937 gen(rd());
    
    std::exponential_distribution<> d(1);

    auto var_kernel = var_seq_kernel<> { 0.5, 1000000 };
    auto var = var_kernel.compute(d, gen);
    std::cout << var << std::endl;

    auto cvar_kernel1 = cvar_seq_kernel<> { 0.5, 1000000 };
    auto cvar1 = cvar_kernel1.compute(d, gen);
    std::cout << cvar1 << std::endl;

    auto cvar_kernel2 = cvar_monte_carlo_kernel<> { 0.5, var, 1000000 };
    auto cvar2 = cvar_kernel2.compute(d, gen);
    std::cout << cvar2 << std::endl;

    return 0;
}
