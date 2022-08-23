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
#include "Core/World.hpp"

#include "Resource/Shader.hpp"
#include "EventSystem/EventSystem.hpp"
#include "EventSystem/GraphicsEvents.hpp"
#include "EventSystem/MainLoopEvents.hpp"
#include "EventSystem/ResourceEvents.hpp"
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

    void RenderEngine::Initialize(const InitInfo& initInfo)
    {
        LINA_TRACE("[Initialization] -> Render Engine ({0})", typeid(*this).name());

        Window::s_instance   = &m_window;
        Backend::s_instance  = &m_backend;
        Renderer::s_instance = &m_levelRenderer;

        m_initedSuccessfully = m_window.Initialize(initInfo.windowProperties);
        m_initedSuccessfully = m_backend.Initialize(initInfo);

        Event::EventSystem::Get()->Connect<Event::ESwapchainRecreated, &RenderEngine::OnSwapchainRecreated>(this);
        Event::EventSystem::Get()->Connect<Event::EEngineResourcesLoaded, &RenderEngine::OnEngineResourcesLoaded>(this);

        if (!m_initedSuccessfully)
        {
            LINA_ERR("[Render Engine] -> Initialization failed, no rendering will occur!");
            return;
        }
        const Vector2i size = m_window.GetSize();

        m_viewport = Viewport{
            .x        = 0.0f,
            .y        = 0.0f,
            .width    = static_cast<float>(size.x),
            .height   = static_cast<float>(size.y),
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
        };

        m_scissor = Recti{
            .pos  = Vector2(0.0f, 0.0f),
            .size = size,
        };

        m_levelRenderer.Initialize();
    }

    //  void RenderEngine::OnPreStartGame(const Event::EPreStartGame& ev)
    //  {

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
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
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
    //  }

    void RenderEngine::Clear()
    {
        RETURN_NOTINITED;
    }

    void RenderEngine::Tick()
    {
        m_levelRenderer.Tick();
    }

    void RenderEngine::GameSimCompleted()
    {
        auto* world = World::EntityWorld::Get();
        m_levelRenderer.FetchAndExtract(world);
    }

    void RenderEngine::Render()
    {
        PROFILER_FUNC(PROFILER_THREAD_RENDER);
        RETURN_NOTINITED;

        if (m_window.IsMinimized())
            return;

        m_levelRenderer.Render();
    }

    void RenderEngine::Join()
    {
        RETURN_NOTINITED;
        m_levelRenderer.Join();
    }

    void RenderEngine::Shutdown()
    {
        RETURN_NOTINITED;

        LINA_TRACE("[Shutdown] -> Render Engine ({0})", typeid(*this).name());

        m_mainDeletionQueue.Flush();

        m_levelRenderer.Shutdown();
        m_window.Shutdown();
        m_backend.Shutdown();
    }

    void RenderEngine::OnSwapchainRecreated(const Event::ESwapchainRecreated& ev)
    {
        Swapchain* swp = static_cast<Swapchain*>(ev.newPtr);

        m_viewport = Viewport{
            .x        = 0.0f,
            .y        = 0.0f,
            .width    = static_cast<float>(swp->width),
            .height   = static_cast<float>(swp->height),
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
        };
        // const Vector2i size = m_window.GetSize();
        // for (int i = 0; i < m_framebuffers.size(); i++)
        // {
        //     Framebuffer& fb = m_framebuffers[i];
        //     fb.Destroy();
        //     fb = Framebuffer{.width = static_cast<uint32>(size.x), .height = static_cast<uint32>(size.y), .layers = 1};
        //     fb.AttachRenderPass(m_renderPass).AddImageView(m_backend.m_swapchain._imageViews[i]).AddImageView(m_depthImage._ptrImgView).Create();
        // }
    }

    void RenderEngine::OnEngineResourcesLoaded(const Event::EEngineResourcesLoaded& ev)
    {
        m_placeholderMaterial  = Resources::ResourceStorage::Get()->GetResource<Material>("Resources/Engine/Materials/Default.linamat");
        m_placeholderModel     = Resources::ResourceStorage::Get()->GetResource<Model>("Resources/Engine/Meshes/Primitives/Cube.fbx");
        m_placeholderModelNode = m_placeholderModel->GetRootNode();
    }

} // namespace Lina::Graphics
