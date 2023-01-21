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
#include "Data/Streams.hpp"

namespace Lina
{
    class SetSerialization
    {
    public:
        template <typename T> static inline void WriteSet_PT(OStream& stream, T& set)
        {
            const uint32 size = static_cast<uint32>(set.size());
            stream << size;

            for (auto& i : set)
                stream << i;
        }

        template <typename T, typename U> static inline void ReadSet_PT(IStream& stream, T& set)
        {
            uint32 size = 0;
            stream >> size;
            set.clear();

            for (uint32 i = 0; i < size; i++)
            {
                U u;
                stream >> u;
                set.insert(u);
            }
        }

        template <typename T> static inline void WriteSet_OBJ(OStream& stream, T& set)
        {
            const uint32 size = static_cast<uint32>(set.size());
            stream << size;

            for (auto& i : set)
                i.SaveToStream(stream);
        }

        template <typename T, typename U> static inline void ReadSet_OBJ(IStream& stream, T& set)
        {
            uint32 size = 0;
            stream >> size;
            set.clear();

            for (uint32 i = 0; i < size; i++)
            {
                U u;
                u.LoadFromStream(stream);
                set.insert(u);
            }
        }

        template <typename T> static inline void SaveToStream_PT(OStream& stream, Set<T>& set)
        {
            WriteSet_PT(stream, set);
        }
        template <typename T> static inline void LoadFromStream_PT(IStream& stream, Set<T>& set)
        {
            ReadSet_PT<Set<T>, T>(stream, set);
        }

        template <typename T> static inline void SaveToStream_OBJ(OStream& stream, Set<T>& set)
        {
            WriteSet_OBJ(stream, set);
        }
        template <typename T> static inline void LoadFromStream_OBJ(IStream& stream, Set<T>& set)
        {
            ReadSet_OBj<Set<T>, T>(stream, set);
        }

        template <typename T> static inline void SaveToStream_PT(OStream& stream, HashSet<T>& set)
        {
            WriteSet_PT(stream, set);
        }
        template <typename T> static inline void LoadFromStream_PT(IStream& stream, HashSet<T>& set)
        {
            ReadSet_PT<HashSet<T>, T>(stream, set);
        }

        template <typename T> static inline void SaveToStream_OBJ(OStream& stream, HashSet<T>& set)
        {
            WriteSet_OBJ(stream, set);
        }
        template <typename T> static inline void LoadFromStream_OBJ(IStream& stream, HashSet<T>& set)
        {
            ReadSet_OBj<HashSet<T>, T>(stream, set);
        }
    };

} // namespace Lina

#endif