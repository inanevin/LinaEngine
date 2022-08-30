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
#include "Archive.hpp"

namespace Lina::Serialization
{
    template<typename T>
    struct Serialize_NonTrivial<Archive<OStream>, Queue<T>>
    {
        template<typename Ar>
        void Serialize(Ar& ar, Queue<T>& q)
        {
            const uint32 size = static_cast<uint32>(q.size());
            ar.m_stream << size;

            Queue<T> rep = q;

            for (uint32 i = 0; i < size; i++)
            {
                T t = rep.front();
                rep.pop();
                ar(t);
            }
        }
    };

    template<typename T>
    struct Serialize_NonTrivial<Archive<IStream>, Queue<T>>
    {
        template<typename Ar>
        void Serialize(Ar& ar, Queue<T>& q)
        {
            uint32 size = 0;
            ar.m_stream >> size;

            q = Queue<T>();

            for (uint32 i = 0; i < size; i++)
            {
                T t;
                ar(t);
                q.push(t);
            }
        }
    };

    template<typename T, typename Container, typename Compare>
    struct Serialize_NonTrivial<Archive<OStream>, PriorityQueue<T, Container, Compare>>
    {
        template<typename Ar>
        void Serialize(Ar& ar, PriorityQueue<T, Container, Compare>& q)
        {
            const uint32 size = static_cast<uint32>(q.size());
            ar.m_stream << size;

            Queue<T, Container> rep = q;

            for (uint32 i = 0; i < size; i++)
            {
                T t = rep.front();
                rep.pop();
                ar(t);
            }
        }
    };

    template<typename T, typename Container, typename Compare>
    struct Serialize_NonTrivial<Archive<IStream>, PriorityQueue<T, Container, Compare>>
    {
        template<typename Ar>
        void Serialize(Ar& ar, PriorityQueue<T, Container, Compare>& q)
        {
            uint32 size = 0;
            ar.m_stream >> size;

            q = Queue<T, Container>();

            for (uint32 i = 0; i < size; i++)
            {
                T t;
                ar(t);
                q.push(t);
            }
        }
    };
}

#endif
