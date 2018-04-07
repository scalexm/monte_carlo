#ifndef DETAIL_AVERAGING_HPP
#define DETAIL_AVERAGING_HPP

#include <utility>

namespace detail {

template<class Sequence>
class averaging {
private:
    Sequence state;
    typename Sequence::result_type avg_state;
    int n = 0;

public:
    using result_type = typename Sequence::result_type;

    averaging(Sequence state) : state { std::move(state) }
    {
    }

    template<class Distribution, class Generator>
    auto next(Distribution & d, Generator & g) -> result_type {
        if (n == 0) {
            avg_state = state.next(d, g);
            ++n;
            return avg_state;
        }
        avg_state -= (avg_state - state.next(d, g)) / (n + 1);
        ++n;
        return avg_state;
    }
};

}

#endif
