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
#include "Graphics/PipelineObjects/Swapchain.hpp"
#include "Graphics/PipelineObjects/CommandPool.hpp"
#include "Graphics/Core/RenderData.hpp"
#include "Profiling/Profiler.hpp"
#include "Graphics/Core/Backend.hpp"
#include "Graphics/Data/Vertex.hpp"
#include "Graphics/Core/RenderEngine.hpp"
#include "Graphics/Resource/Model.hpp"
#include "Graphics/Resource/ModelNode.hpp"
#include "Resource/Core/ResourceManager.hpp"
#include "EventSystem/WindowEvents.hpp"
#include "EventSystem/EventSystem.hpp"
#include "Graphics/Core/GUIBackend.hpp"
#include "EventSystem/WorldEvents.hpp"
#include "Graphics/Utility/Vulkan/VulkanUtility.hpp"
#include "Graphics/Components/RenderableComponent.hpp"
#include "Graphics/Resource/Texture.hpp"
#include "World/Core/World.hpp"
#include "Graphics/Components/ModelNodeComponent.hpp"
#include "Graphics/Components/CameraComponent.hpp"
#include "Graphics/Core/GUIBackend.hpp"
#include "Graphics/Platform/LinaVGIncl.hpp"
#include "EventSystem/GraphicsEvents.hpp"

#define LINAVG_TEXT_SUPPORT
#include "LinaVG/LinaVG.hpp"

namespace Lina::Graphics
{

    Atomic<uint32> Renderer::s_worldCounter = 0;

    template <typename T> void AddRenderables(World::EntityWorld* world, Renderer::RenderWorldData* data)
    {
        uint32 maxSize = 0;
        auto   view    = world->View<T>(&maxSize);

        for (uint32 i = 0; i < maxSize; i++)
        {
            auto* renderable = view[i];
            if (renderable != nullptr)
                renderable->m_renderableID = data->allRenderables.AddItem(renderable);
        }
    }

    bool Renderer::Initialize(GUIBackend* guiBackend, WindowManager* windowManager, RenderEngine* eng)
    {
        if (m_initialized)
        {
            LINA_ERR("[Renderer] -> Failed initialization, already initialized!");
            return false;
        }

        if (m_renderMask.IsSet(RM_SwapchainOwner) && m_swapchain == nullptr)
        {
            LINA_ERR("[Renderer] -> Failed initialization, render mask is set to own a swapchain but no swapchain is provided!");
            return false;
        }

        if (m_renderMask.IsSet(RM_WorldToSurface))
        {
            for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
            {
                m_worldMaterials[i] = new Material();
                m_worldMaterials[i]->SetShader(RenderEngine::Get()->GetEngineShader(EngineShaderType::SQPostProcess));
            }
        }

        m_initialized   = true;
        m_renderEngine  = eng;
        m_windowManager = windowManager;
        m_guiBackend    = guiBackend;
        m_cameraSystem.Initialize(windowManager);

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

        Event::EventSystem::Get()->Connect<Event::EComponentCreated, &Renderer::OnComponentCreated>(this);
        Event::EventSystem::Get()->Connect<Event::EComponentDestroyed, &Renderer::OnComponentDestroyed>(this);
        Event::EventSystem::Get()->Connect<Event::EWindowResized, &Renderer::OnWindowResized>(this);

        return true;
    }

    void Renderer::Shutdown()
    {
        Event::EventSystem::Get()->Disconnect<Event::EComponentCreated>(this);
        Event::EventSystem::Get()->Disconnect<Event::EComponentDestroyed>(this);
        Event::EventSystem::Get()->Disconnect<Event::EWindowResized>(this);

        if (m_targetWorldData != nullptr)
            RemoveWorldImpl();

        for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            if (m_worldMaterials[i] != nullptr)
                delete m_worldMaterials[i];
        }
    }

    void Renderer::RenderWorld(uint32 frameIndex, const CommandBuffer& cmd, RenderWorldData* data)
    {
        data->opaquePass.UpdateViewData(data->viewDataBuffer[frameIndex], data->playerView);
        data->opaquePass.RecordDrawCommands(cmd, m_renderEngine->GetMeshEntries(), data->indirectBuffer[frameIndex]);
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

        if (m_targetWorldData == nullptr && m_worldToSet != nullptr)
        {
            SetWorldImpl();
            m_worldToSet = nullptr;
        }
        else if (m_targetWorldData != nullptr && m_shouldRemoveWorld)
        {
            RemoveWorldImpl();
            m_shouldRemoveWorld = false;
        }

        auto wd = m_targetWorldData;
        if (wd != nullptr)
        {
            wd->extractedRenderables.clear();
            const auto& renderables = wd->allRenderables.GetItems();

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
                wd->extractedRenderables.push_back(data);
            }

            wd->opaquePass.PrepareRenderData(wd->extractedRenderables, wd->playerView);
        }
    }

    void Renderer::Tick()
    {
        auto wd = m_targetWorldData;
        if (wd == nullptr)
            return;

        auto* activeCamera = wd->world->GetActiveCamera();
        if (activeCamera == nullptr)
            return;

        m_cameraSystem.CalculateCamera(wd->world->GetActiveCamera());
        wd->playerView.Tick(activeCamera->GetEntity()->GetPosition(), activeCamera->GetView(), activeCamera->GetProjection());
    }

    bool Renderer::AcquireImage(uint32 frameIndex)
    {
        if (!m_renderMask.IsSet(RM_SwapchainOwner))
        {
            LINA_ERR("[Renderer] -> Can't acquire image on a renderer that's not a swapchain owner!");
            return false;
        }

        VulkanResult res;
        m_acquiredImage = m_swapchain->AcquireNextImage(1.0, m_swapchain->_submitSemaphores[frameIndex], res);
        return CanContinueWithAcquiredImage(res);
    }

    CommandBuffer* Renderer::Render(uint32 frameIndex, Fence& fence)
    {
        auto& wd = m_targetWorldData;

        // Nothing to render.
        if (m_renderMask.GetValue() == 0)
        {
            LINA_WARN("[Renderer] -> Renderer is set to render nothing!");
            return nullptr;
        }

        PROFILER_FUNC(PROFILER_THREAD_RENDER);

        auto& cmd = m_cmds[frameIndex];
        cmd.Reset(true);
        cmd.Begin(GetCommandBufferFlags(CommandBufferFlags::OneTimeSubmit));

        const uint32 depthTransitionFlags = GetPipelineStageFlags(PipelineStageFlags::EarlyFragmentTests) | GetPipelineStageFlags(PipelineStageFlags::LateFragmentTests);
        const Recti  defaultRenderArea    = Recti(Vector2(m_viewport.x, m_viewport.y), Vector2(m_viewport.width, m_viewport.height));

        cmd.CMD_SetViewport(m_viewport);
        cmd.CMD_SetScissors(m_scissors);

        if (m_renderMask.IsSet(RM_RenderWorld) && wd != nullptr)
        {
            // Merged object buffer.
            uint64 offset = 0;
            cmd.CMD_BindVertexBuffers(0, 1, m_renderEngine->GetGPUVertexBuffer()._ptr, &offset);
            cmd.CMD_BindIndexBuffers(m_renderEngine->GetGPUIndexBuffer()._ptr, 0, IndexType::Uint32);

            // Global set.
            // cmd.CMD_BindDescriptorSets(PipelineBindPoint::Graphics, RenderEngine::Get()->GetGlobalAndPassLayouts()._ptr, 0, 1, &frame.globalDescriptor, 0, nullptr);

            // Pass set.
            cmd.CMD_BindDescriptorSets(PipelineBindPoint::Graphics, RenderEngine::Get()->GetGlobalAndPassLayouts()._ptr, 1, 1, &wd->passDescriptor, 0, nullptr);

            // Global - scene data.
            wd->sceneDataBuffer[frameIndex].CopyInto(&wd->sceneData, sizeof(GPUSceneData));

            // Global - light data.
            wd->lightDataBuffer[frameIndex].CopyInto(&wd->lightData, sizeof(GPULightData));

            // Per render pass - obj data.
            Vector<GPUObjectData> gpuObjectData;

            for (auto& r : wd->extractedRenderables)
            {
                // Object data.
                GPUObjectData objData;
                objData.modelMatrix = r.modelMatrix;
                gpuObjectData.push_back(objData);
            }

            wd->objDataBuffer[frameIndex].CopyInto(gpuObjectData.data(), sizeof(GPUObjectData) * gpuObjectData.size());

            // Put necessary images to correct layouts.
            auto mainPassImage      = wd->finalColorTexture->GetImage()._allocatedImg.image;
            auto mainPassImageView  = wd->finalColorTexture->GetImage()._ptrImgView;
            auto mainPassDepthImage = wd->finalDepthTexture->GetImage()._allocatedImg.image;
            auto mainPassDepthView  = wd->finalDepthTexture->GetImage()._ptrImgView;

            // Transition to optimal
            cmd.CMD_ImageTransition(mainPassImage, ImageLayout::Undefined, ImageLayout::ColorOptimal, ImageAspectFlags::AspectColor, AccessFlags::None, AccessFlags::ColorAttachmentWrite, PipelineStageFlags::TopOfPipe,
                                    PipelineStageFlags::ColorAttachmentOutput);
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
        }

        if (!m_renderMask.IsSet(RM_SwapchainOwner))
        {
            cmd.End();
            return &cmd;
        }

        auto swapchainImage          = m_swapchain->_images[m_acquiredImage];
        auto swapchainImageView      = m_swapchain->_imageViews[m_acquiredImage];
        auto swapchainDepthImage     = m_swapchain->_depthImages[m_acquiredImage]._allocatedImg.image;
        auto swapchainDepthImageView = m_swapchain->_depthImages[m_acquiredImage]._ptrImgView;

        // Transition swapchain to optimal
        cmd.CMD_ImageTransition(swapchainImage, ImageLayout::Undefined, ImageLayout::ColorOptimal, ImageAspectFlags::AspectColor, AccessFlags::None, AccessFlags::ColorAttachmentWrite, PipelineStageFlags::TopOfPipe, PipelineStageFlags::ColorAttachmentOutput);
        cmd.CMD_ImageTransition(swapchainDepthImage, ImageLayout::Undefined, ImageLayout::DepthStencilOptimal, ImageAspectFlags::AspectDepth, AccessFlags::None, AccessFlags::DepthStencilAttachmentWrite, depthTransitionFlags, depthTransitionFlags);

        // ********* FINAL & PP PASS *********
        {
            PROFILER_SCOPE_START("Final Pass", PROFILER_THREAD_RENDER);

            // Issue GUI draw commands.
            if (m_renderMask.IsSet(RM_RenderGUI))
            {
                LinaVG::StartFrame();
                m_guiBackend->Prepare(m_swapchain, m_acquiredImage, &cmd);
                Event::EventSystem::Get()->Trigger<Event::EDrawGUI>({m_swapchain});
            }

            cmd.CMD_BeginRenderingDefault(swapchainImageView, swapchainDepthImageView, defaultRenderArea);

            // Draw the rendered world to surface if needed.
            if (m_renderMask.IsAllSet(RM_WorldToSurface | RM_RenderWorld) && m_worldMaterials != nullptr && wd != nullptr)
            {
                m_worldMaterials[frameIndex]->Bind(cmd, MaterialBindFlag::BindPipeline | MaterialBindFlag::BindDescriptor);
                cmd.CMD_Draw(3, 1, 0, 0);
            }

            // Render GUI on top
            // if (m_renderMask.IsSet(RM_RenderGUI))
            //     m_guiBackend->RecordDrawCommands();

            cmd.CMD_EndRendering();

            // Transition to present
            cmd.CMD_ImageTransition(swapchainImage, ImageLayout::ColorOptimal, ImageLayout::PresentSurface, ImageAspectFlags::AspectColor, AccessFlags::ColorAttachmentWrite, AccessFlags::None, PipelineStageFlags::ColorAttachmentOutput,
                                    PipelineStageFlags::BottomOfPipe);

            PROFILER_SCOPE_END("Final Pass", PROFILER_THREAD_RENDER);
            LinaVG::EndFrame();
        }

        cmd.End();
        return &cmd;
    }

    bool Renderer::CanContinueWithAcquiredImage(VulkanResult res)
    {
        if (!m_recreateSwapchain)
            m_newSwapchainSize = m_swapchain->size;

        if (m_recreateSwapchain && res == VulkanResult::OutOfDateKHR || res == VulkanResult::SuboptimalKHR)
        {
            m_recreateSwapchain = false;

            Backend::Get()->WaitIdle();

            // Swapchain
            m_swapchain->Destroy(false);
            m_swapchain->size = m_newSwapchainSize;
            m_swapchain->Create(LINA_MAIN_SWAPCHAIN_ID);

            // Make sure we always match swapchain
            m_newSwapchainSize = m_swapchain->size;

            // Recreate the textures for every world we render.
            auto wd = m_targetWorldData;
            if (wd != nullptr)
            {
                Resources::ResourceManager::Get()->UnloadUserManaged(wd->finalColorTexture);
                delete wd->finalDepthTexture;
                wd->finalColorTexture = VulkanUtility::CreateDefaultPassTextureColor();
                wd->finalDepthTexture = VulkanUtility::CreateDefaultPassTextureDepth();
                const String   sidStr = "World: " + TO_STRING(wd->world->GetID()) + TO_STRING(s_worldCounter++);
                const StringID sid    = TO_SID(sidStr);
                wd->finalColorTexture->SetUserManaged(true);
                wd->finalColorTexture->ChangeSID(sid);
                Resources::ResourceManager::Get()->GetCache<Graphics::Texture>()->AddResource(sid, wd->finalColorTexture);

                SimpleAction act;
                act.Action = [this, wd]() {
                    for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
                    {
                        if (m_worldMaterials[i] != nullptr)
                        {
                            m_worldMaterials[i]->SetTexture(0, wd->finalColorTexture);
                            m_worldMaterials[i]->CheckUpdatePropertyBuffers();
                        }
                    }
                };
                m_syncedActions.push_back(act);
            }

            m_swapchain->_submitSemaphores[m_renderEngine->GetFrameIndex()].Destroy();
            m_swapchain->_submitSemaphores[m_renderEngine->GetFrameIndex()].Create(false);
            // Backend::Get()->GetGraphicsQueue().Submit(m_swapchain->_submitSemaphores[m_renderEngine->GetFrameIndex()]);

            UpdateViewport(m_newSwapchainSize);

            return false;
        }

        return true;
    }

    void Renderer::OnComponentCreated(const Event::EComponentCreated& ev)
    {
        if (m_targetWorldData == nullptr || ev.world != m_targetWorldData->world)
            return;

        if (ev.ptr->GetComponentMask().IsSet(World::ComponentMask::Renderable))
        {
            auto renderable            = static_cast<RenderableComponent*>(ev.ptr);
            renderable->m_renderableID = m_targetWorldData->allRenderables.AddItem(renderable);
        }
    }

    void Renderer::OnComponentDestroyed(const Event::EComponentDestroyed& ev)
    {
        if (m_targetWorldData == nullptr || ev.world != m_targetWorldData->world)
            return;

        if (ev.ptr->GetComponentMask().IsSet(World::ComponentMask::Renderable))
        {
            m_targetWorldData->allRenderables.RemoveItem(static_cast<RenderableComponent*>(ev.ptr)->GetRenderableID());
        }
    }

    void Renderer::OnWindowResized(const Event::EWindowResized& ev)
    {
        if (m_renderMask.IsSet(RM_SwapchainOwner) && ev.window == m_swapchain->_windowHandle)
        {
            SimpleAction act;
            act.Action = [ev, this]() {
                m_recreateSwapchain = true;
                m_newSwapchainSize  = ev.newSize;
            };
            m_syncedActions.push_back(act);
        }
        else if (!m_renderMask.IsSet(RM_SwapchainOwner) && ev.window == Backend::Get()->GetMainSwapchain()._windowHandle)
        {
            SimpleAction act;
            act.Action = [ev, this]() { UpdateViewport(ev.newSize); };
            m_syncedActions.push_back(act);
        }
    }

    void Renderer::AssignSwapchain(Swapchain* swapchain)
    {
        m_swapchain = swapchain;
        UpdateViewport(m_swapchain->size);
    }

    void Renderer::SetWorld(World::EntityWorld* world)
    {
        m_worldToSet = world;
    }

    void Renderer::RemoveWorld()
    {
        m_shouldRemoveWorld = true;
    }

    void Renderer::SetWorldImpl()
    {
        LINA_ASSERT(m_targetWorldData == nullptr, "World already exists!");

        m_targetWorldData        = new RenderWorldData();
        m_targetWorldData->world = m_worldToSet;

        for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            m_targetWorldData->indirectBuffer[i] = Buffer{.size        = OBJ_BUFFER_MAX * sizeof(VkDrawIndexedIndirectCommand),
                                                          .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::TransferDst) | GetBufferUsageFlags(BufferUsageFlags::StorageBuffer) | GetBufferUsageFlags(BufferUsageFlags::IndirectBuffer),
                                                          .memoryUsage = MemoryUsageFlags::CpuToGpu};

            m_targetWorldData->indirectBuffer[i].Create();

            // Scene data
            m_targetWorldData->sceneDataBuffer[i] = Buffer{
                .size        = sizeof(GPUSceneData),
                .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::UniformBuffer),
                .memoryUsage = MemoryUsageFlags::CpuToGpu,
            };
            m_targetWorldData->sceneDataBuffer[i].Create();

            // Light data
            m_targetWorldData->lightDataBuffer[i] = Buffer{
                .size        = sizeof(GPULightData),
                .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::UniformBuffer),
                .memoryUsage = MemoryUsageFlags::CpuToGpu,
            };
            m_targetWorldData->lightDataBuffer[i].Create();

            // View data
            m_targetWorldData->viewDataBuffer[i] = Buffer{
                .size        = sizeof(GPUViewData),
                .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::UniformBuffer),
                .memoryUsage = MemoryUsageFlags::CpuToGpu,
            };
            m_targetWorldData->viewDataBuffer[i].Create();

            // Obj data
            m_targetWorldData->objDataBuffer[i] = Buffer{
                .size        = sizeof(GPUObjectData) * OBJ_BUFFER_MAX,
                .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::StorageBuffer),
                .memoryUsage = MemoryUsageFlags::CpuToGpu,
            };
            m_targetWorldData->objDataBuffer[i].Create();
            m_targetWorldData->objDataBuffer[i].boundSet     = &m_targetWorldData->passDescriptor;
            m_targetWorldData->objDataBuffer[i].boundBinding = 1;
            m_targetWorldData->objDataBuffer[i].boundType    = DescriptorType::StorageBuffer;

            // Pass descriptor
            m_targetWorldData->passDescriptor = DescriptorSet{
                .setCount = 1,
                .pool     = m_descriptorPool._ptr,
            };

            m_targetWorldData->passDescriptor.Create(RenderEngine::Get()->GetLayout(DescriptorSetType::PassSet));
            m_targetWorldData->passDescriptor.BeginUpdate();
            m_targetWorldData->passDescriptor.AddBufferUpdate(m_targetWorldData->sceneDataBuffer[i], m_targetWorldData->sceneDataBuffer[i].size, 0, DescriptorType::UniformBuffer);
            m_targetWorldData->passDescriptor.AddBufferUpdate(m_targetWorldData->viewDataBuffer[i], m_targetWorldData->viewDataBuffer[i].size, 1, DescriptorType::UniformBuffer);
            m_targetWorldData->passDescriptor.AddBufferUpdate(m_targetWorldData->lightDataBuffer[i], m_targetWorldData->lightDataBuffer[i].size, 2, DescriptorType::UniformBuffer);
            m_targetWorldData->passDescriptor.AddBufferUpdate(m_targetWorldData->objDataBuffer[i], m_targetWorldData->objDataBuffer[i].size, 3, DescriptorType::StorageBuffer);
            m_targetWorldData->passDescriptor.SendUpdate();
        }

        m_targetWorldData->opaquePass.Initialize(DrawPassMask::Opaque, 1000.0f);
        m_targetWorldData->allRenderables.Initialize(250, nullptr);
        m_targetWorldData->allRenderables.Initialize(250, nullptr);
        m_targetWorldData->finalColorTexture = VulkanUtility::CreateDefaultPassTextureColor();
        m_targetWorldData->finalDepthTexture = VulkanUtility::CreateDefaultPassTextureDepth();
        const String   sidStr                = "World: " + TO_STRING(m_targetWorldData->world->GetID()) + TO_STRING(s_worldCounter++);
        const StringID sid                   = TO_SID(sidStr);
        m_targetWorldData->finalColorTexture->SetUserManaged(true);
        m_targetWorldData->finalColorTexture->ChangeSID(sid);
        Resources::ResourceManager::Get()->GetCache<Graphics::Texture>()->AddResource(sid, m_targetWorldData->finalColorTexture);
        m_targetWorldData->initialized = true;

        AddRenderables<Graphics::ModelNodeComponent>(m_targetWorldData->world, m_targetWorldData);

        for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            if (m_worldMaterials[i] != nullptr)
            {
                m_worldMaterials[i]->SetTexture(0, m_targetWorldData->finalColorTexture);
                m_worldMaterials[i]->CheckUpdatePropertyBuffers();
            }
        }
    }

    void Renderer::RemoveWorldImpl()
    {
        LINA_ASSERT(m_targetWorldData != nullptr, "World doesn't exist!");
        Resources::ResourceManager::Get()->UnloadUserManaged(m_targetWorldData->finalColorTexture);
        delete m_targetWorldData->finalDepthTexture;
        m_targetWorldData->allRenderables.Reset();
        m_targetWorldData->extractedRenderables.clear();

        for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            m_targetWorldData->objDataBuffer[i].Destroy();
            m_targetWorldData->lightDataBuffer[i].Destroy();
            m_targetWorldData->viewDataBuffer[i].Destroy();
            m_targetWorldData->indirectBuffer[i].Destroy();
            m_targetWorldData->sceneDataBuffer[i].Destroy();
        }

        delete m_targetWorldData;
    }

    void Renderer::UpdateViewport(const Vector2i& size)
    {
        m_viewport.width  = static_cast<float>(size.x);
        m_viewport.height = static_cast<float>(size.y);
        m_scissors.size   = size;
    }

} // namespace Lina::Graphics
