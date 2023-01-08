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
#include "Graphics/Platform/LinaVGIncl.hpp"
#include "GUI/GUI.hpp"

using namespace Lina::Graphics;
namespace Lina::Editor
{
    void EditorRenderer::CreateAdditionalWindow(const String& name, void** handle, Graphics::Swapchain** swp, const Vector2i& pos, const Vector2i& size)
    {
        LOCK_GUARD(m_additionalWindowMtx);
        AdditionalWindowRequest req = AdditionalWindowRequest{
            .name   = name,
            .handle = handle,
            .swp    = swp,
            .pos    = pos,
            .size   = size,
        };

        m_additionalWindowRequests.push_back(req);
    }

    void EditorRenderer::RemoveAdditionalWindow(StringID sid)
    {
        auto  it = m_additionalWindows.find(sid);
        auto& wd = it->second;
        Backend::Get()->DestroyAdditionalSwapchain(sid);
        m_windowManager->DestroyAppWindow(sid);
        m_additionalWindows.erase(it);
    }

    void EditorRenderer::Tick()
    {
        for (auto& [world, data] : m_worldsToRender)
        {
            auto* activeCamera = world->GetActiveCamera();
            if (activeCamera == nullptr)
                continue;

            m_cameraSystem.CalculateCamera(world->GetActiveCamera());
            data.playerView.Tick(activeCamera->GetEntity()->GetPosition(), activeCamera->GetView(), activeCamera->GetProjection());
        }
    }

    void EditorRenderer::Shutdown()
    {
        Renderer::Shutdown();

        Vector<StringID> toRemove;
        for (auto& pair : m_additionalWindows)
            toRemove.push_back(pair.first);

        for (auto sid : toRemove)
            RemoveAdditionalWindow(sid);
    }

    void EditorRenderer::Render()
    {
        PROFILER_FUNC(PROFILER_THREAD_RENDER);

        const uint32 frameIndex = GetFrameIndex();
        auto&        frame      = m_frames[frameIndex];

        frame.graphicsFence.Wait(true, 1.0f);

        Vector<EditorFrameData> allFrameData;

        EditorFrameData mainData = EditorFrameData{
            .submitSemaphore  = &frame.submitSemaphore,
            .presentSemaphore = &frame.presentSemaphore,
            .swp              = m_swapchain,
            .viewport         = m_viewport,
        };

        allFrameData.push_back(mainData);

        for (auto& [sid, wd] : m_additionalWindows)
        {
            EditorFrameData data = EditorFrameData{
                .submitSemaphore  = &wd.submitSemaphores[frameIndex],
                .presentSemaphore = &wd.presentSemaphore[frameIndex],
                .swp              = wd.swapchain,
            };

            data.viewport.width  = static_cast<float>(wd.size.x);
            data.viewport.height = static_cast<float>(wd.size.y);
            data.wd              = &wd;
            allFrameData.push_back(data);
        }

        for (auto& fd : allFrameData)
        {
            const bool                            isMainSwapchain = fd.swp == m_swapchain;
            EditorRenderer::AdditionalWindowData* wd              = isMainSwapchain ? nullptr : GetWindowDataFromSwapchain(fd.swp);
            VulkanResult                          res;
            const uint32                          imageIndex         = fd.swp->AcquireNextImage(1.0, isMainSwapchain ? frame.submitSemaphore : wd->submitSemaphores[frameIndex], res);
            auto                                  swapchainImage     = fd.swp->_images[imageIndex];
            auto                                  swapchainImageView = fd.swp->_imageViews[imageIndex];

            if (isMainSwapchain)
            {
                if (HandleOutOfDateImage(res))
                    return;
            }
            else
            {
                if (HandleOutOfDateImageAdditional(wd, res))
                    return;
            }

            fd.imageIndex = imageIndex;
            fd.cmd        = isMainSwapchain ? &m_cmds[imageIndex] : &fd.wd->cmds[imageIndex];
        }

        frame.graphicsFence.Reset();

        LinaVG::StartFrame();
        LGUI->StartFrame();

        for (auto& fd : allFrameData)
        {
            const Recti defaultRenderArea = Recti(Vector2(fd.viewport.x, fd.viewport.y), Vector2(fd.viewport.width, fd.viewport.height));

            fd.cmd->Reset(true);
            fd.cmd->Begin(GetCommandBufferFlags(CommandBufferFlags::OneTimeSubmit));

            fd.cmd->CMD_SetViewport(fd.viewport);
            fd.cmd->CMD_SetScissors(defaultRenderArea);

            // Merged object buffer.
            uint64 offset = 0;
            fd.cmd->CMD_BindVertexBuffers(0, 1, m_gpuVtxBuffer._ptr, &offset);
            fd.cmd->CMD_BindIndexBuffers(m_gpuIndexBuffer._ptr, 0, IndexType::Uint32);

            // Global set.
            // cmd.CMD_BindDescriptorSets(PipelineBindPoint::Graphics, RenderEngine::Get()->GetGlobalAndPassLayouts()._ptr, 0, 1, &frame.globalDescriptor, 0, nullptr);

            PROFILER_SCOPE_START("Main Passes", PROFILER_THREAD_RENDER);

            // Render worlds only for the main swapchain.
            if (fd.swp == m_swapchain)
            {
                for (auto& [world, worldData] : m_worldsToRenderGPU)
                {
                    // Pass set.
                    fd.cmd->CMD_BindDescriptorSets(PipelineBindPoint::Graphics, RenderEngine::Get()->GetGlobalAndPassLayouts()._ptr, 1, 1, &worldData.passDescriptor, 0, nullptr);

                    // Global - scene data.
                    worldData.sceneDataBuffer[frameIndex].CopyInto(&worldData.sceneData, sizeof(GPUSceneData));

                    // Global - light data.
                    worldData.lightDataBuffer[frameIndex].CopyInto(&worldData.lightData, sizeof(GPULightData));

                    // Per render pass - obj data.
                    Vector<GPUObjectData> gpuObjectData;

                    for (auto& r : worldData.extractedRenderables)
                    {
                        // Object data.
                        GPUObjectData objData;
                        objData.modelMatrix = r.modelMatrix;
                        gpuObjectData.push_back(objData);
                    }

                    worldData.objDataBuffer[frameIndex].CopyInto(gpuObjectData.data(), sizeof(GPUObjectData) * gpuObjectData.size());

                    fd.cmd->CMD_ImageTransition_ToColorOptimal(worldData.finalColorTexture->GetImage()._allocatedImg.image);
                    fd.cmd->CMD_ImageTransition_ToDepthOptimal(worldData.finalDepthTexture->GetImage()._allocatedImg.image);

                    fd.cmd->CMD_BeginRenderingDefault(worldData.finalColorTexture->GetImage()._ptrImgView, worldData.finalDepthTexture->GetImage()._ptrImgView, defaultRenderArea);
                    RenderWorld(*fd.cmd, worldData);
                    fd.cmd->CMD_EndRendering();

                    fd.cmd->CMD_ImageTransition_ToColorShaderRead(worldData.finalColorTexture->GetImage()._allocatedImg.image);
                }
            }

            PROFILER_SCOPE_END("Main Passes", PROFILER_THREAD_RENDER);

            // ********* FINAL PASS *********
            {
                PROFILER_SCOPE_START("Final Pass", PROFILER_THREAD_RENDER);

                auto image     = fd.swp->_images[fd.imageIndex];
                auto imageView = fd.swp->_imageViews[fd.imageIndex];

                fd.cmd->CMD_ImageTransition_ToColorOptimal(image);

                // Issue GUI commands
                m_guiBackend->Prepare(fd.swp, fd.imageIndex, fd.cmd);
                LGUI->SetCurrentSwapchain(fd.swp);
                Event::EventSystem::Get()->Trigger<Event::EDrawGUI>();

                // Actually draw commands
                fd.cmd->CMD_BeginRenderingFinal(imageView, defaultRenderArea);
                m_guiBackend->RecordDrawCommands();
                fd.cmd->CMD_EndRendering();

                // Make sure presentable
                fd.cmd->CMD_ImageTransition_ToPresent(image);
                PROFILER_SCOPE_END("Final Pass", PROFILER_THREAD_RENDER);
            }

            fd.cmd->End();
        }

        LGUI->EndFrame();
        LinaVG::EndFrame();

        Vector<Semaphore*>     allSubmitSemaphores;
        Vector<Semaphore*>     allPresentSemaphores;
        Vector<CommandBuffer*> allCommandBuffers;
        Vector<Swapchain*>     allSwapchains;
        Vector<uint32>         allImageIndices;

        for (auto& fd : allFrameData)
        {
            allSubmitSemaphores.push_back(fd.submitSemaphore);
            allPresentSemaphores.push_back(fd.presentSemaphore);
            allCommandBuffers.push_back(fd.cmd);
            allImageIndices.push_back(fd.imageIndex);
            allSwapchains.push_back(fd.swp);
        }

        // Submit command waits on the present semaphore, e.g. it waits for the acquired image to be ready.
        // Then submits command, and signals render semaphore when its submitted.
        PROFILER_SCOPE_START("Queue Submit & Present", PROFILER_THREAD_RENDER);
        Backend::Get()->GetGraphicsQueue().Submit(allSubmitSemaphores, allPresentSemaphores, frame.graphicsFence, allCommandBuffers, 1);
        Backend::Get()->GetGraphicsQueue().Present(allPresentSemaphores, allSwapchains, allImageIndices);
        // Backend::Get()->GetGraphicsQueue().WaitIdle();
        PROFILER_SCOPE_END("Queue Submit & Present", PROFILER_THREAD_RENDER);

        m_frameNumber++;
    }

    void EditorRenderer::SyncData()
    {
        Renderer::SyncData();

        LOCK_GUARD(m_additionalWindowMtx);

        for (auto& r : m_additionalWindowRequests)
        {
            const StringID sid = TO_SID(r.name);
            m_windowManager->CreateAppWindow(m_windowManager->GetMainWindow().GetHandle(), r.name.c_str(), r.pos, r.size, true);
            auto& createdWindow = m_windowManager->GetWindow(sid);
            auto* windowPtr     = createdWindow.GetHandle();
            auto* swapchainPtr  = Backend::Get()->CreateAdditionalSwapchain(sid, windowPtr, static_cast<int>(r.size.x), static_cast<int>(r.size.y));
            auto& data          = m_additionalWindows[sid];
            *r.swp              = swapchainPtr;
            *r.handle           = windowPtr;

            data.swapchain = swapchainPtr;

            for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
            {
                data.submitSemaphores[i].Create();
                data.presentSemaphore[i].Create();
            }

            const uint32 imageSize = static_cast<uint32>(swapchainPtr->_images.size());
            for (uint32 i = 0; i < imageSize; i++)
            {
                CommandBuffer buf = CommandBuffer{.count = 1, .level = CommandBufferLevel::Primary};
                data.cmds.push_back(buf);
                data.cmds[i].Create(m_cmdPool._ptr);
            }

            data.size = r.size;
            data.pos  = r.pos;
        }

        m_additionalWindowRequests.clear();
    }

    bool EditorRenderer::HandleOutOfDateImageAdditional(AdditionalWindowData* wd, VulkanResult res)
    {
        bool shouldRecreate = false;
        if (res == VulkanResult::OutOfDateKHR || res == VulkanResult::SuboptimalKHR)
            shouldRecreate = true;
        else if (res != VulkanResult::Success)
            LINA_ASSERT(false, "Could not acquire next image!");

        if (shouldRecreate)
        {
            Backend::Get()->WaitIdle();
            Vector2i size = wd->size;

            if (size.x == 0 || size.y == 0)
            {
                Backend::Get()->GetGraphicsQueue().Submit(m_frames[GetFrameIndex()].submitSemaphore);
                return true;
            }

            // Swapchain
            wd->swapchain->Destroy();
            wd->swapchain->size = size;
            wd->swapchain->Create(wd->swapchain->swapchainID);

            // Make sure we always match swapchain
            size = wd->swapchain->size;

            // Make sure the semaphore is unsignalled after resize operation.
            Backend::Get()->GetGraphicsQueue().Submit(m_frames[GetFrameIndex()].submitSemaphore);

            return true;
        }

        return false;
    }

    EditorRenderer::AdditionalWindowData* EditorRenderer::GetWindowDataFromSwapchain(Graphics::Swapchain* swp)
    {
        for (auto& pair : m_additionalWindows)
        {
            if (pair.second.swapchain == swp)
                return &pair.second;
        }

        return nullptr;
    }

} // namespace Lina::Editor
