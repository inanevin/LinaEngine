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

#include "RenderData.hpp"
#include "Graphics/PipelineObjects/CommandPool.hpp"
#include "Graphics/PipelineObjects/CommandBuffer.hpp"
#include "Graphics/PipelineObjects/DescriptorPool.hpp"
#include "Core/Command.hpp"
#include "Core/CommonEngine.hpp"

namespace Lina::Graphics
{
    class GUIBackend;
    class Swapchain;
    class WindowManager;
    class RenderEngine;

    class Renderer
    {

    public:
        Renderer()          = default;
        virtual ~Renderer() = default;

        inline const Bitmask16& GetMask() const
        {
            return m_mask;
        }

        inline void SetRenderMask(const Bitmask16& mask)
        {
            m_mask = mask;
        }

    protected:
        friend class RenderEngine;

        virtual bool           Initialize(RenderEngine* renderEngine);
        virtual void           Shutdown();
        virtual void           SyncData();
        virtual void           Tick(){};
        virtual void           OnPostPresent(VulkanResult res){};
        virtual CommandBuffer* Render(uint32 frameIndex, Fence& fence) = 0;
        virtual void           AcquiredImageInvalid(uint32 frameIndex){};

        virtual bool AcquireImage(uint32 frameIndex)
        {
            return true;
        };

        virtual Swapchain* GetSwapchain() const
        {
            return nullptr;
        };

        virtual uint32 GetAcquiredImage() const
        {
            return 0;
        };

        inline RendererType GetType() const
        {
            return m_type;
        }

        inline void SetUserData(uint32 data)
        {
            m_userData = data;
        }

    protected:
        RenderEngine*         m_renderEngine = nullptr;
        RendererType          m_type         = RendererType::None;
        Bitmask16             m_mask         = 0;
        Vector<SimpleAction>  m_syncedActions;
        CommandPool           m_cmdPool;
        DescriptorPool        m_descriptorPool;
        Vector<CommandBuffer> m_cmds;
        bool                  m_initialized = false;
        uint32                m_userData    = 0;
    };

} // namespace Lina::Graphics

#endif
