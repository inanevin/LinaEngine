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

#ifndef Renderer_HPP
#define Renderer_HPP

#include "Core/SizeDefinitions.hpp"
#include "Data/Mutex.hpp"
#include "PipelineObjects/CommandBuffer.hpp"
#include "PipelineObjects/Semaphore.hpp"
#include "Functional/Functional.hpp"

namespace Lina::Graphics
{
    class Swapchain;
    class CommandPool;
    class Semaphore;
    struct Frame;
    class Buffer;

    class Renderer
    {
    public:
        Renderer()  = default;
        virtual ~Renderer() = default;

        virtual void Initialize();
        virtual void Shutdown(){};
        virtual void Tick(){};
        virtual bool AcquireImage(Frame& frame, uint32 frameIndex);
        virtual void BeginCommandBuffer();
        virtual void RecordCommandBuffer(Frame& frame) = 0;
        virtual void EndCommandBuffer();
        virtual void SyncData(){};
        virtual void Stop();
        virtual void HandleOutOfDateImage() = 0;

        inline void SetSwapchain(Swapchain* swp)
        {
            m_swapchain = swp;
        }

        inline void SetCommandPool(CommandPool* pool)
        {
            m_commandPool = pool;
        }

        inline uint32 GetCurrentImageIndex()
        {
            return m_currentImageIndex;
        }

        inline CommandBuffer* GetCurrentCommandBuffer()
        {
            return &m_cmds[m_currentImageIndex];
        }

        inline Swapchain* GetSwapchain()
        {
            return m_swapchain;
        }

        inline Semaphore* GetCurrentSemaphore(uint32 frameIndex)
        {
            return &m_submitSemaphores[frameIndex];
        }

        inline void SetOnOutOfDateImageHandled(Delegate<void(Swapchain*)>&& del)
        {
            m_handledOutOfDateImage = del;
        }

    protected:
        Delegate<void(Swapchain* swp)> m_handledOutOfDateImage;
        Swapchain*                     m_swapchain   = nullptr;
        CommandPool*                   m_commandPool = nullptr;
        Vector<CommandBuffer>          m_cmds;
        Vector<Semaphore>              m_submitSemaphores;
        uint32                         m_currentImageIndex = 0;
        bool                           m_recreateSwapchain = false;
        Atomic<bool>                   m_stopped           = false;
    };

} // namespace Lina::Graphics

#endif
