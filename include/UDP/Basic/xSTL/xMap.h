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

    // help functions
    template<class Key, class Value, class KeyLike, class Allocator>
    inline bool exists(const std::map<Key, Value, std::less<>, Allocator>& m, const KeyLike& key) noexcept {
        auto iter = m.find(key);
        return iter != m.end();
    }

    template<class Key, class Value, class Allocator, class KeyLike>
    inline typename std::map<Key, Value, std::less<>, Allocator>::mapped_type&
        at(std::map<Key, Value, std::less<>, Allocator>& m, const KeyLike& key) {
        auto iter = m.find(std::string_view(key));
        if (iter == m.end()) {
            throw std::out_of_range("at(std::map) out of range");
        }
        return iter->second;
    }

    template<class Key, class Value, class Allocator, class KeyLike>
    inline typename std::map<Key, Value, std::less<>, Allocator>::mapped_type const&
        at(const std::map<Key, Value, std::less<>, Allocator>& m, const KeyLike& key) {
        auto iter = m.find(std::string_view(key));
        if (iter == m.end()) {
            throw std::out_of_range("at(std::map) out of range");
        }
        return iter->second;
    }
}