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

#include "Core/RenderEngine.hpp"
#include "Log/Log.hpp"
#include "ECS/Components/DecalComponent.hpp"
#include "ECS/Components/SkyComponent.hpp"
#include "ECS/Components/LightComponent.hpp"
#include "ECS/Components/MeshComponent.hpp"
#include "ECS/Components/SpriteComponent.hpp"
#include "ECS/Components/ParticleComponent.hpp"
#include "Data/RQueue.hpp"
#include "Data/CommandBuffer.hpp"
#include "Data/CommandPool.hpp"
#include "Data/RenderPass.hpp"
#include "Data/Framebuffer.hpp"
#include "Data/Semaphore.hpp"
#include "Data/Fence.hpp"
#include "EventSystem/EventSystem.hpp"
#include "EventSystem/GraphicsEvents.hpp"

namespace Lina::Graphics
{

    #define RETURN_NOTINITED  if(!m_initedSuccessfully) return

    RQueue              m_graphicsQueue;
    CommandPool         m_pool;
    CommandBuffer       m_buffer;
    RenderPass          m_renderPass;
    SubPass             m_subpass;
    Vector<Framebuffer> m_framebuffers;
    Fence               m_renderFence;
    Semaphore           m_renderSemaphore;
    Semaphore           m_presentSemaphore;

    void RenderEngine::Initialize(ApplicationInfo& appInfo)
    {
        LINA_TRACE("[Initialization] -> Render Engine ({0})", typeid(*this).name());
        m_appInfo = appInfo;

        Window::s_instance   = &m_window;
        Backend::s_instance  = &m_backend;
        m_initedSuccessfully = m_window.Initialize(appInfo);


        m_initedSuccessfully = m_backend.Initialize(appInfo);
        Event::EventSystem::Get()->Connect<Event::ESwapchainRecreated, &RenderEngine::OnSwapchainRecreated>(this);

        if (!m_initedSuccessfully)
        {
            LINA_ERR("[Render Engine] -> Initialization failed, no rendering will occur!");
            return;
        }

        m_skySystem.Initialize("Sky System");
        m_particleSystem.Initialize("Particle System");
        m_decalSystem.Initialize("Decal System");
        m_spriteSystem.Initialize("Sprite System");
        m_lightingSystem.Initialize("Lighting System");
        m_meshSystem.Initialize("Mesh System");

        m_graphicsQueue.Get(m_backend.GetQueueFamilyIndex(QueueFamilies::Graphics));
        m_pool = CommandPool{.familyIndex = m_graphicsQueue._family, .flags = CommandPoolFlags::Reset}.Create();

        m_buffer = CommandBuffer{.count = 1, .level = CommandBufferLevel::Primary}
                       .Create(m_pool._ptr);

        Attachment att = Attachment{.format = static_cast<Format>(m_backend.m_swapchain.format), .loadOp = LoadOp::Clear}.Create();
        SubPass    sp  = SubPass{.bindPoint = PipelineBindPoint::Graphics}.AddColorAttachmentRef(0, ImageLayout::ColorOptimal).Create();

        m_renderPass = RenderPass().AddAttachment(att).AddSubpass(sp).Create();
        m_framebuffers.reserve(m_backend.m_swapchain._imageViews.size());

        const Vector2i size = m_window.GetSize();

        for (auto iv : m_backend.m_swapchain._imageViews)
        {
            Framebuffer fb = Framebuffer{.width = static_cast<uint32>(size.x), .height = static_cast<uint32>(size.y), .layers = 1}.AttachRenderPass(m_renderPass).Create(iv);
            m_framebuffers.push_back(fb);
        }

        m_presentSemaphore.Create();
        m_renderSemaphore.Create();
        m_renderFence = Fence{.flags = FenceFlags::Signaled}.Create();
    }

    void RenderEngine::SyncRenderData()
    {
        RETURN_NOTINITED;

        PROFILER_FUNC(PROFILER_THREAD_SIMULATION);

        m_systemList.UpdateSystems(0.0f);
    }

    void RenderEngine::Clear()
    {
        RETURN_NOTINITED;

    }

    void RenderEngine::Render()
    {
    
        m_renderFence.Wait(true, 1.0);
        m_renderFence.Reset();

        uint32 imageIndex = m_backend.m_swapchain.AcquireNextImage(1.0, m_presentSemaphore);
        m_buffer.Reset();

        m_buffer.Begin(CommandBufferFlags::OneTimeSubmit);

        m_renderPass.Begin(ClearValue{.clearColor = Color::Red}, m_framebuffers[imageIndex], m_buffer);
        m_renderPass.End(m_buffer);
        m_buffer.End();
        m_graphicsQueue.Submit(m_presentSemaphore, m_renderSemaphore, m_renderFence, m_buffer, 1);
        m_graphicsQueue.Present(m_renderSemaphore, &imageIndex);

        PROFILER_SCOPE_START("Render Sky", PROFILER_THREAD_RENDER);
        m_skySystem.Render();
        PROFILER_SCOPE_END("Render Sky", PROFILER_THREAD_RENDER);

        PROFILER_SCOPE_START("Render Particles", PROFILER_THREAD_RENDER);
        m_particleSystem.Render();
        PROFILER_SCOPE_END("Render Particles", PROFILER_THREAD_RENDER);

        PROFILER_SCOPE_START("Render Decals", PROFILER_THREAD_RENDER);
        m_decalSystem.Render();
        PROFILER_SCOPE_END("Render Decals", PROFILER_THREAD_RENDER);

        PROFILER_SCOPE_START("Render Sprites", PROFILER_THREAD_RENDER);
        m_spriteSystem.Render();
        PROFILER_SCOPE_END("Render Sprites", PROFILER_THREAD_RENDER);

        PROFILER_SCOPE_START("Render Meshes", PROFILER_THREAD_RENDER);
        m_meshSystem.Render();
        PROFILER_SCOPE_END("Render Meshes", PROFILER_THREAD_RENDER);
    }

    void RenderEngine::Shutdown()
    {
        RETURN_NOTINITED;

        LINA_TRACE("[Shutdown] -> Render Engine ({0})", typeid(*this).name());

        m_renderFence.Wait();
        m_renderFence.Destroy();
        m_renderSemaphore.Destroy();
        m_presentSemaphore.Destroy();

        m_renderPass.Destroy();
        m_pool.Destroy();

        for (int i = 0; i < m_framebuffers.size(); i++)
            m_framebuffers[i].Destroy();

        m_window.Shutdown();
        m_backend.Shutdown();
    }

    void RenderEngine::OnSwapchainRecreated(const Event::ESwapchainRecreated& ev)
    {
        const Vector2i size = m_window.GetSize();
        for (int i = 0; i < m_framebuffers.size(); i++)
        {
            Framebuffer& fb = m_framebuffers[i];
            fb.Destroy();
            fb = Framebuffer{.width = static_cast<uint32>(size.x), .height = static_cast<uint32>(size.y), .layers = 1}.AttachRenderPass(m_renderPass).Create(m_backend.m_swapchain._imageViews[i]);
        }
    }

} // namespace Lina::Graphics
