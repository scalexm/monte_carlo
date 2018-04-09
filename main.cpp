#include "src/estimate.hpp"
#include "command_line_parser.hpp"
#include <random>
#include <iostream>
#include <vector>

enum class method {
    stochastic_gradient,
    importance_sampling,
};

void command_line(int argc, char ** argv) {
    double alpha = -1.;
    int N = -1;
    auto method = method::stochastic_gradient;
    auto averaging = averaging::no;
    double exponent = 1.;
    double offset = 0.;

    auto parser = command_line_parser { argc, argv };
    parser.dummy();
    while (!parser.finished()) {
        std::unique_ptr<command_line_parser::handle> hdl;
        if ((hdl = parser.enter("--method"))) {
            hdl->next_arg();
            auto value = hdl->value();
            if (value == "stochastic-gradient") {
                method = method::stochastic_gradient;
            } else if (value == "importance-sampling") {
                method = method::importance_sampling;
            } else {
                throw "bad method name: " + value;
            }
        } else if ((hdl = parser.enter("--averaging"))) {
            hdl->next_arg();
            auto value = hdl->value();
            if (value == "yes") {
                averaging = averaging::yes;
            } else if (value == "no") {
                averaging = averaging::no;
            } else {
                throw "bad averaging parameter: " + value;
            }
        } else if ((hdl = parser.enter("--step"))) {
            hdl->next_arg();
            auto value = hdl->value();
            try { exponent = std::stod(value); } catch(...) { exponent = -1.; }
            if (exponent <= 0 || exponent > 1)
                throw "bad exponent value: " + value;
            hdl->next_arg();
            value = hdl->value();
            try { offset = std::stod(value); } catch(...) { offset = -1.; }
            if (offset < 0)
                throw "bad offset value: " + value;
        } else {
            auto dummy = parser.dummy();
            auto value = dummy->value();
            if (alpha < 0) {
                try { alpha = std::stod(value); } catch(...) { alpha = -1.; }
                if (alpha <= 0 || alpha >= 1)
                    throw "bad alpha value: " + value;
            } else if (N < 0) {
                try { N = std::stoi(value); } catch(...) { N = -1; }
                if (N <= 100)
                    throw "bad N value: " + value;
            }
        }
    }

    if (alpha < 0)
        throw std::string { "missing parameter alpha" };
    if (N < 0)
        throw std::string { "missing parameter N" };

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
    auto step = steps::inverse_pow(exponent, offset);
    if (method == method::stochastic_gradient) {
        result = stochastic_gradient(alpha, N, phi, step, averaging).compute(d, g);
    } else {
        result = importance_sampling(alpha, 1., N, phi, step, averaging).compute(d, g);
    }
    std::cout << result.first << "," << result.second << std::endl;
}

auto main(int argc, char ** argv) -> int {
    try {
        command_line(argc, argv);
    } catch (const std::string & s) {
        std::cerr << s << std::endl;
        return 1;
    }
    return 0;
}
