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
#include "EventSystem/LevelEvents.hpp"
#include "EventSystem/ResourceEvents.hpp"
#include "Graphics/Utility/Vulkan/VulkanUtility.hpp"
#include "Graphics/Components/RenderableComponent.hpp"
#include "Graphics/Resource/Texture.hpp"
#include "World/Core/World.hpp"
#include "Graphics/Components/ModelNodeComponent.hpp"

#define LINAVG_TEXT_SUPPORT
#include "LinaVG/LinaVG.hpp"

namespace Lina::Graphics
{

#define DEF_VTXBUF_SIZE   sizeof(Vertex) * 2000
#define DEF_INDEXBUF_SIZE sizeof(uint32) * 2000

    template <typename T> void AddRenderable(World::EntityWorld* world, Renderer::RenderWorldData& data)
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

    void Renderer::Initialize(Swapchain* swp, GUIBackend* guiBackend, WindowManager* windowManager)
    {
        m_windowManager = windowManager;
        m_swapchain     = swp;
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
        m_descriptorPool.AddPoolSize(DescriptorType::UniformBuffer, 10)
            .AddPoolSize(DescriptorType::UniformBufferDynamic, 10)
            .AddPoolSize(DescriptorType::StorageBuffer, 10)
            .AddPoolSize(DescriptorType::CombinedImageSampler, 10)
            .Create();

        for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            Frame& f = m_frames[i];

            // Sync
            f.presentSemaphore.Create();
            f.submitSemaphore.Create();

            f.graphicsFence = Fence{.flags = GetFenceFlags(FenceFlags::Signaled)};
            f.graphicsFence.Create();
        }

        // Merged buffers.
        m_cpuVtxBuffer = Buffer{
            .size        = DEF_VTXBUF_SIZE,
            .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::TransferSrc),
            .memoryUsage = MemoryUsageFlags::CpuOnly,
        };

        m_cpuIndexBuffer = Buffer{
            .size        = DEF_INDEXBUF_SIZE,
            .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::TransferSrc),
            .memoryUsage = MemoryUsageFlags::CpuOnly,
        };

        m_gpuVtxBuffer = Buffer{
            .size        = DEF_VTXBUF_SIZE,
            .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::VertexBuffer) | GetBufferUsageFlags(BufferUsageFlags::TransferDst),
            .memoryUsage = MemoryUsageFlags::GpuOnly,
        };

        m_gpuIndexBuffer = Buffer{
            .size        = DEF_INDEXBUF_SIZE,
            .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::IndexBuffer) | GetBufferUsageFlags(BufferUsageFlags::TransferDst),
            .memoryUsage = MemoryUsageFlags::GpuOnly,
        };

        m_cpuVtxBuffer.Create();
        m_cpuIndexBuffer.Create();
        m_gpuVtxBuffer.Create();
        m_gpuIndexBuffer.Create();

        ConnectEvents();
    }

    void Renderer::Shutdown()
    {
        DisconnectEvents();

        Vector<World::EntityWorld*> worlds;
        for (auto& p : m_worldsToRender)
            worlds.push_back(p.first);

        for (auto& w : worlds)
            RemoveWorldToRender(w);

        m_gpuIndexBuffer.Destroy();
        m_gpuVtxBuffer.Destroy();
        m_cpuIndexBuffer.Destroy();
        m_cpuVtxBuffer.Destroy();
    }

    void Renderer::Join()
    {
        for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
            m_frames[i].graphicsFence.Wait();
    }

    void Renderer::RenderWorld(const CommandBuffer& cmd, RenderWorldData& data)
    {
        const uint32 frameIndex = GetFrameIndex();
        data.opaquePass.UpdateViewData(data.viewDataBuffer[frameIndex], data.playerView);
        data.opaquePass.RecordDrawCommands(cmd, m_meshEntries, data.indirectBuffer[frameIndex]);
    }

    void Renderer::SyncData()
    {
        PROFILER_FUNC(PROFILER_THREAD_MAIN);

        m_lastMainSwapchainSize = m_windowManager->GetMainWindow().GetSize();
        m_worldsToRenderGPU.clear();
        m_worldsToRenderGPU = m_worldsToRender;

        for (auto& [world, wd] : m_worldsToRenderGPU)
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

    void Renderer::MergeMeshes()
    {
        // Get all the meshes currently loaded in the resource manager
        // Meaning all used meshes for this level.
        // Merge them into big vtx & indx buffers.
        auto  rm    = Resources::ResourceManager::Get();
        auto  cache = rm->GetCache<Graphics::Model>();
        auto& res   = cache->GetResources();
        m_meshEntries.clear();
        m_mergedModelIDs.clear();

        Vector<Vertex> mergedVertices;
        Vector<uint32> mergedIndices;

        uint32 vertexOffset = 0;
        uint32 firstIndex   = 0;

        for (auto& pair : res)
        {
            m_mergedModelIDs.push_back(pair.second->GetSID());

            for (auto& node : pair.second->GetNodes())
            {
                for (auto& m : node->GetMeshes())
                {
                    const auto& vertices = m->GetVertices();
                    const auto& indices  = m->GetIndices();

                    MergedBufferMeshEntry entry;
                    entry.vertexOffset = vertexOffset;
                    entry.firstIndex   = firstIndex;
                    entry.indexSize    = static_cast<uint32>(indices.size());

                    const uint32 vtxSize   = static_cast<uint32>(vertices.size());
                    const uint32 indexSize = static_cast<uint32>(indices.size());
                    for (auto& v : vertices)
                        mergedVertices.push_back(v);

                    for (auto& i : indices)
                        mergedIndices.push_back(i);

                    vertexOffset += vtxSize;
                    firstIndex += indexSize;

                    m_meshEntries[m] = entry;
                }
            }
        }

        const uint32 vtxSize   = static_cast<uint32>(mergedVertices.size() * sizeof(Vertex));
        const uint32 indexSize = static_cast<uint32>(mergedIndices.size() * sizeof(uint32));

        m_cpuVtxBuffer.CopyInto(mergedVertices.data(), vtxSize);
        m_cpuIndexBuffer.CopyInto(mergedIndices.data(), indexSize);

        // Realloc if necessary.
        if (m_gpuVtxBuffer.size < m_cpuVtxBuffer.size)
        {
            m_gpuVtxBuffer.Destroy();
            m_gpuVtxBuffer.size = m_cpuVtxBuffer.size;
            m_gpuVtxBuffer.Create();
        }

        if (m_gpuIndexBuffer.size < m_cpuIndexBuffer.size)
        {
            m_gpuIndexBuffer.Destroy();
            m_gpuIndexBuffer.size = m_cpuIndexBuffer.size;
            m_gpuIndexBuffer.Create();
        }

        Command vtxCmd;
        vtxCmd.Record = [this, vtxSize](CommandBuffer& cmd) {
            BufferCopy copy = BufferCopy{
                .destinationOffset = 0,
                .sourceOffset      = 0,
                .size              = vtxSize,
            };

            Vector<BufferCopy> regions;
            regions.push_back(copy);

            cmd.CMD_CopyBuffer(m_cpuVtxBuffer._ptr, m_gpuVtxBuffer._ptr, regions);
        };

        vtxCmd.OnSubmitted = [this]() {
            m_gpuVtxBuffer._ready = true;
            // f.cpuVtxBuffer.Destroy();
        };

        RenderEngine::Get()->GetGPUUploader().SubmitImmediate(vtxCmd);

        Command indexCmd;
        indexCmd.Record = [this, indexSize](CommandBuffer& cmd) {
            BufferCopy copy = BufferCopy{
                .destinationOffset = 0,
                .sourceOffset      = 0,
                .size              = indexSize,
            };

            Vector<BufferCopy> regions;
            regions.push_back(copy);

            cmd.CMD_CopyBuffer(m_cpuIndexBuffer._ptr, m_gpuIndexBuffer._ptr, regions);
        };

        indexCmd.OnSubmitted = [this] {
            m_gpuIndexBuffer._ready = true;
            // f.cpuIndexBuffer.Destroy();
        };

        RenderEngine::Get()->GetGPUUploader().SubmitImmediate(indexCmd);
    }

    bool Renderer::HandleOutOfDateImage(VulkanResult res)
    {
        bool shouldRecreate = false;

        if (res == VulkanResult::OutOfDateKHR || res == VulkanResult::SuboptimalKHR)
            shouldRecreate      = true;
        else if (res != VulkanResult::Success)
            LINA_ASSERT(false, "Could not acquire next image!");

        if (shouldRecreate)
        {
            Backend::Get()->WaitIdle();

            Vector2i size = m_lastMainSwapchainSize;

            if (size.x == 0 || size.y == 0)
            {
                // Make sure the semaphore is unsignalled after resize operation.
                //Backend::Get()->GetGraphicsQueue().Submit(m_frames[GetFrameIndex()].submitSemaphore);
                return true;
            }

            // Swapchain
            m_swapchain->Destroy();
            m_swapchain->size = size;
            m_swapchain->Create(0);

            // Make sure we always match swapchain
            size = m_swapchain->size;

            // Delete the textures for every worls we render.
            for (auto& [world, wd] : m_worldsToRender)
            {
                Resources::ResourceManager::Get()->UnloadUserManaged(wd.finalColorTexture);
                delete wd.finalDepthTexture;
                wd.finalColorTexture  = VulkanUtility::CreateDefaultPassTextureColor();
                wd.finalDepthTexture  = VulkanUtility::CreateDefaultPassTextureDepth();
                const String   sidStr = "World: " + TO_STRING(world->GetID()) + TO_STRING(m_worldCounter++);
                const StringID sid    = TO_SID(sidStr);
                wd.finalColorTexture->SetUserManaged(true);
                wd.finalColorTexture->ChangeSID(sid);
                Resources::ResourceManager::Get()->GetCache<Graphics::Texture>()->AddResource(sid, wd.finalColorTexture);
            }

            // Re-assign target textures to render passes
            OnTexturesRecreated();
            UpdateViewport(size);

            // Make sure the semaphore is unsignalled after resize operation.
            //Backend::Get()->GetGraphicsQueue().Submit(m_frames[GetFrameIndex()].submitSemaphore);

            return true;
        }

        return false;
    }

    void Renderer::ConnectEvents()
    {
        Event::EventSystem::Get()->Connect<Event::ELevelUninstalled, &Renderer::OnLevelUninstalled>(this);
        Event::EventSystem::Get()->Connect<Event::ELevelInstalled, &Renderer::OnLevelInstalled>(this);
        Event::EventSystem::Get()->Connect<Event::EResourceLoaded, &Renderer::OnResourceLoaded>(this);
        Event::EventSystem::Get()->Connect<Event::EComponentCreated, &Renderer::OnComponentCreated>(this);
        Event::EventSystem::Get()->Connect<Event::EComponentDestroyed, &Renderer::OnComponentDestroyed>(this);
    }

    void Renderer::DisconnectEvents()
    {
        Event::EventSystem::Get()->Disconnect<Event::EComponentCreated>(this);
        Event::EventSystem::Get()->Disconnect<Event::EComponentDestroyed>(this);
        Event::EventSystem::Get()->Disconnect<Event::ELevelInstalled>(this);
        Event::EventSystem::Get()->Disconnect<Event::ELevelUninstalled>(this);
        Event::EventSystem::Get()->Disconnect<Event::EResourceLoaded>(this);
    }

    void Renderer::OnLevelUninstalled(const Event::ELevelUninstalled& ev)
    {
        Join();
        RemoveWorldToRender(m_installedWorld);
        m_installedWorld = nullptr;
        m_meshEntries.clear();
        m_mergedModelIDs.clear();
        m_hasLevelLoaded = false;
    }

    void Renderer::OnLevelInstalled(const Event::ELevelInstalled& ev)
    {
        Join();
        m_installedWorld = ev.world;
        AddWorldToRender(m_installedWorld);
        OnTexturesRecreated();
        m_hasLevelLoaded = true;
        MergeMeshes();
    }

    void Renderer::OnResourceLoaded(const Event::EResourceLoaded& res)
    {
        // When a new model is loaded re-merge meshes
        // Do this only after a level is fully loaded, not during loading level resources.
        if (!m_hasLevelLoaded)
            return;

        if (res.tid == GetTypeID<Model>())
        {
            auto* found = linatl::find_if(m_mergedModelIDs.begin(), m_mergedModelIDs.end(), [&res](StringID sid) { return sid == res.sid; });
            if (found == m_mergedModelIDs.end())
            {
                Join();
                MergeMeshes();
            }
        }
    }

    void Renderer::OnComponentCreated(const Event::EComponentCreated& ev)
    {
        if (ev.ptr->GetComponentMask().IsSet(World::ComponentMask::Renderable))
        {
            auto renderable = static_cast<RenderableComponent*>(ev.ptr);

            if (m_worldsToRender.find(ev.world) == m_worldsToRender.end())
                return;

            auto& wd                   = m_worldsToRender[ev.world];
            renderable->m_renderableID = wd.allRenderables.AddItem(renderable);
        }
    }

    void Renderer::OnComponentDestroyed(const Event::EComponentDestroyed& ev)
    {
        if (ev.ptr->GetComponentMask().IsSet(World::ComponentMask::Renderable))
        {
            if (m_worldsToRender.find(ev.world) == m_worldsToRender.end())
                return;

            auto& wd = m_worldsToRender[ev.world];
            wd.allRenderables.RemoveItem(static_cast<RenderableComponent*>(ev.ptr)->GetRenderableID());
        }
    }

    void Renderer::AddWorldToRender(World::EntityWorld* world)
    {
        LINA_ASSERT(m_worldsToRender.find(world) == m_worldsToRender.end(), "World already exists!");

        auto& data = m_worldsToRender[world];

        for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            data.indirectBuffer[i] =
                Buffer{.size        = OBJ_BUFFER_MAX * sizeof(VkDrawIndexedIndirectCommand),
                       .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::TransferDst) | GetBufferUsageFlags(BufferUsageFlags::StorageBuffer) | GetBufferUsageFlags(BufferUsageFlags::IndirectBuffer),
                       .memoryUsage = MemoryUsageFlags::CpuToGpu};

            data.indirectBuffer[i].Create();

            // Scene data
            data.sceneDataBuffer[i] = Buffer{
                .size        = sizeof(GPUSceneData),
                .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::UniformBuffer),
                .memoryUsage = MemoryUsageFlags::CpuToGpu,
            };
            data.sceneDataBuffer[i].Create();

            // Light data
            data.lightDataBuffer[i] = Buffer{
                .size        = sizeof(GPULightData),
                .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::UniformBuffer),
                .memoryUsage = MemoryUsageFlags::CpuToGpu,
            };
            data.lightDataBuffer[i].Create();

            // View data
            data.viewDataBuffer[i] = Buffer{
                .size        = sizeof(GPUViewData),
                .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::UniformBuffer),
                .memoryUsage = MemoryUsageFlags::CpuToGpu,
            };
            data.viewDataBuffer[i].Create();

            // Obj data
            data.objDataBuffer[i] = Buffer{
                .size        = sizeof(GPUObjectData) * OBJ_BUFFER_MAX,
                .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::StorageBuffer),
                .memoryUsage = MemoryUsageFlags::CpuToGpu,
            };
            data.objDataBuffer[i].Create();
            data.objDataBuffer[i].boundSet     = &data.passDescriptor;
            data.objDataBuffer[i].boundBinding = 1;
            data.objDataBuffer[i].boundType    = DescriptorType::StorageBuffer;

            // Pass descriptor
            data.passDescriptor = DescriptorSet{
                .setCount = 1,
                .pool     = m_descriptorPool._ptr,
            };

            data.passDescriptor.Create(RenderEngine::Get()->GetLayout(DescriptorSetType::PassSet));
            data.passDescriptor.BeginUpdate();
            data.passDescriptor.AddBufferUpdate(data.sceneDataBuffer[i], data.sceneDataBuffer[i].size, 0, DescriptorType::UniformBuffer);
            data.passDescriptor.AddBufferUpdate(data.viewDataBuffer[i], data.viewDataBuffer[i].size, 1, DescriptorType::UniformBuffer);
            data.passDescriptor.AddBufferUpdate(data.lightDataBuffer[i], data.lightDataBuffer[i].size, 2, DescriptorType::UniformBuffer);
            data.passDescriptor.AddBufferUpdate(data.objDataBuffer[i], data.objDataBuffer[i].size, 3, DescriptorType::StorageBuffer);
            data.passDescriptor.SendUpdate();
        }

        data.opaquePass.Initialize(DrawPassMask::Opaque, 1000.0f);
        data.allRenderables.Initialize(250, nullptr);
        data.allRenderables.Initialize(250, nullptr);
        data.finalColorTexture = VulkanUtility::CreateDefaultPassTextureColor();
        data.finalDepthTexture = VulkanUtility::CreateDefaultPassTextureDepth();
        const String   sidStr  = "World: " + TO_STRING(world->GetID()) + TO_STRING(m_worldCounter++);
        const StringID sid     = TO_SID(sidStr);
        data.finalColorTexture->SetUserManaged(true);
        data.finalColorTexture->ChangeSID(sid);
        Resources::ResourceManager::Get()->GetCache<Graphics::Texture>()->AddResource(sid, data.finalColorTexture);
        data.initialized = true;

        AddRenderable<Graphics::ModelNodeComponent>(world, m_worldsToRender[world]);
    }

    void Renderer::RemoveWorldToRender(World::EntityWorld* world)
    {
        auto it = m_worldsToRender.find(world);
        LINA_ASSERT(it != m_worldsToRender.end(), "World doesn't exist!");

        Join();

        auto& data = it->second;
        Resources::ResourceManager::Get()->UnloadUserManaged(data.finalColorTexture);
        delete data.finalDepthTexture;
        data.allRenderables.Reset();
        data.extractedRenderables.clear();

        for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            data.objDataBuffer[i].Destroy();
            data.lightDataBuffer[i].Destroy();
            data.viewDataBuffer[i].Destroy();
            data.indirectBuffer[i].Destroy();
            data.sceneDataBuffer[i].Destroy();
        }

        m_worldsToRender.erase(it);
    }

    void Renderer::UpdateViewport(const Vector2i& size)
    {
        m_viewport.width             = static_cast<float>(size.x);
        m_viewport.height            = static_cast<float>(size.y);
        m_scissors.size              = size;
        LinaVG::Config.displayWidth  = static_cast<unsigned int>(m_viewport.width);
        LinaVG::Config.displayHeight = static_cast<unsigned int>(m_viewport.height);
    }

} // namespace Lina::Graphics
