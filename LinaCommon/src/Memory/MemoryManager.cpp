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

#include "Memory/MemoryManager.hpp"
#include "Log/Log.hpp"
#include "Math/Math.hpp"

namespace Lina::Memory
{

#define STATIC_BLOCK_SIZE 32000
#define BYTES_TO_MB(x)    x * 0.000001f

    MemoryManager* MemoryManager::s_instance = nullptr;

    void MemoryManager::CreateLinearBlockList(StringID sid, size_t size)
    {
        LinearBlock block;
        block.maxSize   = size;
        block.allocator = new LinearAllocator(size);
        block.allocator->Init();

        auto& bl = m_linearBlocks[sid];
        bl.size  = size;
        bl.blocks.push_back(block);

        LINA_TRACE("[Memory Manager] -> Created linear allocator block for: {0}, size: {1} (mb)", sid, BYTES_TO_MB(size));
    }

    void* MemoryManager::GetFromLinearBlock(LinearBlock& block, size_t size)
    {
        block.allocated += size;
        return block.allocator->Allocate(size);
    }

    void* MemoryManager::GetFromLinearBlockList(StringID sid, size_t size)
    {
        LOCK_GUARD(m_linearMtx);

        auto& bl = m_linearBlocks[sid];
        for (auto& block : bl.blocks)
        {
            if (block.IsAvailable(size))
                return GetFromLinearBlock(block, size);
        }

        const uint32 targetSize =  static_cast<uint32>(Math::Max(size, bl.size));
        LinearBlock  block;
        block.maxSize   = targetSize;
        block.allocator = new LinearAllocator(targetSize);
        block.allocator->Init();
        bl.blocks.push_back(block);

        auto data = GetFromLinearBlock(bl.blocks[bl.blocks.size() - 1], size);
        return data;
    }

    void MemoryManager::FlushLinearBlockList(StringID sid)
    {
        auto&  bl         = m_linearBlocks[sid];
        size_t totalFreed = 0;

        Vector<LinearBlock> newBlocks;

        for (auto& b : bl.blocks)
        {
            totalFreed += b.maxSize;

            // remove outliers, only the ones with the initial reserved size are allowed to stay.
            if (b.maxSize > bl.size)
                delete b.allocator;
            else
            {
                newBlocks.push_back(b);
                b.allocated = 0;
                b.allocator->Reset();
            }
        }

        bl.blocks.clear();
        bl.blocks = newBlocks;

        LINA_TRACE("[Memory Manager] -> Flushed linear block list: {0}, total resetted size: {1} (mb)", sid, BYTES_TO_MB(totalFreed));
    }

    void MemoryManager::FreeLinearBlockList(StringID sid)
    {
        LOCK_GUARD(m_linearMtx);

        auto&  bl         = m_linearBlocks[sid];
        size_t totalFreed = 0;
        for (auto& b : bl.blocks)
        {
            totalFreed += b.maxSize;
            delete b.allocator;
        }

        LINA_TRACE("[Memory Manager] -> Cleaning linear block list: {0}, total freed size: {1} (mb)", sid, BYTES_TO_MB(totalFreed));
        bl.blocks.clear();
    }

    void MemoryManager::Initialize()
    {
        LINA_TRACE("[Initialization] -> Memory Manager ({0})", typeid(*this).name());
    }

    void MemoryManager::Shutdown()
    {
        LINA_TRACE("[Shutdown] -> Memory Manager ({0})", typeid(*this).name());

        for (auto& [sid, bl] : m_linearBlocks)
        {
            LINA_TRACE("[Memory Manager] -> Freeing static linear block list, list size: {0}, total max size per item: {1} (mb)", bl.blocks.size(), BYTES_TO_MB(bl.size));
            FreeLinearBlockList(sid);
        }

        for (auto& [sid, vec] : m_poolBlocks)
        {
            for (auto& bl : vec)
            {
                LINA_TRACE("[Memory Manager] -> Freeing pool block of size: {0} (mb)", BYTES_TO_MB(bl.maxSize));
                delete bl.allocator;
            }
        }
    }

    void MemoryManager::PrintBlockInfos()
    {
        int counter = 0;
        for (auto& [sid, bl] : m_linearBlocks)
        {
            LINA_TRACE("Linear Block {0}", counter);

            for (auto& b : bl.blocks)
            {
                LINA_TRACE("  * Allocated: {0} (mb)", BYTES_TO_MB(b.allocated));
                LINA_TRACE("  * Remaining: {0} (mb)", BYTES_TO_MB((b.maxSize - b.allocated)));
                LINA_TRACE("  * Max: {0} (mb)", BYTES_TO_MB(b.maxSize));
            }

            counter++;
        }

        for (auto& [sid, vec] : m_poolBlocks)
        {
            counter = 0;
            for (auto& bl : vec)
            {
                LINA_TRACE("Pool Block: {0} - {1}", bl.name, counter);
                LINA_TRACE("  * Allocated: {0} (mb)", BYTES_TO_MB(bl.allocated));
                LINA_TRACE("  * Remaining: {0} (mb)", BYTES_TO_MB((bl.maxSize - bl.allocated)));
                LINA_TRACE("  * Max: {0} (mb)", BYTES_TO_MB(bl.maxSize));
                counter++;
            }
        }
    }

} // namespace Lina::Memory