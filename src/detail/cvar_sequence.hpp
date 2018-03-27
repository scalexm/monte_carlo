#ifndef DETAIL_CVAR_SEQUENCE_HPP
#define DETAIL_CVAR_SEQUENCE_HPP

#include "var_sequence.hpp"
#include <algorithm>

namespace detail {
    template<class Float>
    auto Lambda(Float state, Float x, Float alpha) -> Float {
        return state + 1 / (1 - alpha) * std::max(x - state, static_cast<Float>(0));
    }

    template<class Float>
    class cvar_sequence {
        private:
            Float alpha, state = 0;
            var_sequence<Float> var_state;
            int n = 0;

        public:
            cvar_sequence(Float alpha) :
                alpha { alpha }, var_state { alpha }
            {
            }

            cvar_sequence(const cvar_sequence &) = default;
            cvar_sequence(cvar_sequence &&) = default;
            auto operator =(const cvar_sequence &) -> cvar_sequence & = default;
            auto operator =(cvar_sequence &&) -> cvar_sequence & = default;

            template<class Distribution, class Generator, class Gamma>
            auto next(Distribution & d, Generator & g, Gamma & gamma) -> Float {
                if (n == 0) {
                    ++n;
                    return state;
                }
                auto lambda = Lambda(var_state.next(d, g, gamma), d(g), alpha);
                state -= 1 / static_cast<Float>(n + 1) * (state - lambda);
                ++n;
                return state;
            }
    };
}

#endif
