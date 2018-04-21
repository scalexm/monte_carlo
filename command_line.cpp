#include "command_line.hpp"
#include <string>

auto parse_command_line(int argc, char ** argv) -> command_line_args {
    command_line_args args;

    int i = 1;
    // Paramètres de la ligne de commande, cf `README.txt`.
    while (i < argc) {
        auto option = std::string { argv[i] };
        if (option == "--method") {
            ++i;
            if (i == argc)
                throw "missing argument for `--method`";
            auto value = std::string { argv[i] };
            if (value == "stochastic-gradient")
                args.method = method::stochastic_gradient;
            else if (value == "importance-sampling")
                args.method = method::importance_sampling;
            else
                throw "bad method name: " + value;
        } else if (option == "--averaging") {
            ++i;
            if (i == argc)
                throw "missing argument for `--averaging`";
            auto value = std::string { argv[i] };
            if (value == "yes")
                args.averaging = averaging::yes;
            else if (value == "no")
                args.averaging = averaging::no;
            else
                throw "bad averaging parameter: " + value;
        } else if (option == "--step") {
            ++i;
            if (i == argc)
                throw "missing argument for `--step`";
            auto value = std::string { argv[i] };
            try { args.exponent = std::stod(value); } catch(...) { args.exponent = -1.; }
            if (args.exponent <= 0 || args.exponent > 1)
                throw "bad exponent value: " + value;
            ++i;
            if (i == argc)
                throw "missing argument for `--step`";
            value = std::string { argv[i] };
            try { args.offset = std::stod(value); } catch(...) { args.offset = -1.; }
            if (args.offset < 0)
                throw "bad offset value: " + value;
        } else {
            if (args.alpha < 0) {
                try { args.alpha = std::stod(option); } catch(...) { args.alpha = -1.; }
                if (args.alpha <= 0 || args.alpha >= 1)
                    throw "bad alpha value: " + option;
            } else if (args.N < 0) {
                try { args.N = std::stoi(option); } catch(...) { args.N = -1; }
                if (args.N <= 100)
                    throw "bad N value: " + option;
            }
        }
        ++i;
    }

    if (args.alpha < 0)
        throw std::string { "missing parameter alpha" };
    if (args.N < 0)
        throw std::string { "missing parameter N" };
    return args;
}
