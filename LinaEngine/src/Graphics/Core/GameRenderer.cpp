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

#include "Graphics/Core/GameRenderer.hpp"
#include "Graphics/Core/GUIBackend.hpp"
#include "EventSystem/EventSystem.hpp"
#include "EventSystem/WorldEvents.hpp"
#include "EventSystem/GraphicsEvents.hpp"
#include "Graphics/Utility/Vulkan/VulkanUtility.hpp"
#include "Profiling/Profiler.hpp"
#include "Graphics/Core/RenderEngine.hpp"
#include "World/Core/Entity.hpp"
#include "Graphics/Components/RenderableComponent.hpp"
#include "Graphics/Resource/Material.hpp"
#include "EventSystem/LevelEvents.hpp"
#include "World/Core/World.hpp"
#include "Graphics/Components/CameraComponent.hpp"
#include "Graphics/Platform/LinaVGIncl.hpp"

namespace Lina::Graphics
{

    void GameRenderer::Tick()
    {
        if (m_installedWorld == nullptr)
            return;

        auto* activeCamera = m_installedWorld->GetActiveCamera();
        if (activeCamera == nullptr)
            return;

        m_cameraSystem.CalculateCamera(m_installedWorld->GetActiveCamera());
        m_worldsToRender[m_installedWorld].playerView.Tick(activeCamera->GetEntity()->GetPosition(), activeCamera->GetView(), activeCamera->GetProjection());
    }

    void GameRenderer::OnTexturesRecreated()
    {
        if (m_installedWorld == nullptr)
            return;

        auto* ppMat = RenderEngine::Get()->GetEngineMaterial(EngineShaderType::SQPostProcess);
        ppMat->SetTexture(0, m_worldsToRender[m_installedWorld].finalColorTexture);
        ppMat->CheckUpdatePropertyBuffers();
    }

    void GameRenderer::Render()
    {
        PROFILER_FUNC(PROFILER_THREAD_RENDER);

        if (m_worldsToRenderGPU.empty())
            return;

        auto&        wd         = m_worldsToRenderGPU[m_installedWorld];
        const uint32 frameIndex = GetFrameIndex();
        auto&        frame      = m_frames[frameIndex];

        frame.graphicsFence.Wait(true, 1.0f);

        VulkanResult res;
        const uint32 imageIndex              = m_mainSwapchain->AcquireNextImage(1.0, frame.submitSemaphore, res);
        auto         swapchainImage          = m_mainSwapchain->_images[imageIndex];
        auto         swapchainImageView      = m_mainSwapchain->_imageViews[imageIndex];
        auto         swapchainDepthImage     = m_mainSwapchain->_depthImages[imageIndex]._allocatedImg.image;
        auto         swapchainDepthImageView = m_mainSwapchain->_depthImages[imageIndex]._ptrImgView;

        if (HandleOutOfDateImage(m_mainSwapchain, res, true))
            return;

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
        const uint32 depthTransitionFlags = GetPipelineStageFlags(PipelineStageFlags::EarlyFragmentTests) | GetPipelineStageFlags(PipelineStageFlags::LateFragmentTests);

        cmd.CMD_ImageTransition(mainPassImage, ImageLayout::Undefined, ImageLayout::ColorOptimal, ImageAspectFlags::AspectColor, AccessFlags::None, AccessFlags::ColorAttachmentWrite, PipelineStageFlags::TopOfPipe, PipelineStageFlags::ColorAttachmentOutput);
        cmd.CMD_ImageTransition(mainPassDepthImage, ImageLayout::Undefined, ImageLayout::DepthStencilOptimal, ImageAspectFlags::AspectDepth, AccessFlags::None, AccessFlags::DepthStencilAttachmentWrite, depthTransitionFlags, depthTransitionFlags);

        // ********* MAIN PASS *********
        {
            PROFILER_SCOPE_START("Main Pass", PROFILER_THREAD_RENDER);
            cmd.CMD_BeginRenderingDefault(mainPassImageView, mainPassDepthView, defaultRenderArea);
            RenderWorld(cmd, wd);
            cmd.CMD_EndRendering();

            // Transition to shader read
            cmd.CMD_ImageTransition(mainPassImage, ImageLayout::ColorOptimal, ImageLayout::ShaderReadOnlyOptimal, ImageAspectFlags::AspectColor, AccessFlags::None, AccessFlags::ColorAttachmentWrite, PipelineStageFlags::TopOfPipe,
                                    PipelineStageFlags::ColorAttachmentOutput);

            PROFILER_SCOPE_END("Main Pass", PROFILER_THREAD_RENDER);
        }

        // Transition swapchain to optimal
        cmd.CMD_ImageTransition(swapchainImage, ImageLayout::Undefined, ImageLayout::ColorOptimal, ImageAspectFlags::AspectColor, AccessFlags::None, AccessFlags::ColorAttachmentWrite, PipelineStageFlags::TopOfPipe, PipelineStageFlags::ColorAttachmentOutput);
        cmd.CMD_ImageTransition(swapchainDepthImage, ImageLayout::Undefined, ImageLayout::DepthStencilOptimal, ImageAspectFlags::AspectDepth, AccessFlags::None, AccessFlags::DepthStencilAttachmentWrite, depthTransitionFlags, depthTransitionFlags);

        // ********* FINAL & PP PASS *********
        {
            LinaVG::StartFrame();
            PROFILER_SCOPE_START("Final Pass", PROFILER_THREAD_RENDER);

            // Issue GUI draw commands.
            m_guiBackend->Prepare(m_mainSwapchain, imageIndex, &cmd);
            Event::EventSystem::Get()->Trigger<Event::EDrawGUI>();

            cmd.CMD_BeginRenderingDefault(swapchainImageView, swapchainDepthImageView, defaultRenderArea);
            auto* ppMat = RenderEngine::Get()->GetEngineMaterial(EngineShaderType::SQPostProcess);
            ppMat->Bind(cmd, MaterialBindFlag::BindPipeline | MaterialBindFlag::BindDescriptor);
            cmd.CMD_Draw(3, 1, 0, 0);

            // Render GUI on top
            m_guiBackend->RecordDrawCommands();
            cmd.CMD_EndRendering();

            // Transition to present
            cmd.CMD_ImageTransition(swapchainImage, ImageLayout::ColorOptimal, ImageLayout::PresentSurface, ImageAspectFlags::AspectColor, AccessFlags::ColorAttachmentWrite, AccessFlags::None, PipelineStageFlags::ColorAttachmentOutput,
                                    PipelineStageFlags::BottomOfPipe);

            PROFILER_SCOPE_END("Final Pass", PROFILER_THREAD_RENDER);
            LinaVG::EndFrame();
        }

        cmd.End();

        // Submit command waits on the present semaphore, e.g. it waits for the acquired image to be ready.
        // Then submits command, and signals render semaphore when its submitted.
        PROFILER_SCOPE_START("Queue Submit & Present", PROFILER_THREAD_RENDER);
        Backend::Get()->GetGraphicsQueue().Submit(frame.submitSemaphore, frame.presentSemaphore, frame.graphicsFence, cmd, 1);
        Backend::Get()->GetGraphicsQueue().Present(frame.presentSemaphore, imageIndex, res);
        HandleOutOfDateImage(m_mainSwapchain, res, false);
        // Backend::Get()->GetGraphicsQueue().WaitIdle();
        PROFILER_SCOPE_END("Queue Submit & Present", PROFILER_THREAD_RENDER);

        m_frameNumber++;
    }

} // namespace Lina::Graphics
