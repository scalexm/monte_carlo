#include "src/estimate.hpp"
#include "command_line.hpp"
#include <random>
#include <iostream>

auto main(int argc, char ** argv) -> int {
    command_line_args args;
    try {
        args = parse_command_line(argc, argv);
    } catch (const std::string & s) {
        std::cerr << s << std::endl;
        return 1;
    }

    std::random_device rd;
    auto g = std::mt19937 { rd() };
    auto d = std::normal_distribution<> { 0., 1. };

    auto phi = [](double x) {
        auto S = 100 * std::exp((0.05 - 0.2 * 0.2 / 2) + 0.2 * x);
        auto result = -std::exp(0.05) * 10.7;
        if (110 < S)
            return result;
        return 110 - S + result;
    };
    
    std::pair<double, double> result;
    auto step = steps::inverse_pow(args.exponent, args.offset);
    if (args.method == method::stochastic_gradient) {
        result = stochastic_gradient(args.alpha, args.N, phi, step, args.averaging).compute(d, g);
    } else {
        result = importance_sampling(args.alpha, 1., args.N, phi, step, args.averaging).compute(d, g);
    }
    std::cout << result.first << "," << result.second << std::endl;
    return 0;
}
