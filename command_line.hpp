#ifndef COMMAND_LINE_HPP
#define COMMAND_LINE_HPP

#include "src/averaging.hpp"

enum class method {
    stochastic_gradient,
    importance_sampling,
};

struct command_line_args {
    double alpha = -1.;
    int N = -1;
    method method = method::stochastic_gradient;
    averaging averaging = averaging::no;
    double exponent = 1.;
    double offset = 0.;
};

auto parse_command_line(int, char **) -> command_line_args;

#endif
