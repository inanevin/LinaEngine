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

namespace Lina::Memory
{

#define STATIC_BLOCK_SIZE 32000

    MemoryManager* MemoryManager::s_instance = nullptr;

    void MemoryManager::Initialize()
    {
        LINA_TRACE("[Initialization] -> Memory Manager ({0})", typeid(*this).name());
        CreateStaticBlock();
    }

    StaticBlock* MemoryManager::CreateStaticBlock()
    {
        LINA_TRACE("[Memory Manager] -> Static block exceeded, allocating new block.");
        StaticBlock staticBlock;
        staticBlock.allocator = new LinearAllocator(STATIC_BLOCK_SIZE);
        staticBlock.allocator->Init();
        staticBlock.maxSize = STATIC_BLOCK_SIZE;
        m_staticBlocks.push_back(staticBlock);
        return m_staticBlocks.end();
    }

    void MemoryManager::Shutdown()
    {
        LINA_TRACE("[Shutdown] -> Memory Manager ({0})", typeid(*this).name());

        for (auto& bl : m_staticBlocks)
        {
            LINA_TRACE("[Memory Manager] -> Freeing static block of size: {0}", bl.maxSize);
            delete bl.allocator;
        }

        for (auto& [sid, vec] : m_poolBlocks)
        {
            for (auto& bl : vec)
            {
                LINA_TRACE("[Memory Manager] -> Freeing pool block of size: {0}", bl.maxSize);
                delete bl.allocator;
            }
        }

        m_staticBlocks.clear();
    }

    void MemoryManager::PrintStaticBlockInfo()
    {
        int counter = 0;
        for (auto& bl : m_staticBlocks)
        {
            LINA_TRACE("Static Block {0}", counter);
            LINA_TRACE("  * Allocated: {0}", bl.allocated);
            LINA_TRACE("  * Remaining: {0}", bl.maxSize - bl.allocated);
            LINA_TRACE("  * Max: {0}", bl.maxSize);
            counter++;
        }

        for (auto& [sid, vec] : m_poolBlocks)
        {
            counter = 0;
            for (auto& bl : vec)
            {
                LINA_TRACE("Pool Block: {0} - {1}", bl.name, counter);
                LINA_TRACE("  * Allocated: {0}", bl.allocated);
                LINA_TRACE("  * Remaining: {0}", bl.maxSize - bl.allocated);
                LINA_TRACE("  * Max: {0}", bl.maxSize);
                counter++;
            }
        }
    }

} // namespace Lina::Memory