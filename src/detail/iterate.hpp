#ifndef DETAIL_ITERATE_HPP
#define DETAIL_ITERATE_HPP

namespace detail {

template<class Sequence, class Distribution, class Generator>
inline auto iterate(
    Sequence sequence,
    int iterations,
    Distribution & d,
    Generator & g
) -> typename Sequence::result_type
{
    typename Sequence::result_type state;
    for (int n = 0; n < iterations; ++n)
        state = sequence.next(d, g);
    return state;
}

}

#endif
