#pragma once

#include <cstdint>

// https://stackoverflow.com/questions/48896142/is-it-possible-to-get-hash-values-as-compile-time-constants
// https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function#FNV_offset_basis

template <typename HashT, typename Str>
constexpr HashT HashString(const Str& toHash)
{
    // For this example, I'm requiring size_t to be 64-bit, but you could
    // easily change the offset and prime used to the appropriate ones
    // based on sizeof(size_t).
    static_assert(sizeof(HashT) == 8 || sizeof(HashT) == 4);
    // FNV-1a 64 bit algorithm

    HashT result = 0;
    HashT fnvPrime = 0;

    if constexpr (sizeof(HashT) == 8) {
        result = 0xcbf29ce484222325; // FNV offset basis
        fnvPrime = 1099511628211;
    }
    else if constexpr (sizeof(HashT) == 4) {
        result = 0x811c9dc5; // FNV offset basis
        fnvPrime = 16777619;
    }
    else {
        static_assert(false, "sizeof(HashT) must be 8 or 4 bytes");
    }

    for (auto c : toHash) {
        result ^= c;
        result *= fnvPrime; // FNV prime
    }

    return result;
}
