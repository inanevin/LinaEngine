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

#ifndef HashMapSerialization_HPP
#define HashMapSerialization_HPP

#include "Data/HashMap.hpp"

namespace Lina::Serialization
{

    template <typename Ar, typename T> void WriteMap(Ar& ar, T& map)
    {
        const uint32 size = static_cast<uint32>(map.size());
        ar(size);

        for (auto& [key, val] : map)
        {
            ar(key);
            ar(val);
        }
    }

    template <typename Ar, typename T, typename U, typename V> void ReadMap(Ar& ar, T& map)
    {
        uint32 size = 0;
        ar(size);
        map.clear();

        for (uint32 i = 0; i < size; i++)
        {
            U key = U();
            V val = V();
            ar(key);
            ar(val);
            map[key] = val;
        }
    }

    template <typename Ar, typename T, typename K> void SaveOther(Ar& ar, HashMap<T, K>& map)
    {
        WriteMap<Ar, HashMap<T, K>>(ar, map);
    }
    template <typename Ar, typename T, typename K> void LoadOther(Ar& ar, HashMap<T, K>& map)
    {
        ReadMap<Ar, HashMap<T, K>, T, K>(ar, map);
    }

    template <typename Ar, typename T, typename K> void SaveOther(Ar& ar, ParallelHashMapMutex<T, K>& map)
    {
        WriteMap<Ar, ParallelHashMapMutex<T, K>>(ar, map);
    }
    template <typename Ar, typename T, typename K> void LoadOther(Ar& ar, ParallelHashMapMutex<T, K>& map)
    {
        ReadMap<Ar, ParallelHashMapMutex<T, K>, T, K>(ar, map);
    }

    template <typename Ar, typename T, typename K> void SaveOther(Ar& ar, ParallelHashMap<T, K>& map)
    {
        WriteMap<Ar, ParallelHashMap<T, K>>(ar, map);
    }
    template <typename Ar, typename T, typename K> void LoadOther(Ar& ar, ParallelHashMap<T, K>& map)
    {
        ReadMap<Ar, ParallelHashMap<T, K>, T, K>(ar, map);
    }

} // namespace Lina::Serialization
#endif
