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
    template<class Float>
    class var_sequence {
        private:
            Float alpha, state = 0;
            int n = 0;

        public:
            var_sequence(Float alpha) :
                alpha { alpha }
            {
            }

            var_sequence(const var_sequence &) = default;
            var_sequence(var_sequence &&) = default;
            auto operator =(const var_sequence &) -> var_sequence & = default;
            auto operator =(var_sequence &&) -> var_sequence & = default;

            template<class Distribution, class Generator, class Gamma>
            auto next(Distribution & d, Generator & g, Gamma & gamma) -> Float {
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
