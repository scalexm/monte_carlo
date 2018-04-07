#ifndef GAMMA_SEQUENCES_HPP
#define GAMMA_SEQUENCES_HPP

#include <cmath>

namespace gamma_sequences {

// $n \longmapsto \frac{1}{n}$
template<class Float>
auto inverse(int n) -> Float {
    return 1 / static_cast<Float>(n);
}

template<class Float, class Lambda>
auto inverse_pow(Float a) -> Lambda {
    return [a](int n) { return std::pow(inverse<Float>(n), a); };
}

}

#endif
