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

#include "ECS/Registry.hpp"
#include "Core/Renderer.hpp"
#include "Core/Window.hpp"
#include "Core/Backend.hpp"

namespace Lina::Graphics
{
    Renderer* Renderer::s_instance = nullptr;

    void Renderer::Initialize()
    {
        m_backend           = Backend::Get();
        const Vector2i size = Window::Get()->GetSize();

        m_graphicsQueue.Get(m_backend->GetQueueFamilyIndex(QueueFamilies::Graphics));
        m_pool = CommandPool{.familyIndex = m_graphicsQueue._family, .flags = GetCommandPoolCreateFlags(CommandPoolFlags::Reset)};
        m_pool.Create();

        m_commandBuffer = CommandBuffer{.count = 1, .level = CommandBufferLevel::Primary};
        m_commandBuffer.Create(m_pool._ptr);

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
            .clearColor = Color::Blue,
        };

        ClearValue clrDepth = ClearValue{
            .depth = 1.0f,
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

        m_presentSemaphore.Create();
        m_renderSemaphore.Create();
        m_renderFence = Fence{.flags = GetFenceFlags(FenceFlags::Signaled)};
        m_renderFence.Create();
    }


    void Renderer::FetchAndExtract()
    {
        PROFILER_FUNC(PROFILER_THREAD_SIMULATION);
        m_featureRendererManager.FetchVisibility();
        m_featureRendererManager.ExtractGameState();
    }

    void Renderer::Render()
    {
        PROFILER_FUNC(PROFILER_THREAD_RENDER);
        m_featureRendererManager.PrepareRenderData();

        m_renderFence.Wait(true, 1.0);
        m_renderFence.Reset();

        uint32 imageIndex = m_backend->m_swapchain.AcquireNextImage(1.0, m_presentSemaphore);

        m_commandBuffer.Reset();
        m_commandBuffer.Begin(GetCommandBufferFlags(CommandBufferFlags::OneTimeSubmit));

        m_renderPass.Begin(m_framebuffers[imageIndex], m_commandBuffer);

        // Render commands.
        m_featureRendererManager.Submit(m_commandBuffer);

        m_renderPass.End(m_commandBuffer);
        m_commandBuffer.End();

        // Submit command waits on the present semaphore, e.g. it waits for the acquired image to be ready.
        // Then submits command, and signals render semaphore when its submitted.
        m_graphicsQueue.Submit(m_presentSemaphore, m_renderSemaphore, m_renderFence, m_commandBuffer, 1);
        m_graphicsQueue.Present(m_renderSemaphore, &imageIndex);
    }

    void Renderer::Join()
    {
        m_renderFence.Wait();
    }

    void Renderer::Shutdown()
    {
        for (int i = 0; i < m_framebuffers.size(); i++)
            m_framebuffers[i].Destroy();
    }



} // namespace Lina::Graphics
