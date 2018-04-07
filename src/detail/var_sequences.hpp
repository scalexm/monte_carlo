#ifndef DETAIL_VAR_SEQUENCES_HPP
#define DETAIL_VAR_SEQUENCES_HPP

#include <tuple>

namespace detail {
namespace var {

// Fonction $H$ du poly (page 175).
template<class RealType>
auto H(RealType state, RealType x, RealType alpha) -> RealType {
    if (x < state)
        return 1;
    return 1 - 1 / (1 - alpha);
}

// Encapsule la suite $\xi_n$ du poly (page 175).
template<class RealType, class Gamma>
class approx_sequence {
    private:
        RealType alpha, xi = 0;
        const Gamma & gamma;
        int n = 0;

    public:
        using result_type = RealType;

        approx_sequence(RealType alpha, const Gamma & gamma) : alpha { alpha }, gamma { gamma }
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

template<class RealType, class Gamma, class Density>
class is_sampling_sequence {
    private:
        RealType alpha, xi = 0, theta = 0, mu = 0;
        const Gamma & gamma;
        int M;
        int n = 0;
    
    public:
        using result_type = std::tuple<RealType, RealType, RealType>;

        is_sampling_sequence(RealType alpha, const Gamma & gamma, int M) :
            alpha { alpha }, gamma { gamma }, M { M }
        {
        }

        template<class Distribution, class Generator>
        auto next(Distribution & d, Generator & g) -> result_type {
            if (n == 0) {
                ++n;
                return std::make_tuple(xi, theta, mu);
            }
            
            RealType alpha_n;
            int treshold = M / 3;
            if (n <= treshold)
                alpha_n = 0.5;
            else if (n <= 2 * treshold)
                alpha_n = 0.8;
            else
                alpha_n = alpha;
            

        }

};

}
}

#endif
