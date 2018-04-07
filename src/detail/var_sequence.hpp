#ifndef DETAIL_VAR_SEQUENCE_HPP
#define DETAIL_VAR_SEQUENCE_HPP

namespace detail {

// Fonction $H$ du poly (page 175).
template<class Float>
auto H(Float state, Float x, Float alpha) -> Float {
    if (x < state)
        return 1;
    return 1 - 1 / (1 - alpha);
}

// Encapsule la suite $\xi_n$ du poly (page 175).
template<class Float, class Gamma>
class var_sequence {
    private:
        Float alpha, state = 0;
        Gamma & gamma;
        int n = 0;

    public:
        using result_type = Float;

        var_sequence(Float alpha, Gamma & gamma) : alpha { alpha }, gamma { gamma }
        {
        }

        template<class Distribution, class Generator>
        auto next(Distribution & d, Generator & g) -> result_type {
            if (n == 0) {
                ++n;
                return state;
            }
            state -= gamma(n) * H(state, d(g), alpha);
            ++n;
            return state;
        }
};

}

#endif
