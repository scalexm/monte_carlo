#ifndef DETAIL_ITERATE_HPP
#define DETAIL_ITERATE_HPP

namespace detail {

template<class Sequence>
auto iterate(
    Sequence sequence,
    int iterations
) -> typename Sequence::result_type
{
    typename Sequence::result_type state;
    for (int n = 0; n < iterations; ++n)
        state = sequence.next();
    return state;
}

}

#endif
