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

#include "Graphics/Core/WorldRenderer.hpp"
#include "Profiling/Profiler.hpp"
#include "Graphics/Core/RenderEngine.hpp"
#include "Graphics/Resource/Model.hpp"
#include "Graphics/Resource/ModelNode.hpp"
#include "EventSystem/EventSystem.hpp"
#include "EventSystem/WorldEvents.hpp"
#include "Graphics/Utility/Vulkan/VulkanUtility.hpp"
#include "Graphics/Components/RenderableComponent.hpp"
#include "Graphics/Resource/Texture.hpp"
#include "World/Core/World.hpp"
#include "Graphics/Components/ModelNodeComponent.hpp"
#include "Graphics/Components/CameraComponent.hpp"
#include "Graphics/Core/SurfaceRenderer.hpp"

namespace Lina::Graphics
{
    Atomic<uint32> WorldRenderer::s_worldCounter = 0;

    template <typename T> void AddRenderables(World::EntityWorld* world, WorldRenderer::RenderWorldData& data)
    {
        uint32 maxSize = 0;
        auto   view    = world->View<T>(&maxSize);

        for (uint32 i = 0; i < maxSize; i++)
        {
            auto* renderable = view[i];
            if (renderable != nullptr)
                renderable->m_renderableID = data.allRenderables.AddItem(renderable);
        }
    }

    bool WorldRenderer::Initialize(GUIBackend* guiBackend, WindowManager* windowManager, RenderEngine* eng)
    {
        Renderer::Initialize(guiBackend, windowManager, eng);

        m_type = RendererType::WorldRenderer;
        m_cameraSystem.Initialize(windowManager);
        for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            m_targetWorldData.indirectBuffer[i] = Buffer{.size        = OBJ_BUFFER_MAX * sizeof(VkDrawIndexedIndirectCommand),
                                                         .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::TransferDst) | GetBufferUsageFlags(BufferUsageFlags::StorageBuffer) | GetBufferUsageFlags(BufferUsageFlags::IndirectBuffer),
                                                         .memoryUsage = MemoryUsageFlags::CpuToGpu};

            m_targetWorldData.indirectBuffer[i].Create();

            // Scene data
            m_targetWorldData.sceneDataBuffer[i] = Buffer{
                .size        = sizeof(GPUSceneData),
                .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::UniformBuffer),
                .memoryUsage = MemoryUsageFlags::CpuToGpu,
            };
            m_targetWorldData.sceneDataBuffer[i].Create();

            // Light data
            m_targetWorldData.lightDataBuffer[i] = Buffer{
                .size        = sizeof(GPULightData),
                .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::UniformBuffer),
                .memoryUsage = MemoryUsageFlags::CpuToGpu,
            };
            m_targetWorldData.lightDataBuffer[i].Create();

            // View data
            m_targetWorldData.viewDataBuffer[i] = Buffer{
                .size        = sizeof(GPUViewData),
                .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::UniformBuffer),
                .memoryUsage = MemoryUsageFlags::CpuToGpu,
            };
            m_targetWorldData.viewDataBuffer[i].Create();

            // Obj data
            m_targetWorldData.objDataBuffer[i] = Buffer{
                .size        = sizeof(GPUObjectData) * OBJ_BUFFER_MAX,
                .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::StorageBuffer),
                .memoryUsage = MemoryUsageFlags::CpuToGpu,
            };
            m_targetWorldData.objDataBuffer[i].Create();
            m_targetWorldData.objDataBuffer[i].boundSet     = &m_targetWorldData.passDescriptor;
            m_targetWorldData.objDataBuffer[i].boundBinding = 1;
            m_targetWorldData.objDataBuffer[i].boundType    = DescriptorType::StorageBuffer;

            // Pass descriptor
            m_targetWorldData.passDescriptor = DescriptorSet{
                .setCount = 1,
                .pool     = m_descriptorPool._ptr,
            };

            m_targetWorldData.passDescriptor.Create(RenderEngine::Get()->GetLayout(DescriptorSetType::PassSet));
            m_targetWorldData.passDescriptor.BeginUpdate();
            m_targetWorldData.passDescriptor.AddBufferUpdate(m_targetWorldData.sceneDataBuffer[i], m_targetWorldData.sceneDataBuffer[i].size, 0, DescriptorType::UniformBuffer);
            m_targetWorldData.passDescriptor.AddBufferUpdate(m_targetWorldData.viewDataBuffer[i], m_targetWorldData.viewDataBuffer[i].size, 1, DescriptorType::UniformBuffer);
            m_targetWorldData.passDescriptor.AddBufferUpdate(m_targetWorldData.lightDataBuffer[i], m_targetWorldData.lightDataBuffer[i].size, 2, DescriptorType::UniformBuffer);
            m_targetWorldData.passDescriptor.AddBufferUpdate(m_targetWorldData.objDataBuffer[i], m_targetWorldData.objDataBuffer[i].size, 3, DescriptorType::StorageBuffer);
            m_targetWorldData.passDescriptor.SendUpdate();
        }

        m_targetWorldData.opaquePass.Initialize(DrawPassMask::Opaque, 1000.0f);
        m_targetWorldData.allRenderables.Initialize(250, nullptr);
        m_targetWorldData.allRenderables.Initialize(250, nullptr);

        const Vector2i& display = m_renderEngine->GetScreen().DisplayResolution();
        CreateTextures(display, true);

        Event::EventSystem::Get()->Connect<Event::EComponentCreated, &WorldRenderer::OnComponentCreated>(this);
        Event::EventSystem::Get()->Connect<Event::EComponentDestroyed, &WorldRenderer::OnComponentDestroyed>(this);

        return true;
    }

    void WorldRenderer::Shutdown()
    {
        Event::EventSystem::Get()->Disconnect<Event::EComponentCreated>(this);
        Event::EventSystem::Get()->Disconnect<Event::EComponentDestroyed>(this);

        DestroyTextures();

        m_targetWorldData.allRenderables.Reset();
        m_targetWorldData.extractedRenderables.clear();

        for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            m_targetWorldData.objDataBuffer[i].Destroy();
            m_targetWorldData.lightDataBuffer[i].Destroy();
            m_targetWorldData.viewDataBuffer[i].Destroy();
            m_targetWorldData.indirectBuffer[i].Destroy();
            m_targetWorldData.sceneDataBuffer[i].Destroy();

            if (m_worldPostProcessMaterials[i] != nullptr)
                delete m_worldPostProcessMaterials[i];
        }
    }

    void WorldRenderer::RenderWorld(uint32 frameIndex, const CommandBuffer& cmd, RenderWorldData& data)
    {
        data.opaquePass.UpdateViewData(data.viewDataBuffer[frameIndex], data.playerView);
        data.opaquePass.RecordDrawCommands(cmd, m_renderEngine->GetMeshEntries(), data.indirectBuffer[frameIndex]);
    }

    void WorldRenderer::SyncData()
    {
        PROFILER_FUNC(PROFILER_THREAD_MAIN);

        Renderer::SyncData();

        // If display resolution is changed, update textures.
        const auto& ext = m_targetWorldData.finalColorTexture->GetExtent();
        if (ext.width != m_renderResolution.x || ext.height != m_renderResolution.y)
        {
            // Need clean queues.
            Backend::Get()->WaitIdle();
            DestroyTextures();
            CreateTextures(m_renderResolution, false);

            for (auto s : m_finalTextureListeners)
                s->SetOffscreenTexture(m_mask.IsSet(RM_ApplyPostProcessing) ? m_targetWorldData.postProcessTexture : m_targetWorldData.finalColorTexture);
        }

        auto& wd = m_targetWorldData;
        if (wd.world != nullptr)
        {
            wd.extractedRenderables.clear();
            const auto& renderables = wd.allRenderables.GetItems();

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
                wd.extractedRenderables.push_back(data);
            }

            wd.opaquePass.PrepareRenderData(wd.extractedRenderables, wd.playerView);
        }
    }

    void WorldRenderer::Tick()
    {
        auto& wd = m_targetWorldData;
        if (wd.world == nullptr)
            return;

        auto* activeCamera = wd.world->GetActiveCamera();
        if (activeCamera == nullptr)
            return;

        m_cameraSystem.CalculateCamera(activeCamera, m_aspectRatio);
        wd.playerView.Tick(activeCamera->GetEntity()->GetPosition(), activeCamera->GetView(), activeCamera->GetProjection());
    }

    CommandBuffer* WorldRenderer::Render(uint32 frameIndex, Fence& fence)
    {
        PROFILER_FUNC(PROFILER_THREAD_RENDER);

        auto& wd = m_targetWorldData;

        auto& cmd = m_cmds[frameIndex];
        cmd.Reset(true);
        cmd.Begin(GetCommandBufferFlags(CommandBufferFlags::OneTimeSubmit));

        const auto& screen = m_renderEngine->GetScreen();

        const Viewport viewport = Viewport{
            .x      = 0,
            .y      = 0,
            .width  = static_cast<float>(m_renderResolution.x),
            .height = static_cast<float>(m_renderResolution.y),
        };

        const Recti  scissors             = Recti(0, 0, m_renderResolution.x, m_renderResolution.y);
        const Recti  defaultRenderArea    = Recti(Vector2(viewport.x, viewport.y), Vector2(viewport.width, viewport.height));
        const uint32 depthTransitionFlags = GetPipelineStageFlags(PipelineStageFlags::EarlyFragmentTests) | GetPipelineStageFlags(PipelineStageFlags::LateFragmentTests);

        cmd.CMD_SetViewport(viewport);
        cmd.CMD_SetScissors(scissors);

        // Merged object buffer.
        uint64 offset = 0;
        cmd.CMD_BindVertexBuffers(0, 1, m_renderEngine->GetGPUVertexBuffer()._ptr, &offset);
        cmd.CMD_BindIndexBuffers(m_renderEngine->GetGPUIndexBuffer()._ptr, 0, IndexType::Uint32);

        // Global set.
        // cmd.CMD_BindDescriptorSets(PipelineBindPoint::Graphics, RenderEngine::Get()->GetGlobalAndPassLayouts()._ptr, 0, 1, &frame.globalDescriptor, 0, nullptr);

        // Pass set.
        cmd.CMD_BindDescriptorSets(PipelineBindPoint::Graphics, RenderEngine::Get()->GetGlobalAndPassLayouts()._ptr, 1, 1, &wd.passDescriptor, 0, nullptr);

        // Global - scene data.
        wd.sceneDataBuffer[frameIndex].CopyInto(&wd.sceneData, sizeof(GPUSceneData));

        // Global - light data.
        wd.lightDataBuffer[frameIndex].CopyInto(&wd.lightData, sizeof(GPULightData));

        // Per render pass - obj data.
        Vector<GPUObjectData> gpuObjectData;

        for (auto& r : wd.extractedRenderables)
        {
            // Object data.
            GPUObjectData objData;
            objData.modelMatrix = r.modelMatrix;
            gpuObjectData.push_back(objData);
        }

        wd.objDataBuffer[frameIndex].CopyInto(gpuObjectData.data(), sizeof(GPUObjectData) * gpuObjectData.size());

        // Put necessary images to correct layouts.
        auto mainPassImage      = wd.finalColorTexture->GetImage()._allocatedImg.image;
        auto mainPassImageView  = wd.finalColorTexture->GetImage()._ptrImgView;
        auto mainPassDepthImage = wd.finalDepthTexture->GetImage()._allocatedImg.image;
        auto mainPassDepthView  = wd.finalDepthTexture->GetImage()._ptrImgView;

        // Transition to optimal
        cmd.CMD_ImageTransition(mainPassImage, ImageLayout::Undefined, ImageLayout::ColorOptimal, ImageAspectFlags::AspectColor, AccessFlags::None, AccessFlags::ColorAttachmentWrite, PipelineStageFlags::TopOfPipe, PipelineStageFlags::ColorAttachmentOutput);
        cmd.CMD_ImageTransition(mainPassDepthImage, ImageLayout::Undefined, ImageLayout::DepthStencilOptimal, ImageAspectFlags::AspectDepth, AccessFlags::None, AccessFlags::DepthStencilAttachmentWrite, depthTransitionFlags, depthTransitionFlags);

        // ********* MAIN PASS *********
        {
            PROFILER_SCOPE_START("Main Pass", PROFILER_THREAD_RENDER);
            cmd.CMD_BeginRenderingDefault(mainPassImageView, mainPassDepthView, defaultRenderArea);
            RenderWorld(frameIndex, cmd, wd);
            cmd.CMD_EndRendering();

            // Transition to shader read
            cmd.CMD_ImageTransition(mainPassImage, ImageLayout::ColorOptimal, ImageLayout::ShaderReadOnlyOptimal, ImageAspectFlags::AspectColor, AccessFlags::None, AccessFlags::ColorAttachmentWrite, PipelineStageFlags::TopOfPipe,
                                    PipelineStageFlags::ColorAttachmentOutput);

            PROFILER_SCOPE_END("Main Pass", PROFILER_THREAD_RENDER);
        }

        if (m_mask.IsSet(RM_ApplyPostProcessing))
        {
            auto ppImage     = wd.postProcessTexture->GetImage()._allocatedImg.image;
            auto ppImageView = wd.postProcessTexture->GetImage()._ptrImgView;

            cmd.CMD_ImageTransition(ppImage, ImageLayout::Undefined, ImageLayout::ColorOptimal, ImageAspectFlags::AspectColor, AccessFlags::None, AccessFlags::ColorAttachmentWrite, PipelineStageFlags::TopOfPipe, PipelineStageFlags::ColorAttachmentOutput);

            cmd.CMD_BeginRenderingDefault(ppImageView, defaultRenderArea);
            m_worldPostProcessMaterials[frameIndex]->Bind(cmd, MaterialBindFlag::BindDescriptor | MaterialBindFlag::BindPipeline);
            cmd.CMD_Draw(3, 1, 0, 0);
            cmd.CMD_EndRendering();

            cmd.CMD_ImageTransition(ppImage, ImageLayout::ColorOptimal, ImageLayout::ShaderReadOnlyOptimal, ImageAspectFlags::AspectColor, AccessFlags::None, AccessFlags::ColorAttachmentWrite, PipelineStageFlags::TopOfPipe,
                                    PipelineStageFlags::ColorAttachmentOutput);
        }

        cmd.End();
        return &cmd;
    }

    void WorldRenderer::OnComponentCreated(const Event::EComponentCreated& ev)
    {
        if (m_targetWorldData.world == nullptr || ev.world != m_targetWorldData.world)
            return;

        if (ev.ptr->GetComponentMask().IsSet(World::ComponentMask::Renderable))
        {
            auto renderable            = static_cast<RenderableComponent*>(ev.ptr);
            renderable->m_renderableID = m_targetWorldData.allRenderables.AddItem(renderable);
        }
    }

    void WorldRenderer::OnComponentDestroyed(const Event::EComponentDestroyed& ev)
    {
        if (m_targetWorldData.world == nullptr || ev.world != m_targetWorldData.world)
            return;

        if (ev.ptr->GetComponentMask().IsSet(World::ComponentMask::Renderable))
        {
            m_targetWorldData.allRenderables.RemoveItem(static_cast<RenderableComponent*>(ev.ptr)->GetRenderableID());
        }
    }

    void WorldRenderer::CreateTextures(const Vector2i& res, bool createMaterials)
    {
        m_targetWorldData.finalColorTexture = VulkanUtility::CreateDefaultPassTextureColor(true, res.x, res.y);
        m_targetWorldData.finalDepthTexture = VulkanUtility::CreateDefaultPassTextureDepth(true, res.x, res.y);

        const String   sidStr = "World: " + TO_STRING(s_worldCounter++);
        const StringID sid    = TO_SID(sidStr);

        if (m_mask.IsSet(RM_ApplyPostProcessing))
        {
            for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
            {
                if (createMaterials)
                {
                    m_worldPostProcessMaterials[i] = new Material();
                    m_worldPostProcessMaterials[i]->SetShader(RenderEngine::Get()->GetEngineShader(EngineShaderType::SQPostProcess));
                }
                m_worldPostProcessMaterials[i]->SetTexture(0, m_targetWorldData.finalColorTexture);
                m_worldPostProcessMaterials[i]->CheckUpdatePropertyBuffers();
            }

            m_targetWorldData.postProcessTexture = VulkanUtility::CreateDefaultPassTextureColor(true, res.x, res.y);
            m_targetWorldData.postProcessTexture->SetUserManaged(true);
            m_targetWorldData.postProcessTexture->ChangeSID(sid);
            Resources::ResourceManager::Get()->GetCache<Graphics::Texture>()->AddResource(sid, m_targetWorldData.postProcessTexture);
        }
        else
        {
            m_targetWorldData.finalColorTexture->SetUserManaged(true);
            m_targetWorldData.finalColorTexture->ChangeSID(sid);
            Resources::ResourceManager::Get()->GetCache<Graphics::Texture>()->AddResource(sid, m_targetWorldData.finalColorTexture);
        }
    }

    void WorldRenderer::DestroyTextures()
    {
        delete m_targetWorldData.finalDepthTexture;

        if (m_mask.IsSet(RM_ApplyPostProcessing))
        {
            delete m_targetWorldData.finalColorTexture;
            Resources::ResourceManager::Get()->UnloadUserManaged(m_targetWorldData.postProcessTexture);
        }
        else
        {
            Resources::ResourceManager::Get()->UnloadUserManaged(m_targetWorldData.finalColorTexture);
        }
    }

    void WorldRenderer::AddFinalTextureListener(SurfaceRenderer* listener)
    {
        m_finalTextureListeners.push_back(listener);
    }

    void WorldRenderer::RemoveFinalTextureListener(SurfaceRenderer* listener)
    {
        auto it = linatl::find_if(m_finalTextureListeners.begin(), m_finalTextureListeners.end(), [listener](SurfaceRenderer* sr) { return sr == listener; });

        if (it != m_finalTextureListeners.end())
            m_finalTextureListeners.erase(it);
    }

    Texture* WorldRenderer::GetFinalTexture()
    {
        if (m_mask.IsSet(RM_ApplyPostProcessing))
            return m_targetWorldData.postProcessTexture;
        else
            return m_targetWorldData.finalColorTexture;
    }

    void WorldRenderer::SetAspectRatio(float aspect)
    {
        if (m_aspectRatio == aspect)
            return;

        SimpleAction act;
        act.Action = [this, aspect]() { m_aspectRatio = aspect; };
        m_syncedActions.push_back(act);
    }

    void WorldRenderer::SetRenderResolution(const Vector2i& res)
    {
        if (m_renderResolution == res)
            return;

        SimpleAction act;
        act.Action = [this, res]() { m_renderResolution = res; };
        m_syncedActions.push_back(act);
    }

    void WorldRenderer::SetWorld(World::EntityWorld* world)
    {
        SimpleAction act;
        act.Action = [this, world]() {
            m_targetWorldData.allRenderables.Reset();
            m_targetWorldData.extractedRenderables.clear();
            m_targetWorldData.world = world;
            AddRenderables<Graphics::ModelNodeComponent>(m_targetWorldData.world, m_targetWorldData);
        };
        m_syncedActions.push_back(act);
    }

    void WorldRenderer::RemoveWorld()
    {
        SimpleAction act;
        act.Action = [this]() {
            m_targetWorldData.allRenderables.Reset();
            m_targetWorldData.extractedRenderables.clear();
            m_targetWorldData.world = nullptr;
        };
        m_syncedActions.push_back(act);
    }

} // namespace Lina::Graphics
