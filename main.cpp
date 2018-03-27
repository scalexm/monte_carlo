#include "src/var.hpp"
#include <iostream>
#include <random>

auto main() -> int {
    std::random_device rd;
    std::mt19937 gen(rd());
    
    auto kernel = var_seq_kernel<> { 1000000 };
    std::exponential_distribution<> d(1);
    auto value = kernel.compute(d, gen, 0.5);
    std::cout << value << std::endl;

    return 0;
}
