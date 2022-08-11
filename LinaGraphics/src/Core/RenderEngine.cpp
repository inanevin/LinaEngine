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

#include "Core/RenderEngine.hpp"
#include "Log/Log.hpp"
#include "ECS/Components/DecalComponent.hpp"
#include "ECS/Components/SkyComponent.hpp"
#include "ECS/Components/LightComponent.hpp"
#include "ECS/Components/MeshComponent.hpp"
#include "ECS/Components/SpriteComponent.hpp"
#include "ECS/Components/ParticleComponent.hpp"

#include "Resource/Shader.hpp"
#include "EventSystem/EventSystem.hpp"
#include "EventSystem/GraphicsEvents.hpp"
#include "EventSystem/MainLoopEvents.hpp"
#include "Core/ResourceStorage.hpp"

#include "Resource/Model.hpp"
#include "Resource/ModelNode.hpp"
#include "Resource/Mesh.hpp"

namespace Lina::Graphics
{
    RenderEngine* RenderEngine::s_instance = nullptr;

    Model* cube = nullptr;

#define RETURN_NOTINITED       \
    if (!m_initedSuccessfully) \
    return

    void RenderEngine::Initialize(ApplicationInfo& appInfo)
    {
        LINA_TRACE("[Initialization] -> Render Engine ({0})", typeid(*this).name());
        m_appInfo = appInfo;

        Window::s_instance  = &m_window;
        Backend::s_instance = &m_backend;

        m_initedSuccessfully = m_window.Initialize(appInfo);
        m_initedSuccessfully = m_backend.Initialize(appInfo);

        Event::EventSystem::Get()->Connect<Event::ESwapchainRecreated, &RenderEngine::OnSwapchainRecreated>(this);
        Event::EventSystem::Get()->Connect<Event::EPreStartGame, &RenderEngine::OnPreStartGame>(this);

        if (!m_initedSuccessfully)
        {
            LINA_ERR("[Render Engine] -> Initialization failed, no rendering will occur!");
            return;
        }

        m_skySystem.Initialize("Sky System");
        m_particleSystem.Initialize("Particle System");
        m_decalSystem.Initialize("Decal System");
        m_spriteSystem.Initialize("Sprite System");
        m_lightingSystem.Initialize("Lighting System");
        m_meshSystem.Initialize("Mesh System");

        m_graphicsQueue.Get(m_backend.GetQueueFamilyIndex(QueueFamilies::Graphics));
        m_pool = CommandPool{.familyIndex = m_graphicsQueue._family, .flags = GetCommandPoolCreateFlags(CommandPoolFlags::Reset)}.Create();

        m_commandBuffer = CommandBuffer{.count = 1, .level = CommandBufferLevel::Primary}
                              .Create(m_pool._ptr);

        Attachment att          = Attachment{.format = m_backend.m_swapchain.format, .loadOp = LoadOp::Clear};
        Attachment depthStencil = Attachment{
            .format         = Format::D32_SFLOAT,
            .loadOp         = LoadOp::Clear,
            .storeOp        = StoreOp::Store,
            .stencilLoadOp  = LoadOp::Clear,
            .stencilStoreOp = StoreOp::DontCare,
            .initialLayout  = ImageLayout::Undefined,
            .finalLayout    = ImageLayout::DepthStencilOptimal};

        SubPass sp = SubPass{.bindPoint = PipelineBindPoint::Graphics}
                         .AddColorAttachmentRef(0, ImageLayout::ColorOptimal)
                         .Create();

        m_renderPass = RenderPass().AddAttachment(att).AddSubpass(sp).Create();
        m_framebuffers.reserve(m_backend.m_swapchain._imageViews.size());

        const Vector2i size = m_window.GetSize();

        for (auto iv : m_backend.m_swapchain._imageViews)
        {
            Framebuffer fb = Framebuffer{.width = static_cast<uint32>(size.x), .height = static_cast<uint32>(size.y), .layers = 1}.AttachRenderPass(m_renderPass).Create(iv);
            m_framebuffers.push_back(fb);
        }

        m_presentSemaphore.Create();
        m_renderSemaphore.Create();
        m_renderFence = Fence{.flags = GetFenceFlags(FenceFlags::Signaled)}.Create();
    }

    void RenderEngine::OnPreStartGame(const Event::EPreStartGame& ev)
    {

        RETURN_NOTINITED;
        const Vector2i size = m_window.GetSize();

        Extent3D ext = Extent3D{.width  = static_cast<unsigned int>(size.x),
                                .height = static_cast<unsigned int>(size.y),
                                .depth  = 1};

        m_depthImage = Image{
            .format          = Format::D32_SFLOAT,
            .tiling          = ImageTiling::Optimal,
            .extent          = ext,
            .aspectFlags     = GetImageAspectFlags(ImageAspectFlags::AspectDepth),
            .imageUsageFlags = GetImageUsage(ImageUsageFlags::DepthStencilAttachment)}
                           .Create();

        Shader* defaultShader = Resources::ResourceStorage::Get()->GetResource<Shader>("Resources/Engine/Shaders/default.linashader");
        cube                  = Resources::ResourceStorage::Get()->GetResource<Model>("Resources/Engine/Meshes/Primitives/Cube.fbx");

        PushConstantRange r = PushConstantRange{
            .offset     = 0,
            .size       = sizeof(MeshPushConstants),
            .stageFlags = GetShaderStage(ShaderStage::Vertex),
        };

        m_pipelineLayout = PipelineLayout{}.AddPushConstant(r).Create();

        Viewport vp = Viewport{
            .x        = 0.0f,
            .y        = 0.0f,
            .width    = static_cast<float>(size.x),
            .height   = static_cast<float>(size.y),
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
        };
        Recti scissors = Recti{
            .pos  = Vector2(0.0f, 0.0f),
            .size = size,
        };

        m_pipeline = Pipeline{
            .viewport    = vp,
            .scissor     = scissors,
            .topology    = Topology::TriangleList,
            .polygonMode = PolygonMode::Fill,
            .cullMode    = CullMode::None,
        }
                         .SetShader(defaultShader)
                         .SetLayout(m_pipelineLayout)
                         .SetRenderPass(m_renderPass)
                         .Create();
    }

    void RenderEngine::SyncRenderData()
    {
        RETURN_NOTINITED;

        PROFILER_FUNC(PROFILER_THREAD_SIMULATION);

        m_systemList.UpdateSystems(0.0f);
    }

    void RenderEngine::Clear()
    {
        RETURN_NOTINITED;
    }

    void RenderEngine::Render()
    {
        RETURN_NOTINITED;

        m_renderFence.Wait(true, 1.0);
        m_renderFence.Reset();

        uint32 imageIndex = m_backend.m_swapchain.AcquireNextImage(1.0, m_presentSemaphore);

        m_commandBuffer.Reset();
        m_commandBuffer.Begin(GetCommandBufferFlags(CommandBufferFlags::OneTimeSubmit));

        m_renderPass.Begin(ClearValue{.clearColor = Color::Blue}, m_framebuffers[imageIndex], m_commandBuffer);
        m_pipeline.Bind(m_commandBuffer, PipelineBindPoint::Graphics);
        Mesh* m = cube->GetRootNode()->GetMeshes()[0];

        uint64 offset = 0;
        m_commandBuffer.BindVertexBuffers(0, 1, m->GetGPUVtxBuffer().buffer, &offset);

        glm::vec3 camPos = {0.f, 0.f, -2.f};

        static float _frameNumber = 0.0f;
        _frameNumber += 0.1f;

        glm::mat4 view = glm::translate(glm::mat4(1.f), camPos);
        // camera projection
        glm::mat4 projection = glm::perspective(glm::radians(70.f), 1200.0f / 1200.0f, 0.1f, 200.0f);
        projection[1][1] *= -1;
        // model rotation
        glm::mat4 model = glm::rotate(glm::mat4{1.0f}, glm::radians(_frameNumber * 0.4f), glm::vec3(0, 1, 0));
        // glm::mat4 model = glm::translate(glm::mat4{1.0f}, glm::vec3(0.0f, 0.0f, 0.0f));

        // calculate final mesh matrix
        glm::mat4 meshMatrix = projection * view * model;

        MeshPushConstants constants;
        constants.renderMatrix = meshMatrix;

        m_commandBuffer.PushConstants(m_pipelineLayout._ptr, GetShaderStage(ShaderStage::Vertex), 0, sizeof(MeshPushConstants), &constants);

        m_commandBuffer.Draw(m->GetVertices().size(), 1, 0, 0);

        m_renderPass.End(m_commandBuffer);
        m_commandBuffer.End();

        // Submit command waits on the present semaphore, e.g. it waits for the acquired image to be ready.
        // Then submits command, and signals render semaphore when its submitted.
        m_graphicsQueue.Submit(m_presentSemaphore, m_renderSemaphore, m_renderFence, m_commandBuffer, 1);
        m_graphicsQueue.Present(m_renderSemaphore, &imageIndex);

        PROFILER_SCOPE_START("Render Sky", PROFILER_THREAD_RENDER);
        m_skySystem.Render();
        PROFILER_SCOPE_END("Render Sky", PROFILER_THREAD_RENDER);

        PROFILER_SCOPE_START("Render Particles", PROFILER_THREAD_RENDER);
        m_particleSystem.Render();
        PROFILER_SCOPE_END("Render Particles", PROFILER_THREAD_RENDER);

        PROFILER_SCOPE_START("Render Decals", PROFILER_THREAD_RENDER);
        m_decalSystem.Render();
        PROFILER_SCOPE_END("Render Decals", PROFILER_THREAD_RENDER);

        PROFILER_SCOPE_START("Render Sprites", PROFILER_THREAD_RENDER);
        m_spriteSystem.Render();
        PROFILER_SCOPE_END("Render Sprites", PROFILER_THREAD_RENDER);

        PROFILER_SCOPE_START("Render Meshes", PROFILER_THREAD_RENDER);
        m_meshSystem.Render();
        PROFILER_SCOPE_END("Render Meshes", PROFILER_THREAD_RENDER);
    }

    void RenderEngine::Join()
    {
        RETURN_NOTINITED;
        m_renderFence.Wait();
    }

    void RenderEngine::Shutdown()
    {
        RETURN_NOTINITED;

        LINA_TRACE("[Shutdown] -> Render Engine ({0})", typeid(*this).name());

        m_mainDeletionQueue.Flush();

        // m_renderFence.Destroy();
        // m_renderSemaphore.Destroy();
        // m_presentSemaphore.Destroy();
        //
        // m_pipelineLayout.Destroy();
        // m_pipeline.Destroy();
        //
        // m_renderPass.Destroy();
        // m_pool.Destroy();
        //
        for (int i = 0; i < m_framebuffers.size(); i++)
            m_framebuffers[i].Destroy();

        m_window.Shutdown();
        m_backend.Shutdown();
    }

    void RenderEngine::OnSwapchainRecreated(const Event::ESwapchainRecreated& ev)
    {
        const Vector2i size = m_window.GetSize();
        for (int i = 0; i < m_framebuffers.size(); i++)
        {
            Framebuffer& fb = m_framebuffers[i];
            fb.Destroy();
            fb = Framebuffer{.width = static_cast<uint32>(size.x), .height = static_cast<uint32>(size.y), .layers = 1}.AttachRenderPass(m_renderPass).Create(m_backend.m_swapchain._imageViews[i]);
        }
    }

} // namespace Lina::Graphics
