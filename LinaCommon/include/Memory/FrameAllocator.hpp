/*
This file is a part of: Lina AudioEngine
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
Class: FrameAllocator

A memory allocator of type that is suitable for single frame operations, and would be cleared at the
beginning of each frame.

Timestamp: 12/19/2020 1:37:43 AM
*/

#pragma once

#ifndef FrameAllocator_HPP
#define FrameAllocator_HPP

// Headers here.
#include "MemoryAllocator.hpp"

namespace Lina
{
    class FrameAllocator : public MemoryAllocator
    {

    public:
        FrameAllocator::FrameAllocator(const std::size_t totalSize) : MemoryAllocator(totalSize)
        {
        }
        virtual ~FrameAllocator();

        virtual void* Allocate(const std::size_t size, const std::size_t alignment = 0) override;
        virtual void  Free(void* ptr) override;
        virtual void  Init() override;
        virtual void  Reset();

    protected:
        void*       m_start_ptr = nullptr;
        std::size_t m_offset    = 0;

    private:
        FrameAllocator(FrameAllocator& linearAllocator);
    };
} // namespace Lina

#endif
