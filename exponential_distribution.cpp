#include <iostream>
#include "src/estimate.hpp"
#include "command_line.hpp"
#include <random>
#include <iostream>

auto var(double alpha, double lambda) -> double {
    return -std::log(1 - alpha) / lambda;
}

auto cvar(double alpha, double lambda) -> double {
    return var(alpha, lambda) + 1 / lambda;
}

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
    auto lambda = 2.;
    auto d = std::exponential_distribution<> { lambda };

    auto phi = identity;
    
    std::pair<double, double> result;
    auto step = steps::inverse_pow(args.exponent, args.offset);
    if (args.method == method::stochastic_gradient) {
        result = stochastic_gradient(args.alpha, args.N, phi, step, args.averaging).compute(d, g);
    } else {
        result = importance_sampling(args.alpha, 1., args.N, phi, step, args.averaging).compute(d, g);
    }
    std::cout << result.first << "," << result.second << std::endl;
    std::cout << var(args.alpha, lambda) << "," << cvar(args.alpha, lambda) << std::endl;
    return 0;
}
