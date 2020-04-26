// ref: https://github.com/star-e/StarEngine/blob/master/Star/SMap.h

#pragma once
#include <map>

// for std::less<> the transparent comparator
// see https://stackoverflow.com/questions/20317413/what-are-transparent-comparators

namespace Ubpa {
    template<class Key, class Value>
    using xMap = std::map<Key, Value, std::less<>>;

    template<class Key, class Value>
    using xPmrMap = std::pmr::map<Key, Value, std::less<>>;
}