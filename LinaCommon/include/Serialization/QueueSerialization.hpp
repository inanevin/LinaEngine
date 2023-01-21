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

#ifndef QueueSerialization_HPP
#define QueueSerialization_HPP

#include "Data/Queue.hpp"
#include "Data/PriorityQueue.hpp"
#include "Data/Streams.hpp"

namespace Lina
{
    class QueueSerialization
    {
    public:
        template <typename T> static inline void SaveToStream_PT(OStream& stream, Queue<T>& q)
        {
            const uint32 size = static_cast<uint32>(q.size());
            stream << size;

            Queue<T> rep = q;

            for (uint32 i = 0; i < size; i++)
            {
                T t = rep.front();
                rep.pop();
                stream << t;
            }
        }

        template <typename T> static inline void SaveToStream_OBJ(OStream& stream, Queue<T>& q)
        {
            const uint32 size = static_cast<uint32>(q.size());
            stream << size;

            Queue<T> rep = q;

            for (uint32 i = 0; i < size; i++)
            {
                T t = rep.front();
                rep.pop();
                t.SaveToStream(stream);
            }
        }

        template <typename T> static inline void LoadFromStream_PT(IStream& stream, Queue<T>& q)
        {
            uint32 size = 0;
            stream >> size;

            q = Queue<T>();

            for (uint32 i = 0; i < size; i++)
            {
                T t;
                stream >> t;
                q.push(t);
            }
        }

        template <typename T> static inline void LoadFromStream_OBJ(IStream& stream, Queue<T>& q)
        {
            uint32 size = 0;
            stream >> size;

            q = Queue<T>();

            for (uint32 i = 0; i < size; i++)
            {
                T t;
                t.LoadFromStream(stream);
                q.push(t);
            }
        }

        template <typename T, typename Container, typename Compare> static inline void SaveToStream_PT(OStream& stream, PriorityQueue<T, Container, Compare>& q)
        {
            const uint32 size = static_cast<uint32>(q.size());
            stream << size;

            Queue<T, Container> rep = q;

            for (uint32 i = 0; i < size; i++)
            {
                T t = rep.front();
                rep.pop();
                stream << t;
            }
        }

        template <typename T, typename Container, typename Compare> static inline void SaveToStream_OBJ(OStream& stream, PriorityQueue<T, Container, Compare>& q)
        {
            const uint32 size = static_cast<uint32>(q.size());
            stream << size;

            Queue<T, Container> rep = q;

            for (uint32 i = 0; i < size; i++)
            {
                T t = rep.front();
                rep.pop();
                t.SaveToStream(stream);
            }
        }

        template <typename T, typename Container, typename Compare> static inline void LoadFromStream_PT(IStream& stream, PriorityQueue<T, Container, Compare>& q)
        {
            uint32 size = 0;
            stream >> size;

            q = Queue<T, Container>();

            for (uint32 i = 0; i < size; i++)
            {
                T t;
                stream >> t;
                q.push(t);
            }
        }

        template <typename T, typename Container, typename Compare> static inline void LoadFromStream_OBJ(IStream& stream, PriorityQueue<T, Container, Compare>& q)
        {
            uint32 size = 0;
            stream >> size;

            q = Queue<T, Container>();

            for (uint32 i = 0; i < size; i++)
            {
                T t;
                t.LoadFromStream(stream);
                q.push(t);
            }
        }
    };

} // namespace Lina::Serialization

#endif
