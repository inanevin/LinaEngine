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
#include "Data/Streams.hpp"

namespace Lina
{
    class HashMapSerialization
    {
    public:
        template <typename T> static inline void WriteMap_PT(OStream& stream, T& map)
        {
            const uint32 size = static_cast<uint32>(map.size());
            stream << size;

            for (auto& [key, val] : map)
            {
                stream << key;
                stream << val;
            }
        }

        template <typename T> static inline void WriteMap_OBJ(OStream& stream, T& map)
        {
            const uint32 size = static_cast<uint32>(map.size());
            stream << size;

            for (auto& [key, val] : map)
            {
                stream << key;
                val.SaveToStream(stream);
            }
        }

        template <typename T, typename U, typename V> static inline void ReadMap_PT(IStream& stream, T& map)
        {
            uint32 size = 0;
            stream >> size;
            map.clear();

            for (uint32 i = 0; i < size; i++)
            {
                U key = U();
                V val = V();
                stream >> key;
                stream >> val;
                map[key] = val;
            }
        }

        template <typename T, typename U, typename V> static inline void ReadMap_OBJ(IStream& stream, T& map)
        {
            uint32 size = 0;
            stream >> size;
            map.clear();

            for (uint32 i = 0; i < size; i++)
            {
                U key = U();
                V val = V();
                stream >> key;
                val.LoadFromStream(stream);
                map[key] = val;
            }
        }

        /*********** HASH MAP *************/
        template <typename T, typename K> static inline void SaveToStream_PT(OStream& stream, HashMap<T, K>& map)
        {
            WriteMap_PT<HashMap<T, K>>(stream, map);
        }

        template <typename T, typename K> static inline void LoadFromStream_PT(IStream& stream, HashMap<T, K>& map)
        {
            ReadMap_PT<HashMap<T, K>, T, K>(stream, map);
        }

        template <typename T, typename K> static inline void SaveToStream_OBJ(OStream& stream, HashMap<T, K>& map)
        {
            WriteMap_OBj<HashMap<T, K>>(stream, map);
        }

        template <typename T, typename K> static inline void LoadFromStream_OBJ(IStream& stream, HashMap<T, K>& map)
        {
            ReadMap_OBJ<HashMap<T, K>, T, K>(stream, map);
        }

        /*********** PARALLEL MUTEX *************/
        template <typename T, typename K> static inline void SaveToStream_PT(OStream& stream, ParallelHashMapMutex<T, K>& map)
        {
            WriteMap_PT<ParallelHashMapMutex<T, K>>(stream, map);
        }
        template <typename T, typename K> static inline void LoadFromStream_PT(IStream& stream, ParallelHashMapMutex<T, K>& map)
        {
            ReadMap_PT<ParallelHashMapMutex<T, K>, T, K>(stream, map);
        }

        template <typename T, typename K> static inline void SaveToStream_OBJ(OStream& stream, ParallelHashMapMutex<T, K>& map)
        {
            WriteMap_OBJ<ParallelHashMapMutex<T, K>>(stream, map);
        }
        template <typename T, typename K> static inline void LoadFromStream_OBJ(IStream& stream, ParallelHashMapMutex<T, K>& map)
        {
            ReadMap_OBJ<ParallelHashMapMutex<T, K>, T, K>(stream, map);
        }

        /*********** PARALLEL *************/
        template <typename T, typename K> static inline void SaveToStream_PT(OStream& stream, ParallelHashMap<T, K>& map)
        {
            WriteMap_PT<ParallelHashMap<T, K>>(stream, map);
        }
        template <typename T, typename K> static inline void LoadFromStream_PT(IStream& stream, ParallelHashMap<T, K>& map)
        {
            ReadMap_PT<ParallelHashMap<T, K>, T, K>(stream, map);
        }

        template <typename T, typename K> static inline void SaveToStream_OBJ(OStream& stream, ParallelHashMap<T, K>& map)
        {
            WriteMap_OBJ<ParallelHashMap<T, K>>(stream, map);
        }
        template <typename T, typename K> static inline void LoadFromStream_OBJ(IStream& stream, ParallelHashMap<T, K>& map)
        {
            ReadMap_OBJ<ParallelHashMap<T, K>, T, K>(stream, map);
        }
    };

} // namespace Lina
#endif
