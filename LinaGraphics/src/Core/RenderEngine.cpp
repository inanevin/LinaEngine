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
        m_renderer.Initialize();
    }

    void RenderEngine::OnPreStartGame(const Event::EPreStartGame& ev)
    {

        //  RETURN_NOTINITED;
        //  const Vector2i size = m_window.GetSize();
        //
        //  Shader* defaultShader = Resources::ResourceStorage::Get()->GetResource<Shader>("Resources/Engine/Shaders/default.linashader");
        //  cube                  = Resources::ResourceStorage::Get()->GetResource<Model>("Resources/Engine/Meshes/Primitives/BlenderMonkey.obj");
        //
        //  PushConstantRange r = PushConstantRange{
        //      .offset     = 0,
        //      .size       = sizeof(MeshPushConstants),
        //      .stageFlags = GetShaderStage(ShaderStage::Vertex),
        //  };
        //
        //  m_pipelineLayout = PipelineLayout{};
        //  m_pipelineLayout.AddPushConstant(r).Create();
        //
        //  Viewport vp = Viewport{
        //      .x        = 0.0f,
        //      .y        = 0.0f,
        //      .width    = static_cast<float>(size.x),
        //      .height   = static_cast<float>(size.y),
        //      .minDepth = 0.0f,
        //      .maxDepth = 1.0f,
        //  };
        //  Recti scissors = Recti{
        //      .pos  = Vector2(0.0f, 0.0f),
        //      .size = size,
        //  };
        //
        //  m_pipeline = Pipeline{
        //      .depthTestEnabled  = true,
        //      .depthWriteEnabled = true,
        //      .depthCompareOp    = CompareOp::LEqual,
        //      .viewport          = vp,
        //      .scissor           = scissors,
        //      .topology          = Topology::TriangleList,
        //      .polygonMode       = PolygonMode::Fill,
        //      .cullMode          = CullMode::None,
        //  };
        //
        //  m_pipeline.SetShader(defaultShader)
        //      .SetLayout(m_pipelineLayout)
        //      .SetRenderPass(m_renderPass)
        //      .Create();
    }

    void RenderEngine::SyncRenderData()
    {
        RETURN_NOTINITED;

        PROFILER_FUNC(PROFILER_THREAD_SIMULATION);

        m_renderer.FetchData();
    }

    void RenderEngine::Clear()
    {
        RETURN_NOTINITED;
    }

    void RenderEngine::Render()
    {
        RETURN_NOTINITED;

        if (m_window.IsMinimized())
            return;

        m_renderer.Render();

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

    }

    void RenderEngine::Join()
    {
        RETURN_NOTINITED;
        m_renderer.Join();
    }

    void RenderEngine::Shutdown()
    {
        RETURN_NOTINITED;

        LINA_TRACE("[Shutdown] -> Render Engine ({0})", typeid(*this).name());

        m_mainDeletionQueue.Flush();

        m_renderer.Shutdown();
        m_window.Shutdown();
        m_backend.Shutdown();
    }

    void RenderEngine::OnSwapchainRecreated(const Event::ESwapchainRecreated& ev)
    {
        // const Vector2i size = m_window.GetSize();
        // for (int i = 0; i < m_framebuffers.size(); i++)
        // {
        //     Framebuffer& fb = m_framebuffers[i];
        //     fb.Destroy();
        //     fb = Framebuffer{.width = static_cast<uint32>(size.x), .height = static_cast<uint32>(size.y), .layers = 1};
        //     fb.AttachRenderPass(m_renderPass).AddImageView(m_backend.m_swapchain._imageViews[i]).AddImageView(m_depthImage._ptrImgView).Create();
        // }
    }

} // namespace Lina::Graphics
