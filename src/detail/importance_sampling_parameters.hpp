#ifndef DETAIL_IMPORTANCE_SAMPLING_PARAMETERS_HPP
#define DETAIL_IMPORTANCE_SAMPLING_PARAMETERS_HPP

#include <random>
#include <cmath> // `std::exp`

namespace detail {

template<class Distribution>
class IS_params {
    private:
        Distribution & d;

        using input_type = decltype(d(std::mt19937 { std::random_device { }() }));

    public:
        IS_params(Distribution & d) : d { d }
        {
            static_assert(d == d, "distribution not supported");
        }

        auto b() const -> double {
            static_assert(d == d, "distribution not supported");
        }

        auto rho() const -> double {
            static_assert(d == d, "distribution not supported");
        }

        auto incr(const input_type & x, const input_type & theta) const -> double {
            static_assert(d == d, "distribution not supported");
        }

        auto W(const input_type & x, const input_type & theta) const -> double {
            static_assert(d == d, "distribution not supported");
        }
};

template<>
class IS_params<std::normal_distribution<>> {
    private:
        std::normal_distribution<> & d;

    public:
        IS_params(std::normal_distribution<> & d) : d { d }
        {
        }

        auto b() const -> double {
            return 2;
        }

        auto rho() const -> double {
            return 0.5;
        }

        auto incr(const double & x, const double & theta) const -> double {
            auto mu = d.mean();
            auto stddev = d.stddev();
            auto y = x - mu;
            auto z = y + theta;
            return std::exp(0.5 / stddev / stddev * (y * y - z * z));
        }

        auto W(const double & x, const double & theta) const -> double {
            auto mu = d.mean();
            auto stddev = d.stddev();
            auto q = theta / stddev;
            return std::exp(q * q) * (2 * theta - x + mu);
        }
};

}

#endif
