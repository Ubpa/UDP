// ref: https://github.com/star-e/StarEngine/blob/master/Star/SSet.h

#pragma once
#include <set>

namespace Ubpa {
    template<class Value>
    using xSet = std::set<Value, std::less<>>;

    template<class Value>
    using xPmrSet = std::set<Value, std::less<>,
        std::pmr::polymorphic_allocator<Value>
    >;

    template<class Value, class KeyLike, class Allocator>
    inline bool exists(const std::set<Value, std::less<>, Allocator>& m, const KeyLike& key) noexcept {
        auto iter = m.find(key);
        return iter != m.end();
    }

    template<class Value, class Allocator, class KeyLike>
    inline typename std::set<Value, std::less<>, Allocator>::mapped_type&
        at(std::set<Value, std::less<>, Allocator>& m, const KeyLike& key) {
        auto iter = m.find(key);
        if (iter == m.end()) {
            throw std::out_of_range("at(std::set) out of range");
        }
        return iter->second;
    }

    template<class Value, class Allocator, class KeyLike>
    inline typename std::set<Value, std::less<>, Allocator>::mapped_type const&
        at(const std::set<Value, std::less<>, Allocator>& m, const KeyLike& key) {
        auto iter = m.find(key);
        if (iter == m.end()) {
            throw std::out_of_range("at(std::set) out of range");
        }
        return iter->second;
    }
}