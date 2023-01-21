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

#ifndef MemoryManager_HPP
#define MemoryManager_HPP

#include "Data/Vector.hpp"
#include "Data/String.hpp"
#include "Core/SizeDefinitions.hpp"
#include <memallocators/Allocator.h>

namespace Lina
{
#define MEMMANAGER_MIN_FREELIST_SIZE 16

    enum class AllocatorPoolGrowPolicy
    {
        NoGrow,
        UseInitialSize,
        UseRequestedSize,
        UseDoubleInitialSize,
        UseDoubledRequestedSize,
    };

    enum class AllocatorType
    {
        Linear,
        Stack,
        Pool,
        FreeList,
    };

    struct AllocatorWrapper
    {
        Allocator* allocator = nullptr;
        bool       IsAvailable(size_t size);
        size_t     minSizeRequirement = 0;
    };

    class MemoryManager;
    class MemoryAllocatorPool
    {
    public:
        void* Allocate(size_t size);
        void  Free(void* ptr);
        void  Reset();

        inline size_t GetMinSizeRequirement()
        {
            return m_minSizeRequirement;
        }

    protected:
        friend class MemoryManager;

        MemoryAllocatorPool(AllocatorType type, AllocatorPoolGrowPolicy growPolicy, size_t initialSize, size_t initialUserData, uint8 maxGrowSize);
        virtual ~MemoryAllocatorPool();

    private:
        static Allocator* CreateAllocator(AllocatorType type, size_t size, size_t userData = 0);
        static String     GetAllocatorName(AllocatorType type);
        void              AddAllocator(size_t size);

    protected:
        AllocatorPoolGrowPolicy  m_growPolicy         = AllocatorPoolGrowPolicy::NoGrow;
        AllocatorType            m_type               = AllocatorType::FreeList;
        size_t                   m_minSizeRequirement = 0;
        int                      m_maxGrowSize        = 0;
        size_t                   m_initialSize        = 0;
        size_t                   m_initialUserData    = 0;
        uint32                   m_currentAllocator;
        Vector<AllocatorWrapper> m_allocators;
    };

    class MemoryManager
    {
    public:
        inline static MemoryManager& Get()
        {
            static MemoryManager instance;
            return instance;
        }

        /// <summary>
        /// For pool allocator, use size = totalSize, userData = chunkSize.
        /// </summary>
        /// <returns></returns>
        MemoryAllocatorPool* CreateAllocatorPool(AllocatorType type, size_t size, AllocatorPoolGrowPolicy growPolicy, uint8 maxGrowSize = 0, size_t userData = 0);
        void                 DestroyAllocatorPool(MemoryAllocatorPool* pool);

        void* AllocateGlobal(size_t size, size_t alignment = 8);
        void  FreeGlobal(void* ptr);

        friend class Engine;

        MemoryManager() = default;
        ~MemoryManager();

        void Shutdown();

    private:
        MemoryAllocatorPool*         m_globalPool = nullptr;
        Vector<MemoryAllocatorPool*> m_allocatorPools;
    };

} // namespace Lina

#endif