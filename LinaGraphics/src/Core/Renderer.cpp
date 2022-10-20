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
#include "Core/RenderEngine.hpp"
#include "Core/Window.hpp"
#include "Core/Backend.hpp"
#include "Core/GUIBackend.hpp"
#include "PipelineObjects/RQueue.hpp"
#include "EventSystem/EventSystem.hpp"
#include "EventSystem/EntityEvents.hpp"
#include "EventSystem/ResourceEvents.hpp"
#include "Resource/Texture.hpp"
#include "Core/LevelManager.hpp"
#include "Core/Level.hpp"
#include "Resource/ModelNode.hpp"
#include "Resource/Mesh.hpp"
#include "Core/ResourceManager.hpp"
#include "Core/Component.hpp"
#include "Components/RenderableComponent.hpp"
#include "Resource/Material.hpp"
#include "Resource/Model.hpp"
#include "Resource/Mesh.hpp"
#include "Utility/Vulkan/VulkanUtility.hpp"
#include "Utility/Vulkan/vk_mem_alloc.h"
#include <vulkan/vulkan.h>
#include <LinaVG/LinaVG.hpp>

namespace Lina::Graphics
{

    void Renderer::Initialize()
    {
        Event::EventSystem::Get()->Connect<Event::ELevelUninstalled, &Renderer::OnLevelUninstalled>(this);
        Event::EventSystem::Get()->Connect<Event::EComponentCreated, &Renderer::OnComponentCreated>(this);
        Event::EventSystem::Get()->Connect<Event::EComponentDestroyed, &Renderer::OnComponentDestroyed>(this);
        Event::EventSystem::Get()->Connect<Event::EPreMainLoop, &Renderer::OnPreMainLoop>(this);

        m_backend = Backend::Get();

        const Vector2i size = Backend::Get()->GetSwapchain().size;
        Extent3D       ext  = Extent3D{.width = static_cast<unsigned int>(size.x), .height = static_cast<unsigned int>(size.y), .depth = 1};

        ImageSubresourceRange depthRange;
        depthRange.aspectFlags = GetImageAspectFlags(ImageAspectFlags::AspectDepth);

        m_depthImage = Image{
            .format              = Format::D32_SFLOAT,
            .tiling              = ImageTiling::Optimal,
            .extent              = ext,
            .imageUsageFlags     = GetImageUsage(ImageUsageFlags::DepthStencilAttachment),
            .memoryUsageFlags    = MemoryUsageFlags::GpuOnly,
            .memoryPropertyFlags = MemoryPropertyFlags::DeviceLocal,
            .subresRange         = depthRange,
        };
        m_depthImage.Create(true, false);

        m_renderPasses[RenderPassType::Main]        = RenderPass();
        m_renderPasses[RenderPassType::PostProcess] = RenderPass();
        m_renderPasses[RenderPassType::Final]       = RenderPass();
        auto& mainPass                              = m_renderPasses[RenderPassType::Main];
        auto& postPass                              = m_renderPasses[RenderPassType::PostProcess];
        auto& finalPass                             = m_renderPasses[RenderPassType::Final];
        VulkanUtility::SetupAndCreateMainRenderPass(mainPass);
        VulkanUtility::SetupAndCreateMainRenderPass(postPass);
        VulkanUtility::SetupAndCreateFinalRenderPass(finalPass);

        // Final pass uses swapchain images.
        for (auto iv : m_backend->m_swapchain._imageViews)
        {
            Framebuffer fb = Framebuffer{
                .width  = static_cast<uint32>(size.x),
                .height = static_cast<uint32>(size.y),
                .layers = 1,
            };

            fb.AttachRenderPass(finalPass).AddImageView(iv).AddImageView(m_depthImage._ptrImgView).Create();
            m_framebuffers.push_back(fb);
        }

        for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            Frame& f = m_frames[i];

            // Commands
            f.pool = CommandPool{.familyIndex = Backend::Get()->GetGraphicsQueue()._family, .flags = GetCommandPoolCreateFlags(CommandPoolFlags::Reset)};
            f.pool.Create();
            f.commandBuffer = CommandBuffer{.count = 1, .level = CommandBufferLevel::Primary};
            f.commandBuffer.Create(f.pool._ptr);

            f.finalPassCmd = CommandBuffer{.count = 1, .level = CommandBufferLevel::Primary};
            f.finalPassCmd.Create(f.pool._ptr);

            // Sync
            f.presentSemaphore.Create();
            f.renderSemaphore.Create();
            f.renderFence = Fence{.flags = GetFenceFlags(FenceFlags::Signaled)};
            f.renderFence.Create();

            f.indirectBuffer =
                Buffer{.size        = OBJ_BUFFER_MAX * sizeof(VkDrawIndexedIndirectCommand),
                       .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::TransferDst) | GetBufferUsageFlags(BufferUsageFlags::StorageBuffer) | GetBufferUsageFlags(BufferUsageFlags::IndirectBuffer),
                       .memoryUsage = MemoryUsageFlags::CpuToGpu};

            f.indirectBuffer.Create();

            // Scene data - set 0 b 0
            f.sceneDataBuffer = Buffer{
                .size        = sizeof(GPUSceneData),
                .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::UniformBuffer),
                .memoryUsage = MemoryUsageFlags::CpuToGpu,
            };
            f.sceneDataBuffer.Create();

            // Light data - set 0 b 1
            f.lightDataBuffer = Buffer{
                .size        = sizeof(GPULightData),
                .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::UniformBuffer),
                .memoryUsage = MemoryUsageFlags::CpuToGpu,
            };
            f.lightDataBuffer.Create();

            f.globalDescriptor = DescriptorSet{
                .setCount = 1,
                .pool     = RenderEngine::Get()->GetDescriptorPool()._ptr,
            };

            f.globalDescriptor.AddLayout(&RenderEngine::Get()->GetLayout(DescriptorSetType::GlobalSet));
            f.globalDescriptor.Create();
            f.globalDescriptor.BeginUpdate();
            f.globalDescriptor.AddBufferUpdate(f.sceneDataBuffer, f.sceneDataBuffer.size, 0, DescriptorType::UniformBuffer);
            f.globalDescriptor.AddBufferUpdate(f.lightDataBuffer, f.lightDataBuffer.size, 1, DescriptorType::UniformBuffer);
            f.globalDescriptor.SendUpdate();

            // Pass descriptor

            f.viewDataBuffer = Buffer{
                .size        = sizeof(GPUViewData),
                .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::UniformBuffer),
                .memoryUsage = MemoryUsageFlags::CpuToGpu,
            };
            f.viewDataBuffer.Create();

            f.objDataBuffer = Buffer{
                .size        = sizeof(GPUObjectData) * OBJ_BUFFER_MAX,
                .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::StorageBuffer),
                .memoryUsage = MemoryUsageFlags::CpuToGpu,
            };
            f.objDataBuffer.Create();
            f.objDataBuffer.boundSet     = &f.passDescriptor;
            f.objDataBuffer.boundBinding = 1;
            f.objDataBuffer.boundType    = DescriptorType::StorageBuffer;

            f.passDescriptor = DescriptorSet{
                .setCount = 1,
                .pool     = RenderEngine::Get()->GetDescriptorPool()._ptr,
            };

            f.passDescriptor.AddLayout(&RenderEngine::Get()->GetLayout(DescriptorSetType::PassSet));
            f.passDescriptor.Create();
            f.passDescriptor.BeginUpdate();
            f.passDescriptor.AddBufferUpdate(f.viewDataBuffer, f.viewDataBuffer.size, 0, DescriptorType::UniformBuffer);
            f.passDescriptor.AddBufferUpdate(f.objDataBuffer, f.objDataBuffer.size, 1, DescriptorType::StorageBuffer);
            f.passDescriptor.SendUpdate();
        }

        // Views
        m_playerView.Initialize(ViewType::Player);
        m_views.push_back(&m_playerView);

        // Draw passes.
        m_opaquePass.Initialize(DrawPassMask::Opaque, &m_playerView, 1000.0f);

        // IDlists
        m_allRenderables.Initialize(250, nullptr);
    }

    void Renderer::Shutdown()
    {
        Event::EventSystem::Get()->Disconnect<Event::ELevelUninstalled>(this);
        Event::EventSystem::Get()->Disconnect<Event::EComponentCreated>(this);
        Event::EventSystem::Get()->Disconnect<Event::EComponentDestroyed>(this);
        Event::EventSystem::Get()->Disconnect<Event::EPreMainLoop>(this);

        for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            m_frames[i].objDataBuffer.Destroy();
            m_frames[i].sceneDataBuffer.Destroy();
            m_frames[i].viewDataBuffer.Destroy();
            m_frames[i].lightDataBuffer.Destroy();
            m_frames[i].indirectBuffer.Destroy();
        }

        m_renderPasses[RenderPassType::Main].Destroy();
        m_renderPasses[RenderPassType::PostProcess].Destroy();
        m_renderPasses[RenderPassType::Final].Destroy();

        m_depthImage.Destroy();

        for (int i = 0; i < m_framebuffers.size(); i++)
            m_framebuffers[i].Destroy();
    }

    void Renderer::Tick()
    {
        PROFILER_FUNC(PROFILER_THREAD_MAIN);
        m_cameraSystem.Tick();
        m_playerView.Tick(m_cameraSystem.GetPos(), m_cameraSystem.GetView(), m_cameraSystem.GetProj());
    }

    void Renderer::Join()
    {
        for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
            m_frames[i].renderFence.Wait();
    }

    void Renderer::SyncData()
    {
        PROFILER_FUNC(PROFILER_THREAD_MAIN);

        m_extractedRenderables.clear();
        const auto& renderables = m_allRenderables.GetItems();

        uint32 i = 0;
        for (auto r : renderables)
        {
            if (r == nullptr)
                continue;

            if (!r->GetEntity()->GetEntityMask().IsSet(World::EntityMask::Visible))
                continue;

            RenderableData data;
            data.entityID          = r->GetEntity()->GetID();
            data.modelMatrix       = r->GetEntity()->ToMatrix();
            data.entityMask        = r->GetEntity()->GetEntityMask();
            data.position          = r->GetEntity()->GetPosition();
            data.aabb              = r->GetAABB();
            data.passMask          = r->GetDrawPasses();
            data.type              = r->GetType();
            data.meshMaterialPairs = r->GetMeshMaterialPairs();
            data.objDataIndex      = i++;
            m_extractedRenderables.push_back(data);
        }

        m_opaquePass.PrepareRenderData(m_extractedRenderables);
    }

    void Renderer::Render()
    {
        PROFILER_FUNC(PROFILER_THREAD_RENDER);

        GetCurrentFrame().renderFence.Wait(true, 1.0f);

        VulkanResult res;
        uint32       imageIndex = m_backend->m_swapchain.AcquireNextImage(1.0, GetCurrentFrame().presentSemaphore, res);

        if (m_recreateSwapchain || res == VulkanResult::OutOfDateKHR || res == VulkanResult::SuboptimalKHR)
        {
            HandleOutOfDateImage();
            return;
        }
        else if (res != VulkanResult::Success)
            LINA_ASSERT(false, "Could not acquire next image!");

        GetCurrentFrame().renderFence.Reset();

        auto& cmd = GetCurrentFrame().commandBuffer;

        cmd.Reset();
        cmd.Begin(GetCommandBufferFlags(CommandBufferFlags::OneTimeSubmit));

        VkViewport _viewport = VulkanUtility::GetViewport(RenderEngine::Get()->GetViewport());
        VkRect2D   _scissors = VulkanUtility::GetRect(RenderEngine::Get()->GetScissors());

        vkCmdSetViewport(cmd._ptr, 0, 1, &_viewport);
        vkCmdSetScissor(cmd._ptr, 0, 1, &_scissors);

        // Global set.
        cmd.CMD_BindDescriptorSets(PipelineBindPoint::Graphics, RenderEngine::Get()->GetGlobalAndPassLayouts()._ptr, 0, 1, &GetCurrentFrame().globalDescriptor, 0, nullptr);

        // Global - scene data.
        GetSceneDataBuffer().CopyInto(&m_sceneData, sizeof(GPUSceneData));

        // Global - light data.
        GetLightDataBuffer().CopyInto(&m_lightData, sizeof(GPULightData));

        // Pass set.
        cmd.CMD_BindDescriptorSets(PipelineBindPoint::Graphics, RenderEngine::Get()->GetGlobalAndPassLayouts()._ptr, 1, 1, &GetCurrentFrame().passDescriptor, 0, nullptr);

        // Per render pass - obj data.
        Vector<GPUObjectData> gpuObjectData;

        for (auto& r : m_extractedRenderables)
        {
            // Object data.
            GPUObjectData objData;
            objData.modelMatrix = r.modelMatrix;
            gpuObjectData.push_back(objData);
        }

        GetObjectDataBuffer().CopyInto(gpuObjectData.data(), sizeof(GPUObjectData) * gpuObjectData.size());

        auto& mainPass  = m_renderPasses[RenderPassType::Main];
        auto& postPass  = m_renderPasses[RenderPassType::PostProcess];
        auto& finalPass = m_renderPasses[RenderPassType::Final];

        mainPass.Begin(*mainPass._framebuffer, cmd);
        m_opaquePass.UpdateViewData(GetViewDataBuffer());
        m_opaquePass.RecordDrawCommands(cmd, RenderPassType::Main);
        mainPass.End(cmd);

        postPass.Begin(*postPass._framebuffer, cmd);
        auto* ppMat = RenderEngine::Get()->GetEngineMaterial(EngineShaderType::SQPostProcess);
        ppMat->BindPipelineAndDescriptors(cmd, RenderPassType::PostProcess);
        cmd.CMD_Draw(3, 1, 0, 0);
        postPass.End(cmd);

        const bool drawEditor = ApplicationInfo::GetAppMode() == ApplicationMode::Editor;

        if (drawEditor)
            Event::EventSystem::Get()->Trigger<Event::EOnEditorDrawBegin>(Event::EOnEditorDrawBegin{.cmd = &cmd});

        finalPass.Begin(m_framebuffers[imageIndex], cmd);

        auto* finalQuadMat = RenderEngine::Get()->GetEngineMaterial(EngineShaderType::SQFinal);
        finalQuadMat->BindPipelineAndDescriptors(cmd, RenderPassType::Final);
        cmd.CMD_Draw(3, 1, 0, 0);

        if (drawEditor)
            Event::EventSystem::Get()->Trigger<Event::EOnEditorDraw>(Event::EOnEditorDraw{.cmd = &cmd});

        finalPass.End(cmd);

        if (drawEditor)
            Event::EventSystem::Get()->Trigger<Event::EOnEditorDrawEnd>(Event::EOnEditorDrawEnd{.cmd = &cmd});

        cmd.End();

        // Submit command waits on the present semaphore, e.g. it waits for the acquired image to be ready.
        // Then submits command, and signals render semaphore when its submitted.
        PROFILER_SCOPE_START("Queue Submit & Present", PROFILER_THREAD_RENDER);
        Backend::Get()->GetGraphicsQueue().Submit(GetCurrentFrame().presentSemaphore, GetCurrentFrame().renderSemaphore, GetCurrentFrame().renderFence, GetCurrentFrame().commandBuffer, 1);
        Backend::Get()->GetGraphicsQueue().Present(GetCurrentFrame().renderSemaphore, &imageIndex);
        PROFILER_SCOPE_END("Queue Submit & Present", PROFILER_THREAD_RENDER);

        m_frameNumber++;
    }

    void Renderer::OnLevelUninstalled(const Event::ELevelUninstalled& ev)
    {
        m_allRenderables.Reset();
    }

    void Renderer::OnComponentCreated(const Event::EComponentCreated& ev)
    {
        if (ev.ptr->GetComponentMask().IsSet(World::ComponentMask::Renderable))
        {
            auto renderable            = static_cast<RenderableComponent*>(ev.ptr);
            renderable->m_renderableID = m_allRenderables.AddItem(renderable);
        }
    }

    void Renderer::OnComponentDestroyed(const Event::EComponentDestroyed& ev)
    {
        if (ev.ptr->GetComponentMask().IsSet(World::ComponentMask::Renderable))
            m_allRenderables.RemoveItem(static_cast<RenderableComponent*>(ev.ptr)->GetRenderableID());
    }

    void Renderer::OnPreMainLoop(const Event::EPreMainLoop& ev)
    {
        auto* finalQuadMat = RenderEngine::Get()->GetEngineMaterial(EngineShaderType::SQFinal);
        auto* ppMat        = RenderEngine::Get()->GetEngineMaterial(EngineShaderType::SQPostProcess);
        auto& mainPass     = m_renderPasses[RenderPassType::Main];
        auto& ppPass       = m_renderPasses[RenderPassType::PostProcess];
        finalQuadMat->SetTexture(0, ppPass._colorTexture);
        ppMat->SetTexture(0, mainPass._colorTexture);
    }

    void Renderer::OnWindowResized(const Vector2i& newSize)
    {
        m_recreateSwapchain = true;
    }

    void Renderer::OnWindowPositioned(const Vector2i& newPos)
    {
        m_recreateSwapchain = true;
    }

    void Renderer::HandleOutOfDateImage()
    {
        Backend::Get()->WaitIdle();
        Vector2i size = Window::Get()->GetSize();

        // Framebuffers
        for (auto& fb : m_framebuffers)
            fb.Destroy();
        m_framebuffers.clear();

        // Swapchain
        auto& swapchain = Backend::Get()->GetSwapchain();
        swapchain.Destroy();
        swapchain.size = size;
        swapchain.Create();

        // Make sure we always match swapchain
        size = swapchain.size;

        // Frame buffer depth
        m_depthImage.Destroy();
        m_depthImage.extent.width  = size.x;
        m_depthImage.extent.height = size.y;
        m_depthImage.Create(true, false);

        // Only final render pass depends on swapchain, destroy it.
        auto& mainPass  = m_renderPasses[RenderPassType::Main];
        auto& ppPass    = m_renderPasses[RenderPassType::PostProcess];
        auto& finalPass = m_renderPasses[RenderPassType::Final];
        mainPass.Destroy();
        ppPass.Destroy();
        finalPass.Destroy();
        mainPass  = RenderPass();
        ppPass    = RenderPass();
        finalPass = RenderPass();

        VulkanUtility::SetupAndCreateMainRenderPass(mainPass);
        VulkanUtility::SetupAndCreateMainRenderPass(ppPass);
        VulkanUtility::SetupAndCreateFinalRenderPass(finalPass);

        // Re-assign target textures to render passes
        OnPreMainLoop({});

        // framebuffers
        for (auto iv : m_backend->m_swapchain._imageViews)
        {
            Framebuffer fb = Framebuffer{
                .width  = static_cast<uint32>(size.x),
                .height = static_cast<uint32>(size.y),
                .layers = 1,
            };

            fb.AttachRenderPass(finalPass).AddImageView(iv).AddImageView(m_depthImage._ptrImgView).Create();
            m_framebuffers.push_back(fb);
        }

        m_recreateSwapchain = false;
    }

} // namespace Lina::Graphics
