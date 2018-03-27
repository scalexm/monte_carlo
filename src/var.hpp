#ifndef VAR_HPP
#define VAR_HPP

#include "detail/var_sequence.hpp"

namespace gamma_sequences {
    template<class Float>
    auto inverse(int n) -> Float {
        return 1 / static_cast<Float>(n);
    }
}

template<class Float = double, class Gamma = decltype(gamma_sequences::inverse<Float>)>
class var_seq_kernel {
    private:
        Gamma & gamma;
        Float alpha;
        int iterations;

    public:
        var_seq_kernel(
            Float alpha,
            int iterations,
            Gamma & gamma = gamma_sequences::inverse<Float>
        ) : alpha { alpha }, iterations { iterations }, gamma { gamma }
        {
        }

        var_seq_kernel(const var_seq_kernel &) = default;
        var_seq_kernel(var_seq_kernel &&) = default;
        auto operator =(const var_seq_kernel &) -> var_seq_kernel & = default;
        auto operator =(var_seq_kernel &&) -> var_seq_kernel & = default;

        template<class Distribution, class Generator>
        auto compute(Distribution & d, Generator & g) -> Float {
            auto sequence = detail::var_sequence<Float> { alpha };

            Float state;
            for (int n = 0; n < iterations; ++n)
                state = sequence.next(d, g, gamma);
            return state;
        }
};

#endif
