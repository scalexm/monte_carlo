#include "var.hpp"
#include "detail/cvar_sequence.hpp"

template<class Float = double, class Gamma = decltype(gamma_sequences::inverse<Float>)>
class cvar_seq_kernel {
    private:
        Gamma & gamma;
        Float alpha;
        int iterations;

    public:
        cvar_seq_kernel(
            Float alpha,
            int iterations,
            Gamma & gamma = gamma_sequences::inverse<Float>
        ) : alpha { alpha }, iterations { iterations }, gamma { gamma }
        {
        }

        cvar_seq_kernel(const cvar_seq_kernel &) = default;
        cvar_seq_kernel(cvar_seq_kernel &&) = default;
        auto operator =(const cvar_seq_kernel &) -> cvar_seq_kernel & = default;
        auto operator =(cvar_seq_kernel &&) -> cvar_seq_kernel & = default;

        template<class Distribution, class Generator>
        auto compute(Distribution & d, Generator & g) -> Float {
            auto sequence = detail::cvar_sequence<Float> { alpha };

            Float state;
            for (int n = 0; n < iterations; ++n)
                state = sequence.next(d, g, gamma);
            return state;
        }
};

template<class Float = double>
class cvar_monte_carlo_kernel {
    private:
        Float alpha, estimated_var;
        int iterations;

    public:
        cvar_monte_carlo_kernel(
            Float alpha,
            Float estimated_var,
            int iterations
        ) : alpha { alpha }, estimated_var { estimated_var }, iterations { iterations }
        {
        }

        cvar_monte_carlo_kernel(const cvar_monte_carlo_kernel &) = default;
        cvar_monte_carlo_kernel(cvar_monte_carlo_kernel &&) = default;
        auto operator =(const cvar_monte_carlo_kernel &) -> cvar_monte_carlo_kernel & = default;
        auto operator =(cvar_monte_carlo_kernel &&) -> cvar_monte_carlo_kernel & = default;

        template<class Distribution, class Generator>
        auto compute(Distribution & d, Generator & g) -> Float {
            Float sum = 0;
            int n = 0;
            while (n < iterations) {
                Float x = d(g);
                if (x < estimated_var)
                    continue;
                ++n;
                sum += x;
            }
            return sum / iterations;
        }
};
