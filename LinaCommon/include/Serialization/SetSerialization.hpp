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

#ifndef SetSerialization_HPP
#define SetSerialization_HPP

#include "Data/Set.hpp"
#include "Data/HashSet.hpp"
#include "Archive.hpp"

namespace Lina::Serialization
{
    template <typename T> void WriteSet(Archive<OStream>& ar, T& set)
    {
        const uint32 size = static_cast<uint32>(set.size());
        ar(size);

        for (auto& i : set)
            ar(i);
    }

    template <typename T, typename U> void ReadSet(Archive<IStream>& ar, T& set)
    {
        uint32 size = 0;
        ar(size);
        set.clear();

        for (uint32 i = 0; i < size; i++)
        {
            U u;
            ar(u);
            set.insert(u);
        }
    }

    template <typename Ar, typename T> void SaveOther(Ar& ar, Set<T>& set)
    {
        WriteSet<Set<T>>(ar, set);
    }
    template <typename Ar, typename T> void LoadOther(Ar& ar, Set<T>& set)
    {
        ReadSet<Set<T>, T>(ar, set);
    }

    template <typename Ar, typename T> void SaveOther(Ar& ar, HashSet<T>& set)
    {
        WriteSet<HashSet<T>>(ar, set);
    }
    template <typename Ar, typename T> void LoadOther(Ar& ar, HashSet<T>& set)
    {
        ReadSet<HashSet<T>, T>(ar, set);
    }

} // namespace Lina::Serialization

#endif