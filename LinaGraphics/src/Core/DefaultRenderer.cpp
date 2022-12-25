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

#include "Core/DefaultRenderer.hpp"
#include "EventSystem/EventSystem.hpp"
#include "EventSystem/LevelEvents.hpp"
#include "EventSystem/ComponentEvents.hpp"
#include "EventSystem/WindowEvents.hpp"
#include "EventSystem/MainLoopEvents.hpp"
#include "PipelineObjects/Swapchain.hpp"
#include "PipelineObjects/CommandPool.hpp"
#include "Utility/Vulkan/VulkanUtility.hpp"
#include "Data/Vertex.hpp"
#include "Profiling/Profiler.hpp"
#include "Core/RenderEngine.hpp"
#include "Core/Entity.hpp"
#include "Components/RenderableComponent.hpp"
#include "Resource/Material.hpp"
#include "Core/Backend.hpp"
#include "Resource/Model.hpp"
#include "Resource/ModelNode.hpp"

namespace Lina::Graphics
{

#define DEF_VTXBUF_SIZE   sizeof(Vertex) * 2000
#define DEF_INDEXBUF_SIZE sizeof(uint32) * 2000

    void DefaultRenderer::Initialize()
    {
        Renderer::Initialize();

        Event::EventSystem::Get()->Connect<Event::ELevelUninstalled, &DefaultRenderer::OnLevelUninstalled>(this);
        Event::EventSystem::Get()->Connect<Event::ELevelInstalled, &DefaultRenderer::OnLevelInstalled>(this);
        Event::EventSystem::Get()->Connect<Event::EComponentCreated, &DefaultRenderer::OnComponentCreated>(this);
        Event::EventSystem::Get()->Connect<Event::EComponentDestroyed, &DefaultRenderer::OnComponentDestroyed>(this);
        Event::EventSystem::Get()->Connect<Event::EPreMainLoop, &DefaultRenderer::OnPreMainLoop>(this);
        Event::EventSystem::Get()->Connect<Event::EWindowResized, &DefaultRenderer::OnWindowResized>(this);
        Event::EventSystem::Get()->Connect<Event::EWindowPositioned, &DefaultRenderer::OnWindowPositioned>(this);

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

        // Draw passes.
        m_opaquePass.Initialize(DrawPassMask::Opaque, 1000.0f);

        // IDlists
        m_allRenderables.Initialize(250, nullptr);
    }

    void DefaultRenderer::Shutdown()
    {
        Renderer::Shutdown();

        Event::EventSystem::Get()->Disconnect<Event::ELevelInstalled>(this);
        Event::EventSystem::Get()->Disconnect<Event::ELevelUninstalled>(this);
        Event::EventSystem::Get()->Disconnect<Event::EComponentCreated>(this);
        Event::EventSystem::Get()->Disconnect<Event::EComponentDestroyed>(this);
        Event::EventSystem::Get()->Disconnect<Event::EPreMainLoop>(this);
        Event::EventSystem::Get()->Disconnect<Event::EWindowResized>(this);
        Event::EventSystem::Get()->Disconnect<Event::EWindowPositioned>(this);


        m_gpuIndexBuffer.Destroy();
        m_gpuVtxBuffer.Destroy();
        m_cpuIndexBuffer.Destroy();
        m_cpuVtxBuffer.Destroy();

        // for (int i = 0; i < m_framebuffers.size(); i++)
        //     m_framebuffers[i].Destroy();
    }

    void DefaultRenderer::SyncData()
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

        m_opaquePass.PrepareRenderData(m_extractedRenderables, RenderEngine::Get()->GetPlayerView());
    }

    void DefaultRenderer::RecordCommandBuffer(Frame& frame)
    {
        auto& cmd = m_cmds[m_currentImageIndex];

        auto        vp                = RenderEngine::Get()->GetViewport();
        auto        scissor           = RenderEngine::Get()->GetScissors();
        const Recti defaultRenderArea = Recti(Vector2(vp.x, vp.y), Vector2(vp.width, vp.height));
        cmd.CMD_SetViewport(vp);
        cmd.CMD_SetScissors(scissor);

        // Merged object buffer.
        uint64 offset = 0;
        cmd.CMD_BindVertexBuffers(0, 1, m_gpuVtxBuffer._ptr, &offset);
        cmd.CMD_BindIndexBuffers(m_gpuIndexBuffer._ptr, 0, IndexType::Uint32);

        // Global set.
        cmd.CMD_BindDescriptorSets(PipelineBindPoint::Graphics, RenderEngine::Get()->GetGlobalAndPassLayouts()._ptr, 0, 1, &frame.globalDescriptor, 0, nullptr);

        // Pass set.
        cmd.CMD_BindDescriptorSets(PipelineBindPoint::Graphics, RenderEngine::Get()->GetGlobalAndPassLayouts()._ptr, 1, 1, &frame.passDescriptor, 0, nullptr);

        // Global - scene data.
        frame.sceneDataBuffer.CopyInto(&m_sceneData, sizeof(GPUSceneData));

        // Global - light data.
        frame.lightDataBuffer.CopyInto(&m_lightData, sizeof(GPULightData));

        // Per render pass - obj data.
        Vector<GPUObjectData> gpuObjectData;

        for (auto& r : m_extractedRenderables)
        {
            // Object data.
            GPUObjectData objData;
            objData.modelMatrix = r.modelMatrix;
            gpuObjectData.push_back(objData);
        }

        frame.objDataBuffer.CopyInto(gpuObjectData.data(), sizeof(GPUObjectData) * gpuObjectData.size());

        auto& mainPass  = RenderEngine::Get()->GetRenderPass(RenderPassType::Main);
        auto& postPass  = RenderEngine::Get()->GetRenderPass(RenderPassType::PostProcess);
        auto& finalPass = RenderEngine::Get()->GetRenderPass(RenderPassType::Final);

        // ****** MAIN PASS ******
        PROFILER_SCOPE_START("Main Pass", PROFILER_THREAD_RENDER);
        mainPass.Begin(mainPass.framebuffers[m_currentImageIndex], cmd, defaultRenderArea);
        m_opaquePass.UpdateViewData(frame.viewDataBuffer, RenderEngine::Get()->GetPlayerView());
        m_opaquePass.RecordDrawCommands(cmd, RenderPassType::Main, m_meshEntries, frame.indirectBuffer);
        mainPass.End(cmd);
        PROFILER_SCOPE_END("Main Pass", PROFILER_THREAD_RENDER);


        // ****** POST PROCESS PASS ******
        PROFILER_SCOPE_START("PP Pass", PROFILER_THREAD_RENDER);
        postPass.Begin(postPass.framebuffers[m_currentImageIndex], cmd, defaultRenderArea);
        auto* ppMat = RenderEngine::Get()->GetEngineMaterial(EngineShaderType::SQPostProcess);
        ppMat->Bind(cmd, RenderPassType::PostProcess, MaterialBindFlag::BindPipeline | MaterialBindFlag::BindDescriptor);
        cmd.CMD_Draw(3, 1, 0, 0);
        postPass.End(cmd);
        PROFILER_SCOPE_END("PP Pass", PROFILER_THREAD_RENDER);

        // ****** FINAL PASS ******
        PROFILER_SCOPE_START("Final Pass", PROFILER_THREAD_RENDER);
        finalPass.Begin(finalPass.framebuffers[m_currentImageIndex], cmd, defaultRenderArea);
        auto* finalQuadMat = RenderEngine::Get()->GetEngineMaterial(EngineShaderType::SQFinal);
        finalQuadMat->Bind(cmd, RenderPassType::Final, MaterialBindFlag::BindPipeline | MaterialBindFlag::BindDescriptor);
        cmd.CMD_Draw(3, 1, 0, 0);
        finalPass.End(cmd);
        PROFILER_SCOPE_END("Final Pass", PROFILER_THREAD_RENDER);
    }

    void DefaultRenderer::OnLevelUninstalled(const Event::ELevelUninstalled& ev)
    {
        m_allRenderables.Reset();
    }

    void DefaultRenderer::OnLevelInstalled(const Event::ELevelInstalled& ev)
    {
        MergeMeshes();
    }

    void DefaultRenderer::OnComponentCreated(const Event::EComponentCreated& ev)
    {
        if (ev.ptr->GetComponentMask().IsSet(World::ComponentMask::Renderable))
        {
            auto renderable            = static_cast<RenderableComponent*>(ev.ptr);
            renderable->m_renderableID = m_allRenderables.AddItem(renderable);
        }
    }

    void DefaultRenderer::OnComponentDestroyed(const Event::EComponentDestroyed& ev)
    {
        if (ev.ptr->GetComponentMask().IsSet(World::ComponentMask::Renderable))
            m_allRenderables.RemoveItem(static_cast<RenderableComponent*>(ev.ptr)->GetRenderableID());
    }

    void DefaultRenderer::OnPreMainLoop(const Event::EPreMainLoop& ev)
    {
        auto* finalQuadMat = RenderEngine::Get()->GetEngineMaterial(EngineShaderType::SQFinal);
        auto* ppMat        = RenderEngine::Get()->GetEngineMaterial(EngineShaderType::SQPostProcess);
        auto& mainPass     = RenderEngine::Get()->GetRenderPass(RenderPassType::Main);
        auto& ppPass       = RenderEngine::Get()->GetRenderPass(RenderPassType::PostProcess);
        finalQuadMat->SetTexture(0, ppPass._colorTexture);
        finalQuadMat->CheckUpdatePropertyBuffers();
        ppMat->SetTexture(0, mainPass._colorTexture);
        ppMat->CheckUpdatePropertyBuffers();
    }

    void DefaultRenderer::OnWindowResized(const Event::EWindowResized& newSize)
    {
        m_recreateSwapchain = true;
    }

    void DefaultRenderer::OnWindowPositioned(const Event::EWindowPositioned& newPos)
    {
        m_recreateSwapchain = true;
    }

    void DefaultRenderer::HandleOutOfDateImage()
    {
       // Backend::Get()->WaitIdle();
       // Vector2i size = Window::Get()->GetSize();
       //
       // if (size.x == 0 || size.y == 0)
       //     return;
       //
       // auto& mainPass  = m_renderPasses[RenderPassType::Main];
       // auto& ppPass    = m_renderPasses[RenderPassType::PostProcess];
       // auto& finalPass = m_renderPasses[RenderPassType::Final];
       //
       // // Only final render pass depends on swapchain, destroy it.
       // finalPass.Destroy();
       //
       // // Swapchain
       // auto& swapchain = Backend::Get()->GetMainSwapchain();
       // swapchain.Destroy();
       // swapchain.size = size;
       // swapchain.Create();
       //
       // // Make sure we always match swapchain
       // size = swapchain.size;
       //
       // mainPass.Destroy();
       // ppPass.Destroy();
       //
       // // Recreate passes
       // mainPass  = RenderPass();
       // ppPass    = RenderPass();
       // finalPass = RenderPass();
       // VulkanUtility::SetupAndCreateMainRenderPass(mainPass);
       // VulkanUtility::SetupAndCreateMainRenderPass(ppPass);
       // VulkanUtility::SetupAndCreateFinalRenderPass(finalPass);
       //
       // // Re-assign target textures to render passes
       // OnPreMainLoop({});
       //
       // m_recreateSwapchain = false;
       // m_handledOutOfDateImage(m_swapchain);
    }

    void DefaultRenderer::MergeMeshes()
    {
        // Get all the meshes currently loaded in the resource manager
        // Meaning all used meshes for this level.
        // Merge them into big vtx & indx buffers.
        auto  rm    = Resources::ResourceManager::Get();
        auto  cache = rm->GetCache<Graphics::Model>();
        auto& res   = cache->GetResources();
        m_meshEntries.clear();

        Vector<Vertex> mergedVertices;
        Vector<uint32> mergedIndices;

        uint32 vertexOffset = 0;
        uint32 firstIndex   = 0;

        for (auto& pair : res)
        {
            for (auto& node : pair.second->GetNodes())
            {
                for (auto& m : node->GetMeshes())
                {
                    const auto& vertices = m->GetVertices();
                    const auto& indices  = m->GetIndices();

                    MergedBufferMeshEntry entry;
                    entry.vertexOffset = vertexOffset;
                    entry.firstIndex   = firstIndex;
                    entry.indexSize    = indices.size();

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

} // namespace Lina::Graphics
