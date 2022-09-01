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
#include "EventSystem/LevelEvents.hpp"
#include "Resource/Texture.hpp"
#include "Utility/Vulkan/VulkanUtility.hpp"
#include "Core/LevelManager.hpp"
#include "Core/Level.hpp"
#include "Utility/Vulkan/vk_mem_alloc.h"
#include <vulkan/vulkan.h>
#include "Resource/ModelNode.hpp"
#include "Resource/Mesh.hpp"

namespace Lina::Graphics
{

#define RENDERABLES_STACK_SIZE 1000

    void Renderer::Initialize()
    {
        Event::EventSystem::Get()->Connect<Event::ELevelUninstalled, &Renderer::OnLevelUninstalled>(this);
        Event::EventSystem::Get()->Connect<Event::ELevelInstalled, &Renderer::OnLevelInstalled>(this);

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

        m_scenePropertiesBuffer = Buffer{
            .size        = FRAME_OVERLAP * VulkanUtility::PadUniformBufferSize(sizeof(GPUSceneData)),
            .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::UniformBuffer),
            .memoryUsage = MemoryUsageFlags::CpuToGpu,
        };

        m_scenePropertiesBuffer.Create();

        m_globalDescriptor = DescriptorSet{
            .setCount = 1,
            .pool     = RenderEngine::Get()->GetDescriptorPool()._ptr,
        };

        m_globalDescriptor.AddSetLayout(RenderEngine::Get()->GetLayout(0)._ptr);
        m_globalDescriptor.Create();

        m_textureDescriptor = DescriptorSet{
            .setCount = 1,
            .pool     = RenderEngine::Get()->GetDescriptorPool()._ptr,
        };

        m_textureDescriptor.AddSetLayout(RenderEngine::Get()->GetLayout(2)._ptr);
        m_textureDescriptor.Create();

        WriteDescriptorSet sceneWrite = WriteDescriptorSet{
            .buffer          = m_scenePropertiesBuffer._ptr,
            .offset          = 0,
            .range           = sizeof(GPUSceneData),
            .dstSet          = m_globalDescriptor._ptr,
            .dstBinding      = 0,
            .descriptorCount = 1,
            .descriptorType  = DescriptorType::UniformBufferDynamic};

        Vector<WriteDescriptorSet> vv0;
        vv0.push_back(sceneWrite);
        DescriptorSet::UpdateDescriptorSets(vv0);

        for (int i = 0; i < FRAME_OVERLAP; i++)
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
                .size        = sizeof(GPUObjectData) * 5,
                .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::StorageBuffer),
                .memoryUsage = MemoryUsageFlags::CpuToGpu,
            };
            f.objDataBuffer.Create();

            f.objDataDescriptor = DescriptorSet{
                .setCount = 1,
                .pool     = RenderEngine::Get()->GetDescriptorPool()._ptr,
            };

            f.objDataDescriptor.AddSetLayout(RenderEngine::Get()->GetLayout(1)._ptr);
            f.objDataDescriptor.Create();

            WriteDescriptorSet objDataWrite = WriteDescriptorSet{
                .buffer          = f.objDataBuffer._ptr,
                .offset          = 0,
                .range           = sizeof(GPUObjectData) * 5,
                .dstSet          = f.objDataDescriptor._ptr,
                .dstBinding      = 0,
                .descriptorCount = 1,
                .descriptorType  = DescriptorType::StorageBuffer};

            Vector<WriteDescriptorSet> vv;
            vv.push_back(objDataWrite);
            DescriptorSet::UpdateDescriptorSets(vv);
        }

        // Views
        m_playerView.m_viewType = ViewType::Player;
        m_views.push_back(&m_playerView);

        // Feature renderers.
    }

    void Renderer::Tick()
    {
        PROFILER_FUNC(PROFILER_THREAD_MAIN);
        m_cameraSystem.Tick();
        m_playerView.CalculateFrustum(m_cameraSystem.GetPos(), m_cameraSystem.GetView(), m_cameraSystem.GetProj());
    }

    void Renderer::FetchAndExtract(World::EntityWorld* world)
    {
        return;

        PROFILER_FUNC(PROFILER_THREAD_MAIN);
        if (world == nullptr)
            return;
        // Determine views.

        // Determine views.

        // Calculate visible renderables for each view.
       //Taskflow tf;
       //tf.for_each(m_views.begin(), m_views.end(), [&](View* v) {
       //    v->CalculateVisibility(m_renderables.data(), m_nextRenderableID);
       //});
       //JobSystem::Get()->GetMainExecutor().Run(tf).wait();

    }

    void Renderer::OnLevelInstalled(const Event::ELevelInstalled& ev)
    {
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

        auto w = World::LevelManager::Get()->GetCurrentLevel()->GetWorld();

        uint32      maxSize;
        const auto& v = w.View<ModelNodeComponent>(&maxSize);

        for (uint32 i = 0; i < maxSize; i++)
        {
            if (v[i] != nullptr)
                AddRenderable(v[i]);
        }
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

        // m_featureRendererManager.PrepareRenderData();

        GetCurrentFrame().renderFence.Wait(true, 1.0f);
        GetCurrentFrame().renderFence.Reset();

        uint32 imageIndex = m_backend->m_swapchain.AcquireNextImage(1.0, GetCurrentFrame().presentSemaphore);

        GetCurrentFrame().commandBuffer.Reset();
        GetCurrentFrame().commandBuffer.Begin(GetCommandBufferFlags(CommandBufferFlags::OneTimeSubmit));

        m_renderPass.Begin(m_framebuffers[imageIndex], GetCurrentFrame().commandBuffer);

        GPUSceneData sceneData;
        sceneData.proj     = m_cameraSystem.GetProj();
        sceneData.view     = m_cameraSystem.GetView();
        sceneData.viewProj = sceneData.proj * sceneData.view;

        uint32 frameIndex      = m_frameNumber % FRAME_OVERLAP;
        sceneData.ambientColor = Vector4(Math::Sin((Time::GetCPUTime() * 5) + 1) * 0.5f, 0, 0, 1);
        m_scenePropertiesBuffer.CopyIntoPadded(&sceneData, sizeof(GPUSceneData), VulkanUtility::PadUniformBufferSize(sizeof(GPUSceneData)) * frameIndex);

        Vector<GPUObjectData> objDatas;
        for (int i = 0; i < 1; i++)
        {
            GPUObjectData data;
            data.modelMatrix = Matrix::Translate(Vector3(i, 0, 0));

            objDatas.push_back(data);
        }
        GetCurrentFrame().objDataBuffer.CopyInto(objDatas.data(), sizeof(GPUObjectData) * objDatas.size());
//
//   for (int i = 0; i < 1; i++)
//   {
//       Material * mat = RenderEngine::Get()->GetPlaceholderMaterial();
//       auto& pipeline = mat->GetShaderHandle().value->GetPipeline();
//       pipeline.Bind(GetCurrentFrame().commandBuffer, PipelineBindPoint::Graphics);
//
//       auto& renderer = RenderEngine::Get()->GetLevelRenderer();
//       DescriptorSet& descSet = renderer.GetGlobalSet();
//       DescriptorSet& objSet = renderer.GetObjectSet();
//       DescriptorSet& txtSet = renderer.GetTextureSet();
//
//       uint32_t uniformOffset = VulkanUtility::PadUniformBufferSize(sizeof(GPUSceneData)) * renderer.GetFrameIndex();
//
//       GetCurrentFrame().commandBuffer.CMD_BindDescriptorSets(PipelineBindPoint::Graphics, pipeline._layout, 0, 1, &descSet, 1, &uniformOffset);
//
//       GetCurrentFrame().commandBuffer.CMD_BindDescriptorSets(PipelineBindPoint::Graphics, pipeline._layout, 1, 1, &objSet, 0, nullptr);
//
//       GetCurrentFrame().commandBuffer.CMD_BindDescriptorSets(PipelineBindPoint::Graphics, pipeline._layout, 2, 1, &txtSet, 0, nullptr);
//
//
//       for (auto mr : m_renderables)
//       {
//           Graphics::MeshPushConstants constants;
//           constants.renderMatrix = Matrix::Translate(Vector3(0, 0, 0));
//           GetCurrentFrame().commandBuffer.CMD_PushConstants(mat->GetShaderHandle().value->GetPipeline()._layout, GetShaderStage(ShaderStage::Vertex), 0, sizeof(Graphics::MeshPushConstants), &constants);
//
//           Model* m = mr->m_modelHandle.IsValid() ? mr->m_modelHandle.value : RenderEngine::Get()->GetPlaceholderModel();
//           ModelNode* n = mr->m_modelHandle.IsValid() ?  RenderEngine::Get()->GetPlaceholderModelNode() :m->GetNodes()[mr->m_nodeIndex];
//           
//           auto& meshes = n->GetMeshes();
//           int k = 0;
//
//           for (auto mesh : meshes)
//           {
//               uint64 offset = 0;
//               GetCurrentFrame().commandBuffer.CMD_BindVertexBuffers(0, 1, mesh->GetGPUVtxBuffer()._ptr, &offset);
//               GetCurrentFrame().commandBuffer.CMD_BindIndexBuffers(mesh->GetGPUIndexBuffer()._ptr, 0, IndexType::Uint32);
//               // buffer.CMD_Draw(static_cast<uint32>(rp.mesh->GetVertices().size()), 1, 0, i);
//               GetCurrentFrame().commandBuffer.CMD_DrawIndexed(static_cast<uint32>(mesh->GetIndexSize()), 1, 0, 0, 0);
//               k++;
//           }
//          
//         
//       }
//      // for (auto& rp : pair)
//      // {
//      //     
//      // }
//   }

        // Render commands.
        // m_featureRendererManager.Submit(GetCurrentFrame().commandBuffer);

        m_renderPass.End(GetCurrentFrame().commandBuffer);
        GetCurrentFrame().commandBuffer.End();

        PROFILER_SCOPE_START("Queue Submit & Present", PROFILER_THREAD_RENDER);
        // Submit command waits on the present semaphore, e.g. it waits for the acquired image to be ready.
        // Then submits command, and signals render semaphore when its submitted.
        Backend::Get()->GetGraphicsQueue().Submit(GetCurrentFrame().presentSemaphore, GetCurrentFrame().renderSemaphore, GetCurrentFrame().renderFence, GetCurrentFrame().commandBuffer, 1);
        Backend::Get()->GetGraphicsQueue().Present(GetCurrentFrame().renderSemaphore, &imageIndex);
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
            m_frames[i].objDataBuffer.Destroy();
        }

        m_scenePropertiesBuffer.Destroy();

        for (int i = 0; i < m_framebuffers.size(); i++)
            m_framebuffers[i].Destroy();
    }

    void Renderer::AddRenderable(ModelNodeComponent* comp)
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

    void Renderer::RemoveRenderable(ModelNodeComponent* comp)
    {
        m_renderables[comp->m_renderableID] = nullptr;
        m_availableRenderableIDs.push(comp->m_renderableID);
    }

} // namespace Lina::Graphics
