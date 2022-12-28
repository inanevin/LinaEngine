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

#include "Core/EditorRenderer.hpp"
#include "Profiling/Profiler.hpp"
#include "Graphics/Core/GUIBackend.hpp"
#include "Graphics/Core/Backend.hpp"
#include "Graphics/Core/RenderEngine.hpp"
#include "Graphics/Resource/Material.hpp"
#include "EventSystem/GraphicsEvents.hpp"
#include "World/Core/Entity.hpp"
#include "World/Core/Component.hpp"
#include "EventSystem/ComponentEvents.hpp"
#include "Graphics/Components/RenderableComponent.hpp"
#include "World/Core/World.hpp"
#include "EventSystem/LevelEvents.hpp"

using namespace Lina::Graphics;
namespace Lina::Editor
{
    void EditorRenderer::CreateRenderPasses()
    {
        GameRenderer::CreateRenderPasses();
    }

    void EditorRenderer::Tick()
    {
        for (auto& pair : m_worldsToRender)
        {
            m_cameraSystem.SetActiveCamera(pair.second.cameraComponent);
            m_cameraSystem.Tick();
            pair.second.playerView.Tick(m_cameraSystem.GetPos(), m_cameraSystem.GetView(), m_cameraSystem.GetProj());
        }
    }

    void EditorRenderer::Render()
    {
        PROFILER_FUNC(PROFILER_THREAD_RENDER);

        const uint32 frameIndex = GetFrameIndex();
        auto&        frame      = m_frames[frameIndex];
        m_guiBackend->SetFrameIndex(frameIndex);

        frame.graphicsFence.Wait(true, 1.0f);

        VulkanResult res;
        const uint32 imageIndex = Backend::Get()->GetMainSwapchain().AcquireNextImage(1.0, frame.submitSemaphore, res);

        if (m_recreateSwapchain || res == VulkanResult::OutOfDateKHR || res == VulkanResult::SuboptimalKHR)
        {
            if (!m_stopped.load())
                HandleOutOfDateImage();
            return;
        }
        else if (res != VulkanResult::Success)
        {
            LINA_ASSERT(false, "Could not acquire next image!");
            return;
        }

        frame.graphicsFence.Reset();

        auto& cmd = m_cmds[imageIndex];
        cmd.Reset(true);
        cmd.Begin(GetCommandBufferFlags(CommandBufferFlags::OneTimeSubmit));

        const Recti defaultRenderArea = Recti(Vector2(m_viewport.x, m_viewport.y), Vector2(m_viewport.width, m_viewport.height));
        cmd.CMD_SetViewport(m_viewport);
        cmd.CMD_SetScissors(m_scissors);

        // Merged object buffer.
        uint64 offset = 0;
        cmd.CMD_BindVertexBuffers(0, 1, m_gpuVtxBuffer._ptr, &offset);
        cmd.CMD_BindIndexBuffers(m_gpuIndexBuffer._ptr, 0, IndexType::Uint32);

        // Global set.
        cmd.CMD_BindDescriptorSets(PipelineBindPoint::Graphics, RenderEngine::Get()->GetGlobalAndPassLayouts()._ptr, 0, 1, &frame.globalDescriptor, 0, nullptr);

        // Pass set.
        cmd.CMD_BindDescriptorSets(PipelineBindPoint::Graphics, RenderEngine::Get()->GetGlobalAndPassLayouts()._ptr, 1, 1, &frame.passDescriptor, 0, nullptr);

        auto& mainPass  = m_renderPasses[RenderPassType::Main];
        auto& postPass  = m_renderPasses[RenderPassType::PostProcess];
        auto& finalPass = m_renderPasses[RenderPassType::Final];

        // ****** MAIN PASS ******
        PROFILER_SCOPE_START("Main Pass", PROFILER_THREAD_RENDER);

        mainPass.Begin(mainPass.framebuffers[imageIndex], cmd, defaultRenderArea);

        for (auto& pair : m_worldsToRender)
        {
            auto& worldData = pair.second;

            // Global - scene data.
            frame.sceneDataBuffer.CopyInto(&worldData.sceneData, sizeof(GPUSceneData));

            // Global - light data.
            frame.lightDataBuffer.CopyInto(&worldData.lightData, sizeof(GPULightData));

            // Per render pass - obj data.
            Vector<GPUObjectData> gpuObjectData;

            for (auto& r : worldData.extractedRenderables)
            {
                // Object data.
                GPUObjectData objData;
                objData.modelMatrix = r.modelMatrix;
                gpuObjectData.push_back(objData);
            }

            frame.objDataBuffer.CopyInto(gpuObjectData.data(), sizeof(GPUObjectData) * gpuObjectData.size());
            worldData.opaquePass.UpdateViewData(frame.viewDataBuffer, worldData.playerView);
            worldData.opaquePass.RecordDrawCommands(cmd, RenderPassType::Main, m_meshEntries, frame.indirectBuffer);
        }

        mainPass.End(cmd);

        PROFILER_SCOPE_END("Main Pass", PROFILER_THREAD_RENDER);

        // ****** POST PROCESS PASS ******
        PROFILER_SCOPE_START("PP Pass", PROFILER_THREAD_RENDER);
        m_guiBackend->SetCmd(&cmd);
        Event::EventSystem::Get()->Trigger<Event::EDrawGUI>();
        postPass.Begin(postPass.framebuffers[imageIndex], cmd, defaultRenderArea);
        auto* ppMat = RenderEngine::Get()->GetEngineMaterial(EngineShaderType::SQPostProcess);
        ppMat->Bind(cmd, RenderPassType::PostProcess, MaterialBindFlag::BindPipeline | MaterialBindFlag::BindDescriptor);
        cmd.CMD_Draw(3, 1, 0, 0);
        m_guiBackend->RecordDrawCommands();
        postPass.End(cmd);
        PROFILER_SCOPE_END("PP Pass", PROFILER_THREAD_RENDER);

        // ****** FINAL PASS ******
        // PROFILER_SCOPE_START("Final Pass", PROFILER_THREAD_RENDER);
        // m_guiBackend->SetCmd(&cmd);
        // Event::EventSystem::Get()->Trigger<Event::EDrawGUI>();
        // finalPass.Begin(finalPass.framebuffers[imageIndex], cmd, defaultRenderArea);
        // auto* finalQuadMat = RenderEngine::Get()->GetEngineMaterial(EngineShaderType::SQFinal);
        // finalQuadMat->Bind(cmd, RenderPassType::Final, MaterialBindFlag::BindPipeline | MaterialBindFlag::BindDescriptor);
        // cmd.CMD_Draw(3, 1, 0, 0);
        // m_guiBackend->RecordDrawCommands();
        // finalPass.End(cmd);
        // PROFILER_SCOPE_END("Final Pass", PROFILER_THREAD_RENDER);

        cmd.End();

        // Submit command waits on the present semaphore, e.g. it waits for the acquired image to be ready.
        // Then submits command, and signals render semaphore when its submitted.
        PROFILER_SCOPE_START("Queue Submit & Present", PROFILER_THREAD_RENDER);
        Backend::Get()->GetGraphicsQueue().Submit(frame.submitSemaphore, frame.presentSemaphore, frame.graphicsFence, cmd, 1);
        Backend::Get()->GetGraphicsQueue().Present(frame.presentSemaphore, imageIndex);

        // Backend::Get()->GetGraphicsQueue().WaitIdle();
        PROFILER_SCOPE_END("Queue Submit & Present", PROFILER_THREAD_RENDER);

        m_frameNumber++;
    }

    void EditorRenderer::SyncData()
    {
        PROFILER_FUNC(PROFILER_THREAD_MAIN);

        for (auto& pair : m_worldsToRender)
        {
            auto& worldData = pair.second;
            worldData.extractedRenderables.clear();

            const auto& renderables = worldData.allRenderables.GetItems();

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
                worldData.extractedRenderables.push_back(data);
            }

            worldData.opaquePass.PrepareRenderData(worldData.extractedRenderables, worldData.playerView);
        }
    }

    void EditorRenderer::OnComponentCreated(const Event::EComponentCreated& ev)
    {
        if (ev.ptr->GetComponentMask().IsSet(World::ComponentMask::Renderable))
        {
            auto       renderable = static_cast<RenderableComponent*>(ev.ptr);
            const bool exists     = m_worldsToRender.find(ev.world) != m_worldsToRender.end();

            auto& data = m_worldsToRender[ev.world];

            if (!exists)
            {
                data.opaquePass.Initialize(DrawPassMask::Opaque, 1000.0f);
                data.allRenderables.Initialize(250, nullptr);
                data.allRenderables.Initialize(250, nullptr);
            }

            renderable->m_renderableID = m_worldsToRender[ev.world].allRenderables.AddItem(renderable);
        }
    }

    void EditorRenderer::OnComponentDestroyed(const Event::EComponentDestroyed& ev)
    {
        if (ev.ptr->GetComponentMask().IsSet(World::ComponentMask::Renderable))
            m_worldsToRender[ev.world].allRenderables.RemoveItem(static_cast<RenderableComponent*>(ev.ptr)->GetRenderableID());
    }

    void EditorRenderer::OnLevelUninstalled(const Event::ELevelUninstalled& ev)
    {
        Join();
        m_meshEntries.clear();
        m_mergedModelIDs.clear();
        m_hasLevelLoaded = false;

        for (auto& pair : m_worldsToRender)
        {
            if (ev.world == pair.first)
                pair.second.allRenderables.Reset();
        }
    }

} // namespace Lina::Editor
