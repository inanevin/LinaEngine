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

#include "Core/Renderer.hpp"
#include "Core/Window.hpp"
#include "Core/Backend.hpp"
#include "EventSystem/EventSystem.hpp"
#include "EventSystem/LevelEvents.hpp"

namespace Lina::Graphics
{
    Renderer* Renderer::s_instance = nullptr;

#define RENDERABLES_STACK_SIZE 1000

    void Renderer::Initialize()
    {
        Event::EventSystem::Get()->Connect<Event::ELevelUninstalled, &Renderer::OnLevelUninstalled>(this);

        m_backend           = Backend::Get();
        const Vector2i size = Window::Get()->GetSize();

        m_graphicsQueue.Get(m_backend->GetQueueFamilyIndex(QueueFamilies::Graphics));

        Extent3D ext = Extent3D{.width  = static_cast<unsigned int>(size.x),
                                .height = static_cast<unsigned int>(size.y),
                                .depth  = 1};

        m_depthImage = Image{
            .format          = Format::D32_SFLOAT,
            .tiling          = ImageTiling::Optimal,
            .extent          = ext,
            .aspectFlags     = GetImageAspectFlags(ImageAspectFlags::AspectDepth),
            .imageUsageFlags = GetImageUsage(ImageUsageFlags::DepthStencilAttachment)};
        m_depthImage.Create();

        Attachment att          = Attachment{.format = m_backend->m_swapchain.format, .loadOp = LoadOp::Clear};
        Attachment depthStencil = Attachment{
            .format         = Format::D32_SFLOAT,
            .loadOp         = LoadOp::Clear,
            .storeOp        = StoreOp::Store,
            .stencilLoadOp  = LoadOp::Clear,
            .stencilStoreOp = StoreOp::DontCare,
            .initialLayout  = ImageLayout::Undefined,
            .finalLayout    = ImageLayout::DepthStencilOptimal};

        SubPass sp = SubPass{.bindPoint = PipelineBindPoint::Graphics};
        sp.AddColorAttachmentRef(0, ImageLayout::ColorOptimal)
            .SetDepthStencilAttachmentRef(1, ImageLayout::DepthStencilOptimal)
            .Create();

        SubPassDependency dep = SubPassDependency{
            .dstSubpass    = 0,
            .srcStageMask  = GetPipelineStageFlags(PipelineStageFlags::ColorAttachmentOutput),
            .srcAccessMask = 0,
            .dstStageMask  = GetPipelineStageFlags(PipelineStageFlags::ColorAttachmentOutput),
            .dstAccessMask = GetAccessFlags(AccessFlags::ColorAttachmentWrite),
        };

        const uint32 depthFlags = GetPipelineStageFlags(PipelineStageFlags::EarlyFragmentTests) | GetPipelineStageFlags(PipelineStageFlags::LateFragmentTests);

        SubPassDependency depDepth = SubPassDependency{
            .dstSubpass    = 0,
            .srcStageMask  = depthFlags,
            .srcAccessMask = 0,
            .dstStageMask  = depthFlags,
            .dstAccessMask = GetAccessFlags(AccessFlags::DepthStencilAttachmentWrite),
        };

        ClearValue clrCol = ClearValue{
            .clearColor = Color(0.1f, 0.1f, 0.1f, 1.0f),
            .isColor    = true,
        };

        ClearValue clrDepth = ClearValue{
            .isColor = false,
            .depth   = 1.0f,
        };

        m_renderPass = RenderPass{};
        m_renderPass.AddAttachment(att)
            .AddAttachment(depthStencil)
            .AddSubpass(sp)
            .AddSubPassDependency(dep)
            .AddSubPassDependency(depDepth)
            .AddClearValue(clrCol)
            .AddClearValue(clrDepth)
            .Create();
        m_framebuffers.reserve(m_backend->m_swapchain._imageViews.size());

        for (auto iv : m_backend->m_swapchain._imageViews)
        {
            Framebuffer fb = Framebuffer{
                .width  = static_cast<uint32>(size.x),
                .height = static_cast<uint32>(size.y),
                .layers = 1,
            };

            fb.AttachRenderPass(m_renderPass).AddImageView(iv).AddImageView(m_depthImage._ptrImgView).Create();
            m_framebuffers.push_back(fb);
        }

        m_descriptorPool = DescriptorPool{
            .maxSets = 10,
            .flags   = DescriptorPoolCreateFlags::None,
        };
        m_descriptorPool.AddPoolSize(DescriptorType::UniformBuffer, 10).Create();

        DescriptorSetLayoutBinding binding = DescriptorSetLayoutBinding{
            .binding         = 0,
            .descriptorCount = 1,
            .stage           = ShaderStage::Vertex,
            .type            = DescriptorType::UniformBuffer,
        };

        m_globalSetLayout.AddBinding(binding).Create();

        for (int i = 0; i < FRAME_OVERLAP; i++)
        {
            Frame& f = m_frames[i];

            // Commands
            f.pool = CommandPool{.familyIndex = m_graphicsQueue._family, .flags = GetCommandPoolCreateFlags(CommandPoolFlags::Reset)};
            f.pool.Create();
            f.commandBuffer = CommandBuffer{.count = 1, .level = CommandBufferLevel::Primary};
            f.commandBuffer.Create(f.pool._ptr);

            // Sync
            f.presentSemaphore.Create();
            f.renderSemaphore.Create();
            f.renderFence = Fence{.flags = GetFenceFlags(FenceFlags::Signaled)};
            f.renderFence.Create();

            // Descriptor
            f.cameraBuffer = Buffer{
                .size        = sizeof(GPUCameraData),
                .bufferUsage = BufferUsageFlags::UniformBuffer,
                .memoryUsage = MemoryUsageFlags::CpuToGpu,
            };
            f.cameraBuffer.Create();

            f.globalDescriptor = DescriptorSet{
                .setCount = 1,
                .pool     = m_descriptorPool._ptr,
            };

            f.globalDescriptor.AddSetLayout(m_globalSetLayout._ptr);
            f.globalDescriptor.Create();

            f.globalDescriptor.UpdateSingle(0, DescriptorType::UniformBuffer, &f.cameraBuffer, 0, sizeof(GPUCameraData));
        }

        // Views
        m_playerView.m_viewType = ViewType::Player;
        m_views.push_back(&m_playerView);

        // Feature renderers.
        m_featureRendererManager.m_renderer = this;
        m_meshRenderer.Initialize(m_featureRendererManager);
    }

    void Renderer::Tick()
    {
        PROFILER_FUNC(PROFILER_THREAD_MAIN);
        m_cameraSystem.Tick();
        m_playerView.CalculateFrustum(m_cameraSystem.GetPos(), m_cameraSystem.GetView(), m_cameraSystem.GetProj());

        GPUCameraData camData;
        camData.proj     = m_cameraSystem.GetProj();
        camData.view     = m_cameraSystem.GetView();
        camData.viewProj = camData.proj * camData.view;

        GetCurrentFrame().cameraBuffer.CopyInto(&camData, sizeof(GPUCameraData));
    }

    void Renderer::FetchAndExtract(World::EntityWorld* world)
    {
        PROFILER_FUNC(PROFILER_THREAD_MAIN);
        if (world == nullptr)
            return;

        // Determine views.

        // Determine views.

        // Calculate visible renderables for each view.
        Taskflow tf;
        tf.for_each(m_views.begin(), m_views.end(), [&](View* v) {
            v->CalculateVisibility(m_renderables.data(), m_nextRenderableID);
        });
        JobSystem::Get()->Run(tf).wait();

        // Start extracting game state.
        m_featureRendererManager.ExtractGameState(world);
    }

    void Renderer::OnLevelUninstalled(const Event::ELevelUninstalled& ev)
    {
        m_renderables.clear();
    }

    Frame& Renderer::GetCurrentFrame()
    {
        return m_frames[m_frameNumber % FRAME_OVERLAP];
    }

    void Renderer::Render()
    {
        PROFILER_FUNC(PROFILER_THREAD_RENDER);

        m_featureRendererManager.PrepareRenderData();

        GetCurrentFrame().renderFence.Wait(true, 1.0);
        GetCurrentFrame().renderFence.Reset();

        uint32 imageIndex = m_backend->m_swapchain.AcquireNextImage(1.0, GetCurrentFrame().presentSemaphore);

        GetCurrentFrame().commandBuffer.Reset();
        GetCurrentFrame().commandBuffer.Begin(GetCommandBufferFlags(CommandBufferFlags::OneTimeSubmit));

        m_renderPass.Begin(m_framebuffers[imageIndex], GetCurrentFrame().commandBuffer);

        // Render commands.
        m_featureRendererManager.Submit(GetCurrentFrame().commandBuffer);

        m_renderPass.End(GetCurrentFrame().commandBuffer);
        GetCurrentFrame().commandBuffer.End();

        PROFILER_SCOPE_START("Queue Submit & Present", PROFILER_THREAD_RENDER);
        // Submit command waits on the present semaphore, e.g. it waits for the acquired image to be ready.
        // Then submits command, and signals render semaphore when its submitted.
        m_graphicsQueue.Submit(GetCurrentFrame().presentSemaphore, GetCurrentFrame().renderSemaphore, GetCurrentFrame().renderFence, GetCurrentFrame().commandBuffer, 1);
        m_graphicsQueue.Present(GetCurrentFrame().renderSemaphore, &imageIndex);
        PROFILER_SCOPE_END("Queue Submit & Present", PROFILER_THREAD_RENDER);

        m_frameNumber++;
    }

    void Renderer::Join()
    {
        for (int i = 0; i < FRAME_OVERLAP; i++)
            m_frames[i].renderFence.Wait();
    }

    void Renderer::Shutdown()
    {
        Event::EventSystem::Get()->Disconnect<Event::ELevelInstalled>(this);
        Event::EventSystem::Get()->Disconnect<Event::ELevelUninstalled>(this);

        m_meshRenderer.Shutdown();

        for (int i = 0; i < FRAME_OVERLAP; i++)
        {
            m_frames[i].cameraBuffer.Destroy();
        }

        for (int i = 0; i < m_framebuffers.size(); i++)
            m_framebuffers[i].Destroy();
    }

    void Renderer::AddRenderable(RenderableComponent* comp)
    {
        uint32 id = 0;

        if (!m_availableRenderableIDs.empty())
        {
            id = m_availableRenderableIDs.front();
            m_availableRenderableIDs.pop();
        }
        else
        {
            id = m_nextRenderableID++;

            const uint32 size = static_cast<uint32>(m_renderables.size());
            if (m_nextRenderableID > size)
                m_renderables.resize(size + RENDERABLES_STACK_SIZE);
        }

        comp->m_renderableID = id;
        m_renderables[id]    = comp;
    }

    void Renderer::RemoveRenderable(RenderableComponent* comp)
    {
        m_renderables[comp->m_renderableID] = nullptr;
        m_availableRenderableIDs.push(comp->m_renderableID);
    }

} // namespace Lina::Graphics
