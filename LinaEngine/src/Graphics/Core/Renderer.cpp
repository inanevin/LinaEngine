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

#include "Graphics/Core/Renderer.hpp"
#include "Profiling/Profiler.hpp"
#include "Graphics/Core/Backend.hpp"

namespace Lina::Graphics
{
    bool Renderer::Initialize(GUIBackend* guiBackend, WindowManager* windowManager, RenderEngine* eng)
    {
        if (m_initialized)
        {
            LINA_ERR("[Renderer] -> Failed initialization, already initialized!");
            return false;
        }

        m_initialized   = true;
        m_renderEngine  = eng;
        m_windowManager = windowManager;
        m_guiBackend    = guiBackend;

        m_cmdPool = CommandPool{.familyIndex = Backend::Get()->GetGraphicsQueue()._family, .flags = GetCommandPoolCreateFlags(CommandPoolFlags::Reset)};
        m_cmdPool.Create();

        const uint32 imageSize = static_cast<uint32>(Backend::Get()->GetMainSwapchain()._images.size());

        for (uint32 i = 0; i < imageSize; i++)
        {
            CommandBuffer buf = CommandBuffer{.count = 1, .level = CommandBufferLevel::Primary};
            m_cmds.push_back(buf);
            m_cmds[i].Create(m_cmdPool._ptr);
        }

        m_descriptorPool = DescriptorPool{
            .maxSets = 10,
            .flags   = DescriptorPoolCreateFlags::None,
        };
        m_descriptorPool.AddPoolSize(DescriptorType::UniformBuffer, 10).AddPoolSize(DescriptorType::UniformBufferDynamic, 10).AddPoolSize(DescriptorType::StorageBuffer, 10).AddPoolSize(DescriptorType::CombinedImageSampler, 10).Create();

        return true;
    }

    void Renderer::Shutdown()
    {
    }

    void Renderer::SyncData()
    {
        PROFILER_FUNC(PROFILER_THREAD_MAIN);

        // Sync shared data.
        for (auto c : m_syncedActions)
        {
            c.Action();

            if (c.OnExecuted)
                c.OnExecuted();
        }

        m_syncedActions.clear();
    }
} // namespace Lina::Graphics
