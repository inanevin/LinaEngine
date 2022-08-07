/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#ifndef DataStructuresHashMapSerialization_HPP
#define DataStructuresHashMapSerialization_HPP

#include "cereal/cereal.hpp"
#include <parallel_hashmap/phmap.h>

namespace cereal
{
    // HashMap (flat_hash_map)
    template <class Archive, class K, class V, class Hash, class Eq, class Alloc>
    inline void CEREAL_SAVE_FUNCTION_NAME(Archive& ar, phmap::flat_hash_map<K, V, Hash, Eq, Alloc> const& map)
    {
        ar(make_size_tag(static_cast<size_type>(map.size())));

        for (const auto& i : map)
            ar(make_map_item(i.first, i.second));
    }

    //! ParallelHashMap & ParallelHashMapMutex
    template <class Archive, class K, class V, class Hash, class Eq, class Alloc, size_t N, class Mtx_>
    inline void CEREAL_SAVE_FUNCTION_NAME(Archive& ar, phmap::parallel_flat_hash_map<K, V, Hash, Eq, Alloc, N, Mtx_> const& map)
    {
        ar(make_size_tag(static_cast<size_type>(map.size())));

        for (const auto& i : map)
            ar(make_map_item(i.first, i.second));
    }

    // HashMap (flat_hash_map)
    template <class Archive, class K, class V, class Hash, class Eq, class Alloc>
    inline void CEREAL_LOAD_FUNCTION_NAME(Archive& ar, phmap::flat_hash_map<K, V, Hash, Eq, Alloc>& map)
    {
        size_type size;
        ar(make_size_tag(size));

        map.clear();

        auto hint = map.begin();
        for (size_t i = 0; i < size; ++i)
        {
            typename phmap::parallel_flat_hash_map<K, V, Hash, Eq, Alloc>::key_type    key;
            typename phmap::parallel_flat_hash_map<K, V, Hash, Eq, Alloc>::mapped_type value;

            ar(make_map_item(key, value));
#ifdef CEREAL_OLDER_GCC
            hint = map.insert(hint, std::make_pair(std::move(key), std::move(value)));
#else  // NOT CEREAL_OLDER_GCC
            hint = map.emplace_hint(hint, std::move(key), std::move(value));
#endif // NOT CEREAL_OLDER_GCC
        }
    }

    //! ParallelHashMap & ParallelHashMapMutex
    template <class Archive, class K, class V, class Hash, class Eq, class Alloc, size_t N, class Mtx_>
    inline void CEREAL_LOAD_FUNCTION_NAME(Archive& ar, phmap::parallel_flat_hash_map<K, V, Hash, Eq, Alloc, N, Mtx_>& map)
    {
        size_type size;
        ar(make_size_tag(size));

        map.clear();

        auto hint = map.begin();
        for (size_t i = 0; i < size; ++i)
        {
            typename phmap::parallel_flat_hash_map<K, V, Hash, Eq, Alloc, N, Mtx_>::key_type    key;
            typename phmap::parallel_flat_hash_map<K, V, Hash, Eq, Alloc, N, Mtx_>::mapped_type value;

            ar(make_map_item(key, value));
#ifdef CEREAL_OLDER_GCC
            hint = map.insert(hint, std::make_pair(std::move(key), std::move(value)));
#else  // NOT CEREAL_OLDER_GCC
            hint = map.emplace_hint(hint, std::move(key), std::move(value));
#endif // NOT CEREAL_OLDER_GCC
        }
    }

} // namespace cereal

#endif
