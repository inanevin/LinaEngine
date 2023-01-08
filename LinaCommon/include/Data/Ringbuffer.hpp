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

#ifndef DataStructuresRingbuffer_HPP
#define DataStructuresRingbuffer_HPP

#include "Data/Array.hpp"
#include "Data/Mutex.hpp"

namespace Lina
{
    // Lock-free ring buffer using a fixed-size array and atomic indices
    template <typename T, typename int N> class RingBuffer
    {
    public:
        RingBuffer() : readIndex(0), writeIndex(0)
        {
        }

        // Push an element to the end of the buffer
        void Push(const T& element)
        {
            // Get the next write index
            int wi     = writeIndex.load(std::memory_order_relaxed);
            int nextWI = (wi + 1) % N;

            // Spin until there is space in the buffer
            while (nextWI == readIndex.load(std::memory_order_acquire))
            {
                // Optional: do something else while waiting
                int a = 5;
            }

            // Write the element to the buffer
            buffer[wi] = element;

            // Increment the write index
            writeIndex.store(nextWI, std::memory_order_release);
        }

        // Pop an element from the front of the buffer
        bool Pop(T* element)
        {
            // Get the next read index
            int ri = readIndex.load(std::memory_order_relaxed);

            // Check if the buffer is empty
            if (ri == writeIndex.load(std::memory_order_acquire))
                return false;

            // Read the element from the buffer
            *element = buffer[ri];

            // Increment the read index
            readIndex.store((ri + 1) % N, std::memory_order_release);

            return true;
        }

    private:
        Array<T, N> buffer;
        Atomic<int> readIndex;
        Atomic<int> writeIndex;
    };

} // namespace Lina

#endif
