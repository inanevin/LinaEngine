/*
This file is a part of: Lina Engine
https://github.com/inanevin/Lina

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

Custom Memory Allocators: Copyright (c) 2016 Mariano Trebino

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

/*
Class: MemoryAllocator

Base class for various custom memory allocators.

Timestamp: 12/19/2020 1:28:46 AM
*/

#pragma once

#ifndef MemoryAllocator_HPP
#define MemoryAllocator_HPP

// Headers here.
#define NOMINMAX

#include <cstddef>

namespace Lina
{
    class MemoryAllocator
    {

    public:
        MemoryAllocator(const std::size_t totalSize) : m_totalSize{totalSize}, m_used{0}, m_peak{0}
        {
        }
        virtual ~MemoryAllocator()
        {
            m_totalSize = 0;
        }

        virtual void* Allocate(const std::size_t size, const std::size_t alignment = 0) = 0;
        virtual void  Free(void* ptr)                                                   = 0;
        virtual void  Init()                                                            = 0;

    protected:
        std::size_t m_totalSize = 0;
        std::size_t m_used      = 0;
        std::size_t m_peak      = 0;
    };
} // namespace Lina

#endif
