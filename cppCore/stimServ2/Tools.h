#pragma once
// Ease of use functions

#include <algorithm>

// https://www.fluentcpp.com/2018/03/30/is-stdfor_each-obsolete/
namespace ranges
{
    template<typename Range, typename Function>
    Function for_each(Range& range, Function f)
    {
        return std::for_each(begin(range), end(range), f);
    }
}
