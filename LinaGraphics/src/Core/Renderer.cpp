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
#include "PipelineObjects/RQueue.hpp"
#include "EventSystem/EventSystem.hpp"
#include "EventSystem/EntityEvents.hpp"
#include "EventSystem/LevelEvents.hpp"
#include "Resource/Texture.hpp"
#include "Core/LevelManager.hpp"
#include "Core/Level.hpp"
#include "Resource/ModelNode.hpp"
#include "Resource/Mesh.hpp"
#include "Core/ResourceManager.hpp"
#include "Core/Component.hpp"
#include "Components/RenderableComponent.hpp"
#include "Resource/Material.hpp"
#include "Utility/Vulkan/VulkanUtility.hpp"
#include "Utility/Vulkan/vk_mem_alloc.h"
#include <vulkan/vulkan.h>

namespace Lina::Graphics
{

#define OBJ_BUFFER_MAX 15

    void Renderer::Initialize()
    {
        Event::EventSystem::Get()->Connect<Event::ELevelUninstalled, &Renderer::OnLevelUninstalled>(this);
        Event::EventSystem::Get()->Connect<Event::ELevelInstalled, &Renderer::OnLevelInstalled>(this);
        Event::EventSystem::Get()->Connect<Event::EComponentCreated, &Renderer::OnComponentCreated>(this);
        Event::EventSystem::Get()->Connect<Event::EComponentDestroyed, &Renderer::OnComponentDestroyed>(this);

        m_backend           = Backend::Get();
        const Vector2i size = Window::Get()->GetSize();

        Extent3D ext = Extent3D{.width  = static_cast<unsigned int>(size.x),
                                .height = static_cast<unsigned int>(size.y),
                                .depth  = 1};

        m_depthImage = Image{
            .format              = Format::D32_SFLOAT,
            .tiling              = ImageTiling::Optimal,
            .extent              = ext,
            .aspectFlags         = GetImageAspectFlags(ImageAspectFlags::AspectDepth),
            .imageUsageFlags     = GetImageUsage(ImageUsageFlags::DepthStencilAttachment),
            .memoryUsageFlags    = MemoryUsageFlags::GpuOnly,
            .memoryPropertyFlags = MemoryPropertyFlags::DeviceLocal,
        };
        m_depthImage.Create(true);

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

        m_textureDescriptor = DescriptorSet{
            .setCount = 1,
            .pool     = RenderEngine::Get()->GetDescriptorPool()._ptr,
        };

        m_textureDescriptor.AddLayout(RenderEngine::Get()->GetLayout(SetLayouts::TextureLayout));
        m_textureDescriptor.Create();

        for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            Frame& f = m_frames[i];

            // Commands
            f.pool = CommandPool{.familyIndex = Backend::Get()->GetGraphicsQueue()._family, .flags = GetCommandPoolCreateFlags(CommandPoolFlags::Reset)};
            f.pool.Create();
            f.commandBuffer = CommandBuffer{.count = 1, .level = CommandBufferLevel::Primary};
            f.commandBuffer.Create(f.pool._ptr);

            // Sync
            f.presentSemaphore.Create();
            f.renderSemaphore.Create();
            f.renderFence = Fence{.flags = GetFenceFlags(FenceFlags::Signaled)};
            f.renderFence.Create();

            f.objDataBuffer = Buffer{
                .size        = sizeof(GPUObjectData) * OBJ_BUFFER_MAX,
                .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::StorageBuffer),
                .memoryUsage = MemoryUsageFlags::CpuToGpu,
            };
            f.objDataBuffer.Create();
            f.objDataBuffer.boundSet = &f.objDataDescriptor;

            // f.testBuffer = Buffer{
            //     .size        = sizeof(TestData) * OBJ_BUFFER_MAX,
            //     .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::StorageBuffer),
            //     .memoryUsage = MemoryUsageFlags::CpuToGpu,
            // };
            // f.testBuffer.Create();
            // f.testBuffer.boundSet = &f.objDataDescriptor;

            f.objDataDescriptor = DescriptorSet{
                .setCount = 1,
                .pool     = RenderEngine::Get()->GetDescriptorPool()._ptr,
            };

            f.objDataDescriptor.AddLayout(RenderEngine::Get()->GetLayout(SetLayouts::ObjectDataLayout));
            f.objDataDescriptor.AddBuffer(f.objDataBuffer);
            f.objDataDescriptor.Create();
            f.objDataDescriptor.Update();

            f.indirectBuffer = Buffer{
                .size        = OBJ_BUFFER_MAX * sizeof(VkDrawIndexedIndirectCommand),
                .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::TransferDst) | GetBufferUsageFlags(BufferUsageFlags::StorageBuffer) | GetBufferUsageFlags(BufferUsageFlags::IndirectBuffer),
                .memoryUsage = MemoryUsageFlags::CpuToGpu};

            f.indirectBuffer.Create();

            f.scenePropertiesBuffer = Buffer{
                .size        = sizeof(GPUSceneData),
                .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::UniformBuffer),
                .memoryUsage = MemoryUsageFlags::CpuToGpu,
            };

            f.scenePropertiesBuffer.Create();

            f.globalDescriptor = DescriptorSet{
                .setCount = 1,
                .pool     = RenderEngine::Get()->GetDescriptorPool()._ptr,
            };

            f.globalDescriptor.AddLayout(RenderEngine::Get()->GetLayout(SetLayouts::GlobalLayout));
            f.globalDescriptor.AddBuffer(f.scenePropertiesBuffer);
            f.globalDescriptor.Create();
            f.globalDescriptor.Update();
        }

        // Views
        m_playerView.Initialize(ViewType::Player);
        m_views.push_back(&m_playerView);

        // Draw passes.
        m_opaquePass.Initialize(DrawPassMask::Opaque, &m_playerView, 1000.0f);

        // Feature renderers.
        m_featureRendererManager.Initialize();

        // IDlists
        m_allRenderables.Initialize(250, nullptr);
    }

    void Renderer::Shutdown()
    {
        Event::EventSystem::Get()->Disconnect<Event::ELevelInstalled>(this);
        Event::EventSystem::Get()->Disconnect<Event::ELevelUninstalled>(this);
        Event::EventSystem::Get()->Disconnect<Event::EComponentCreated>(this);
        Event::EventSystem::Get()->Disconnect<Event::EComponentDestroyed>(this);
        m_featureRendererManager.Shutdown();

        for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            m_frames[i].objDataBuffer.Destroy();
            m_frames[i].scenePropertiesBuffer.Destroy();
        }

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
        GetCurrentFrame().renderFence.Reset();

        // Obj data buffer.
        Vector<GPUObjectData> gpuObjectData;

        for (auto& r : m_extractedRenderables)
        {
            // Object data.
            GPUObjectData objData;
            objData.modelMatrix = r.modelMatrix;
            gpuObjectData.push_back(objData);
        }

        GetObjectDataBuffer().CopyInto(gpuObjectData.data(), sizeof(GPUObjectData) * gpuObjectData.size());

        uint32 imageIndex = m_backend->m_swapchain.AcquireNextImage(1.0, GetCurrentFrame().presentSemaphore);

        GetCurrentFrame().commandBuffer.Reset();
        GetCurrentFrame().commandBuffer.Begin(GetCommandBufferFlags(CommandBufferFlags::OneTimeSubmit));

        m_renderPass.Begin(m_framebuffers[imageIndex], GetCurrentFrame().commandBuffer);

        m_opaquePass.RecordDrawCommands(GetCurrentFrame().commandBuffer);

        m_renderPass.End(GetCurrentFrame().commandBuffer);
        GetCurrentFrame().commandBuffer.End();

        // Submit command waits on the present semaphore, e.g. it waits for the acquired image to be ready.
        // Then submits command, and signals render semaphore when its submitted.
        PROFILER_SCOPE_START("Queue Submit & Present", PROFILER_THREAD_RENDER);
        Backend::Get()->GetGraphicsQueue().Submit(GetCurrentFrame().presentSemaphore, GetCurrentFrame().renderSemaphore, GetCurrentFrame().renderFence, GetCurrentFrame().commandBuffer, 1);
        Backend::Get()->GetGraphicsQueue().Present(GetCurrentFrame().renderSemaphore, &imageIndex);
        PROFILER_SCOPE_END("Queue Submit & Present", PROFILER_THREAD_RENDER);

        m_frameNumber++;
    }

    void Renderer::OnLevelInstalled(const Event::ELevelInstalled& ev)
    {
        m_hasLevelInstalled = true;
        return;
        Texture* txt = Resources::ResourceManager::Get()->GetResource<Texture>("Resources/Engine/Textures/Tests/empire_diffuse.png");

        WriteDescriptorSet textureWrite = WriteDescriptorSet{
            .dstSet          = m_textureDescriptor._ptr,
            .dstBinding      = 0,
            .descriptorCount = 1,
            .descriptorType  = DescriptorType::CombinedImageSampler,
            .imageView       = txt->GetImage()._ptrImgView,
            .imageLayout     = ImageLayout::ShaderReadOnlyOptimal,
            .sampler         = txt->GetSampler()._ptr,
        };

        Vector<WriteDescriptorSet> vv0;
        vv0.push_back(textureWrite);
        DescriptorSet::UpdateDescriptorSets(vv0);
    }

    void Renderer::OnLevelUninstalled(const Event::ELevelUninstalled& ev)
    {
        m_hasLevelInstalled = false;
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

} // namespace Lina::Graphics
