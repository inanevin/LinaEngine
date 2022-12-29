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
#include "EventSystem/WindowEvents.hpp"
#include "World/Core/Entity.hpp"
#include "World/Core/Component.hpp"
#include "EventSystem/WorldEvents.hpp"
#include "Graphics/Components/RenderableComponent.hpp"
#include "World/Core/World.hpp"
#include "EventSystem/LevelEvents.hpp"
#include "Graphics/Utility/Vulkan/VulkanUtility.hpp"
#include "Graphics/Components/CameraComponent.hpp"

using namespace Lina::Graphics;
namespace Lina::Editor
{
    void EditorRenderer::CreateRenderPasses()
    {
        m_renderPasses[RenderPassType::Main]         = RenderPass();
        m_renderPasses[RenderPassType::Intermediate] = RenderPass();
        m_renderPasses[RenderPassType::Final]        = RenderPass();
        auto& mainPass                               = m_renderPasses[RenderPassType::Main];
        auto& interPass                              = m_renderPasses[RenderPassType::Intermediate];
        auto& finalPass                              = m_renderPasses[RenderPassType::Final];
        VulkanUtility::CreateMainRenderPass(mainPass);
        VulkanUtility::CreateMainRenderPass(interPass);
        VulkanUtility::CreatePresentRenderPass(finalPass);
    }

    void EditorRenderer::Tick()
    {
        for (auto& pair : m_worldDataCPU)
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
        auto& finalPass = m_renderPasses[RenderPassType::Final];

        // ****** MAIN PASS ******
        PROFILER_SCOPE_START("Main Pass", PROFILER_THREAD_RENDER);
            LINA_TRACE("-------------------------------------------");

        for (auto& pair : m_worldDataGPU)
        {
            auto& worldData = pair.second;

            LINA_TRACE("Rendering World With: ");
            LINA_TRACE("Object Count {0}", worldData.extractedRenderables.size());
            auto p = worldData.cameraComponent->GetEntity()->GetPosition();
            LINA_TRACE("Cam Pos {0} {1} {2}", p.x, p.y, p.z);

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

            mainPass.Begin(worldData.framebuffers[imageIndex], cmd, defaultRenderArea);
            worldData.opaquePass.UpdateViewData(frame.viewDataBuffer, worldData.playerView);
            worldData.opaquePass.RecordDrawCommands(cmd, RenderPassType::Main, m_meshEntries, frame.indirectBuffer);
            mainPass.End(cmd);

            // aq = worldData.finalColorTexture;

            //  const Extent3D    ext = mainPass._colorTexture->GetExtent();
            //  Vector<ImageBlit> regions;
            //  ImageBlit         blit;
            //  blit.srcRange.aspectFlags = GetImageAspectFlags(ImageAspectFlags::AspectColor);
            //  blit.dstRange.aspectFlags = GetImageAspectFlags(ImageAspectFlags::AspectColor);
            //  blit.srcOffsets[0]        = Offset3D{0, 0, 0};
            //  blit.srcOffsets[1]        = VulkanUtility::GetOffset3D(ext);
            //  blit.dstOffsets[0]        = Offset3D{0, 0, 0};
            //  blit.dstOffsets[1]        = VulkanUtility::GetOffset3D(ext);
            //
            //  regions.push_back(blit);
            //  cmd.CMD_BlitImage(mainPass._colorTexture->GetImage()._allocatedImg.image, ImageLayout::ShaderReadOnlyOptimal, worldData.finalTexture->GetImage()._allocatedImg.image, ImageLayout::TransferDstOptimal,
            //  regions,
            //                    Filter::Linear);
        }

        PROFILER_SCOPE_END("Main Pass", PROFILER_THREAD_RENDER);

        // ****** POST PROCESS PASS ******
        // PROFILER_SCOPE_START("PP Pass", PROFILER_THREAD_RENDER);
        // m_guiBackend->SetCmd(&cmd);
        // Event::EventSystem::Get()->Trigger<Event::EDrawGUI>();
        // postPass.Begin(postPass.framebuffers[imageIndex], cmd, defaultRenderArea);
        // auto* ppMat = RenderEngine::Get()->GetEngineMaterial(EngineShaderType::SQPostProcess);
        // ppMat->Bind(cmd, RenderPassType::PostProcess, MaterialBindFlag::BindPipeline | MaterialBindFlag::BindDescriptor);
        // cmd.CMD_Draw(3, 1, 0, 0);
        // m_guiBackend->RecordDrawCommands();
        // postPass.End(cmd);
        // PROFILER_SCOPE_END("PP Pass", PROFILER_THREAD_RENDER);

        // ****** FINAL PASS ******
        PROFILER_SCOPE_START("Final Pass", PROFILER_THREAD_RENDER);
        m_guiBackend->SetCmd(&cmd);
        Event::EventSystem::Get()->Trigger<Event::EDrawGUI>();
        finalPass.Begin(finalPass.framebuffers[imageIndex], cmd, defaultRenderArea);
        auto* finalQuadMat = RenderEngine::Get()->GetEngineMaterial(EngineShaderType::SQFinal);
        finalQuadMat->Bind(cmd, RenderPassType::Final, MaterialBindFlag::BindPipeline | MaterialBindFlag::BindDescriptor);
        cmd.CMD_Draw(3, 1, 0, 0);
        m_guiBackend->RecordDrawCommands();
        finalPass.End(cmd);
        PROFILER_SCOPE_END("Final Pass", PROFILER_THREAD_RENDER);

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

        m_worldDataGPU.clear();
        m_worldDataGPU = m_worldDataCPU;

        for (auto& pair : m_worldDataGPU)
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

    int testCtr = 0;

    void EditorRenderer::OnComponentCreated(const Event::EComponentCreated& ev)
    {
        if (ev.ptr->GetComponentMask().IsSet(World::ComponentMask::Renderable))
        {
            auto  renderable = static_cast<RenderableComponent*>(ev.ptr);
            auto& data       = m_worldDataCPU[ev.world];

            if (!data.initialized)
            {
                data.opaquePass.Initialize(DrawPassMask::Opaque, 1000.0f);
                data.allRenderables.Initialize(250, nullptr);
                data.allRenderables.Initialize(250, nullptr);
                auto entity          = ev.world->GetEntity(LINA_EDITOR_CAMERA_NAME);
                auto comp            = ev.world->GetComponent<Graphics::CameraComponent>(entity);
                data.cameraComponent = comp;

                // Create framebuffers & attachment textures for this world. We won't be using the main passes' textures but will use this instead.
                data.finalColorTexture = VulkanUtility::CreateDefaultPassTextureColor();
                data.finalDepthTexture = VulkanUtility::CreateDefaultPassTextureDepth();
                const String   sidStr  = "World: " + TO_STRING(testCtr++);
                const StringID sid     = TO_SID(sidStr);
                data.finalColorTexture->ChangeSID(sid);
                data.finalColorTexture->SetUserManaged(true);

                Resources::ResourceManager::Get()->GetCache<Texture>()->AddResource(sid, data.finalColorTexture);

                const uint32   imgSize  = static_cast<uint32>(Backend::Get()->GetMainSwapchain()._images.size());
                const Vector2i size     = Backend::Get()->GetMainSwapchain().size;
                auto&          mainPass = m_renderPasses[RenderPassType::Main];
                for (uint32 i = 0; i < imgSize; i++)
                {
                    data.framebuffers.push_back(Framebuffer());
                    auto& fb = data.framebuffers[i];
                    fb       = Framebuffer{
                              .width  = static_cast<uint32>(size.x),
                              .height = static_cast<uint32>(size.y),
                              .layers = 1,
                    };

                    fb.AttachRenderPass(mainPass);
                    fb.AddImageView(data.finalColorTexture->GetImage()._ptrImgView).AddImageView(data.finalDepthTexture->GetImage()._ptrImgView);
                    fb.Create();
                }

                data.initialized = true;
            }

            renderable->m_renderableID = m_worldDataCPU[ev.world].allRenderables.AddItem(renderable);
        }
    }

    void EditorRenderer::OnComponentDestroyed(const Event::EComponentDestroyed& ev)
    {
        if (ev.ptr->GetComponentMask().IsSet(World::ComponentMask::Renderable))
            m_worldDataCPU[ev.world].allRenderables.RemoveItem(static_cast<RenderableComponent*>(ev.ptr)->GetRenderableID());
    }

    void EditorRenderer::OnLevelUninstalled(const Event::ELevelUninstalled& ev)
    {
        Join();
        m_meshEntries.clear();
        m_mergedModelIDs.clear();
        m_hasLevelLoaded = false;

        for (auto& pair : m_worldDataCPU)
        {
            if (ev.world == pair.first)
                pair.second.allRenderables.Reset();
        }
    }

    void EditorRenderer::OnPreMainLoop(const Event::EPreMainLoop& ev)
    {
        auto* finalQuadMat = RenderEngine::Get()->GetEngineMaterial(EngineShaderType::SQFinal);
        auto* ppMat        = RenderEngine::Get()->GetEngineMaterial(EngineShaderType::SQPostProcess);
        auto& mainPass     = m_renderPasses[RenderPassType::Main];
        finalQuadMat->SetTexture(0, RenderEngine::Get()->GetEngineTexture(EngineTextureType::DummyBlack32));
        finalQuadMat->CheckUpdatePropertyBuffers();
        m_guiBackend->UpdateProjection();
    }

    void EditorRenderer::ConnectEvents()
    {
        Event::EventSystem::Get()->Connect<Event::ELevelUninstalled, &EditorRenderer::OnLevelUninstalled>(this);
        Event::EventSystem::Get()->Connect<Event::ELevelInstalled, &EditorRenderer::OnLevelInstalled>(this);
        Event::EventSystem::Get()->Connect<Event::EComponentCreated, &EditorRenderer::OnComponentCreated>(this);
        Event::EventSystem::Get()->Connect<Event::EComponentDestroyed, &EditorRenderer::OnComponentDestroyed>(this);
        Event::EventSystem::Get()->Connect<Event::EPreMainLoop, &EditorRenderer::OnPreMainLoop>(this);
        Event::EventSystem::Get()->Connect<Event::EWindowResized, &EditorRenderer::OnWindowResized>(this);
        Event::EventSystem::Get()->Connect<Event::EWindowPositioned, &EditorRenderer::OnWindowPositioned>(this);
        Event::EventSystem::Get()->Connect<Event::EResourceLoaded, &EditorRenderer::OnResourceLoaded>(this);
        Event::EventSystem::Get()->Connect<Event::EWorldDestroyed, &EditorRenderer::OnWorldDestroyed>(this);
    }

    void EditorRenderer::OnWorldDestroyed(const Event::EWorldDestroyed& ev)
    {
        auto it = m_worldDataCPU.find(ev.world);

        if (it != m_worldDataCPU.end())
        {
            auto* world = it->first;

            if (world == ev.world)
            {
                Join();
                delete it->second.finalColorTexture;
                delete it->second.finalDepthTexture;

                for (auto& fb : it->second.framebuffers)
                    fb.Destroy();

                m_worldDataCPU.erase(it);
                return;
            }
        }
    }

} // namespace Lina::Editor
