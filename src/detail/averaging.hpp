#ifndef DETAIL_AVERAGING_HPP
#define DETAIL_AVERAGING_HPP

#include "tuple.hpp"
#include <utility> // `std::move`

namespace detail {

// Moyennise une suite donnée à la volée.
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

    auto next() -> result_type {
        if (n == 0) {
            avg_state = state.next();
            ++n;
            return avg_state;
        }
        avg_state = avg_state - (avg_state - state.next()) / n;
        ++n;
        return avg_state;
    }
};

}

#endif
