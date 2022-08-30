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

#include <Data/HashMap.hpp>
#include "Archive.hpp"

namespace Lina::Serialization
{

    template<typename T>
    void WriteMap(Archive<OStream>& ar, T& map)
    {
        const uint32 size = static_cast<uint32>(map.size());
        ar.m_stream << size;

        for (auto& [key, val] : map)
        {
            ar(key);
            ar(val);
        }
    }

    template<typename T, typename U, typename V>
    void ReadMap(Archive<IStream>& ar, T& map)
    {
        uint32 size = 0;
        ar.m_stream >> size;
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

    template<typename A, typename B>
    struct Serialize_NonTrivial<Archive<OStream>, HashMap<A, B>>
    {
        template<typename Ar>
        void Serialize(Ar& ar, HashMap<A, B>& map)
        {
            WriteMap<HashMap<A,B>>(ar, map);
        }
    };

    template<typename A, typename B>
    struct Serialize_NonTrivial<Archive<IStream>, HashMap<A, B>>
    {
        template<typename Ar>
        void Serialize(Ar& ar, HashMap<A, B>& map)
        {
            ReadMap<HashMap<A, B>, A, B>(ar, map);
        }
    };

    template<typename A, typename B>
    struct Serialize_NonTrivial<Archive<OStream>, ParallelHashMap<A, B>>
    {
        template<typename Ar>
        void Serialize(Ar& ar, ParallelHashMap<A, B>& map)
        {
            WriteMap<ParallelHashMap<A, B>>(ar, map);
        }
    };

    template<typename A, typename B>
    struct Serialize_NonTrivial<Archive<IStream>, ParallelHashMap<A, B>>
    {
        template<typename Ar>
        void Serialize(Ar& ar, ParallelHashMap<A, B>& map)
        {
            ReadMap<ParallelHashMap<A, B>, A, B>(ar, map);
        }
    };

    template<typename A, typename B>
    struct Serialize_NonTrivial<Archive<OStream>, ParallelHashMapMutex<A, B>>
    {
        template<typename Ar>
        void Serialize(Ar& ar, ParallelHashMapMutex<A, B>& map)
        {
            WriteMap<ParallelHashMapMutex<A, B>>(ar, map);
        }
    };

    template<typename A, typename B>
    struct Serialize_NonTrivial<Archive<IStream>, ParallelHashMapMutex<A, B>>
    {
        template<typename Ar>
        void Serialize(Ar& ar, ParallelHashMapMutex<A, B>& map)
        {
            ReadMap<ParallelHashMapMutex<A, B>, A, B>(ar, map);
        }
    };
}
#endif
