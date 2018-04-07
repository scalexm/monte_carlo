#ifndef DETAIL_VAR_SEQUENCES_HPP
#define DETAIL_VAR_SEQUENCES_HPP

namespace detail {
namespace var {

// Fonction $H$ du poly (page 175).
template<class Float>
auto H(Float state, Float x, Float alpha) -> Float {
    if (x < state)
        return 1;
    return 1 - 1 / (1 - alpha);
}

// Encapsule la suite $\xi_n$ du poly (page 175).
template<class Float, class Gamma>
class approx_sequence {
    private:
        Float alpha, xi = 0;
        const Gamma & gamma;
        int n = 0;

    public:
        using result_type = Float;

        approx_sequence(Float alpha, const Gamma & gamma) : alpha { alpha }, gamma { gamma }
        {
        }

        template<class Distribution, class Generator>
        auto next(Distribution & d, Generator & g) -> result_type {
            if (n == 0) {
                ++n;
                return xi;
            }
            xi -= gamma(n) * H(xi, d(g), alpha);
            ++n;
            return xi;
        }
};

template<class Float, class Gamma>
class is_sampling_sequence {
    private:

};

}
}

#endif
