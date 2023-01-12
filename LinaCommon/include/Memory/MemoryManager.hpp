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

#include "Data/String.hpp"
#include "Data/Vector.hpp"
#include "Log/Log.hpp"
#include "Data/Mutex.hpp"
#include "Data/HashMap.hpp"
#include "Utility/StringId.hpp"
#include <memallocators/LinearAllocator.h>
#include <memallocators/PoolAllocator.h>
#include <memallocators/StackAllocator.h>

namespace Lina::Memory
{
    struct LinearBlock
    {
        LinearAllocator* allocator = nullptr;
        size_t           allocated = 0;
        size_t           maxSize   = 0;

        bool IsAvailable(size_t size) const
        {
            return maxSize - allocated > size;
        }
    };

    struct LinearBlockList
    {
        Vector<LinearBlock> blocks;
        size_t              size = 0;
    };

    struct PoolBlock
    {
        String         name       = "";
        PoolAllocator* allocator  = nullptr;
        size_t         allocated  = 0;
        size_t         maxSize    = 0;
        uint32         chunkCount = 0;

        bool IsAvailable(size_t size) const
        {
            return maxSize - allocated > size;
        }
    };

    class MemoryManager
    {
    public:
        inline static MemoryManager* Get()
        {
            return s_instance;
        }

        void  CreateLinearBlockList(StringID sid, size_t size);
        void* GetFromLinearBlockList(StringID sid, size_t size);
        void  FlushLinearBlockList(StringID sid);
        void  FreeLinearBlockList(StringID sid);

        template <typename T> T* GetFromPoolBlock()
        {
            LOCK_GUARD(m_poolMtx);

            const TypeID tid       = GetTypeID<T>();
            PoolBlock*   block     = nullptr;
            uint32       poolIndex = (uint32)0;
            const size_t size      = sizeof(T);

            auto& vec = m_poolBlocks[tid];

            if (vec.empty())
            {
                LINA_ERR("[Memory Manager] -> Could not fetch from pool block because no block exists of this type, create one first!");
                return nullptr;
            }

            const uint32 maxSize = static_cast<uint32>(vec.size());
            for (uint32 i = 0; i < maxSize; i++)
            {
                bool  found = false;
                auto& bl    = vec[i];
                if (bl.IsAvailable(size))
                {
                    block     = &bl;
                    found     = true;
                    poolIndex = i;
                    break;
                }
                if (found)
                    break;
            }

            if (block == nullptr)
            {
                PoolBlock& first = vec[0];
                block            = CreatePoolBlock<T>(first.chunkCount, first.name);
                poolIndex        = static_cast<uint32>(vec.size() - 1);
            }

            void* ptr = block->allocator->Allocate(size);
            LINA_ASSERT(ptr != nullptr, "Could not allocate from block!");

            block->allocated += size;
            T* newPtr                = new (ptr) T();
            newPtr->m_allocPoolIndex = poolIndex;
            return newPtr;
        }

        template <typename T> void FreeFromPoolBlock(void* ptr, uint32 poolIndex)
        {
            PoolBlock& block = m_poolBlocks[GetTypeID<T>()][poolIndex];
            block.allocator->Free(ptr);
            block.allocated -= sizeof(T);
        }

        PoolBlock* CreatePoolBlock(TypeID tid, uint32 chunkSize, uint32 chunkCount, const String& blockName)
        {
            PoolBlock    block;
            const size_t totalSize = static_cast<size_t>(chunkSize * chunkCount);
            block.allocator        = new PoolAllocator(totalSize, chunkSize);
            block.allocator->Init();
            block.maxSize    = totalSize;
            block.chunkCount = chunkCount;
            block.name       = blockName;
            m_poolBlocks[tid].push_back(block);
            return &m_poolBlocks[tid][m_poolBlocks[tid].size() - 1];
        }

        template <typename T> PoolBlock* CreatePoolBlock(uint32 chunkCount, const String& blockName)
        {
            return CreatePoolBlock(GetTypeID<T>(), sizeof(T), chunkCount, blockName);
        }

        template <typename T> bool PoolBlockExists()
        {
            return m_poolBlocks.find(GetTypeID<T>()) != m_poolBlocks.end();
        }

    private:
        friend class Engine;

        static MemoryManager* s_instance;

        MemoryManager()  = default;
        ~MemoryManager() = default;

        void  Initialize();
        void  Shutdown();
        void  PrintBlockInfos();
        void* GetFromLinearBlock(LinearBlock& block, size_t size);

    private:
        Mutex                              m_poolMtx;
        Mutex                              m_linearMtx;
        HashMap<TypeID, Vector<PoolBlock>> m_poolBlocks;
        HashMap<StringID, LinearBlockList> m_linearBlocks;
    };
} // namespace Lina::Memory

#endif