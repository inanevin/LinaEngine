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

#include "Graphics/Core/SurfaceRenderer.hpp"
#include "Profiling/Profiler.hpp"
#include "Graphics/Core/RenderEngine.hpp"
#include "EventSystem/WindowEvents.hpp"
#include "EventSystem/EventSystem.hpp"
#include "Graphics/Core/GUIBackend.hpp"
#include "Graphics/Platform/LinaVGIncl.hpp"
#include "EventSystem/GraphicsEvents.hpp"
#include "Graphics/Resource/Material.hpp"

#define LINAVG_TEXT_SUPPORT
#include "LinaVG/LinaVG.hpp"

namespace Lina::Graphics
{
    void SurfaceRenderer::SetOffscreenTexture(Texture* txt)
    {
        if (!m_mask.IsSet(RM_DrawOffscreenTexture))
        {
            LINA_ERR("[Surface Renderer] -> Renderer is not set to draw an offscreen texture, returning!");
            return;
        }

        for (auto m : m_offscreenMaterials)
        {
            m->SetTexture(0, txt);
            m->CheckUpdatePropertyBuffers();
        }
    }

    bool SurfaceRenderer::Initialize(GUIBackend* guiBackend, WindowManager* windowManager, RenderEngine* eng)
    {
        if (m_swapchain == nullptr)
        {
            LINA_ERR("[Surface Renderer] -> Surface renderers require a swapchain, aborting init!");
            return false;
        }

        m_type = RendererType::SurfaceRenderer;

        Renderer::Initialize(guiBackend, windowManager, eng);

        Event::EventSystem::Get()->Connect<Event::EWindowResized, &SurfaceRenderer::OnWindowResized>(this);

        if (m_mask.IsSet(RM_DrawOffscreenTexture))
        {
            m_offscreenMaterials.resize(FRAMES_IN_FLIGHT);
            for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
            {
                m_offscreenMaterials[i] = new Material();
                m_offscreenMaterials[i]->SetShader(m_renderEngine->GetEngineShader(EngineShaderType::SQFinal));
            }
        }

        if (m_mask.IsSet(RM_ApplyPostProcessing))
            LINA_WARN("[Surface Renderer] -> Post Processing mask has no effect on surface renderers!");

        return true;
    }

    void SurfaceRenderer::Shutdown()
    {
        Renderer::Shutdown();

        for (auto& m : m_offscreenMaterials)
            delete m;

        Event::EventSystem::Get()->Disconnect<Event::EWindowResized>(this);
    }

    bool SurfaceRenderer::AcquireImage(uint32 frameIndex)
    {
        VulkanResult res;
        m_acquiredImage = m_swapchain->AcquireNextImage(1.0, m_swapchain->_submitSemaphores[frameIndex], res);
        return CanContinueWithAcquiredImage(res, true);
    }

    CommandBuffer* SurfaceRenderer::Render(uint32 frameIndex, Fence& fence)
    {
        PROFILER_FUNC(PROFILER_THREAD_RENDER);

        auto& cmd = m_cmds[frameIndex];
        cmd.Reset(true);
        cmd.Begin(GetCommandBufferFlags(CommandBufferFlags::OneTimeSubmit));

        const uint32 depthTransitionFlags = GetPipelineStageFlags(PipelineStageFlags::EarlyFragmentTests) | GetPipelineStageFlags(PipelineStageFlags::LateFragmentTests);

        const Viewport viewport = Viewport{
            .x      = 0,
            .y      = 0,
            .width  = static_cast<float>(m_swapchain->size.x),
            .height = static_cast<float>(m_swapchain->size.y),
        };

        const Recti scissors          = Recti(0, 0, m_swapchain->size.x, m_swapchain->size.y);
        const Recti defaultRenderArea = Recti(Vector2(viewport.x, viewport.y), Vector2(viewport.width, viewport.height));
        cmd.CMD_SetViewport(viewport);
        cmd.CMD_SetScissors(scissors);

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
            if (m_mask.IsSet(RM_RenderGUI))
            {
                LinaVG::StartFrame();
                m_guiBackend->Prepare(m_swapchain, frameIndex, &cmd);
                Event::EventSystem::Get()->Trigger<Event::EDrawGUI>({m_swapchain});
            }

            cmd.CMD_BeginRenderingDefault(swapchainImageView, swapchainDepthImageView, defaultRenderArea);

            if (!m_offscreenMaterials.empty())
            {
                m_offscreenMaterials[frameIndex]->Bind(cmd, MaterialBindFlag::BindDescriptor | MaterialBindFlag::BindPipeline);
                cmd.CMD_Draw(3, 1, 0, 0);
            }

            // Render GUI on top
            if (m_mask.IsSet(RM_RenderGUI))
                m_guiBackend->RecordDrawCommands();

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

    void SurfaceRenderer::AcquiredImageInvalid(uint32 frameIndex)
    {
        m_swapchain->_submitSemaphores[frameIndex].Destroy();
        m_swapchain->_submitSemaphores[frameIndex].Create(false);
    }

    bool SurfaceRenderer::CanContinueWithAcquiredImage(VulkanResult res, bool disallowSuboptimal)
    {
        if (!m_recreateSwapchain)
            m_newSwapchainSize = m_swapchain->size;

        if (m_recreateSwapchain || res == VulkanResult::OutOfDateKHR || (disallowSuboptimal && res == VulkanResult::SuboptimalKHR))
        {
            Backend::Get()->WaitIdle();
            m_recreateSwapchain = false;

            // Swapchain
            m_swapchain->Destroy(false);
            m_swapchain->size = m_newSwapchainSize;
            m_swapchain->Create(LINA_MAIN_SWAPCHAIN_ID);

            // Make sure we always match swapchain
            m_newSwapchainSize = m_swapchain->size;
            return false;
        }

        return true;
    }

    void SurfaceRenderer::OnWindowResized(const Event::EWindowResized& ev)
    {
        SimpleAction act;
        act.Action = [ev, this]() {
            m_recreateSwapchain = true;
            m_newSwapchainSize  = ev.newSize;
        };
        m_syncedActions.push_back(act);
    }

} // namespace Lina::Graphics
