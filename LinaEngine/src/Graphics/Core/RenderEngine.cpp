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

#include "Graphics/Core/RenderEngine.hpp"
#include "Log/Log.hpp"
#include "World/Core/World.hpp"
#include "Graphics/Core/Renderer.hpp"

#include "Graphics/Resource/Shader.hpp"
#include "EventSystem/EventSystem.hpp"
#include "EventSystem/WindowEvents.hpp"
#include "EventSystem/MainLoopEvents.hpp"
#include "Resource/Core/ResourceManager.hpp"

#include "Graphics/Resource/Model.hpp"
#include "Graphics/Resource/ModelNode.hpp"
#include "Graphics/Resource/Mesh.hpp"
#include "Graphics/Resource/Material.hpp"
#include "Graphics/Core/GUIBackend.hpp"
#include "Profiling/Profiler.hpp"
#include "Graphics/Utility/Vulkan/VulkanUtility.hpp"

#define LINAVG_TEXT_SUPPORT
#include "LinaVG/LinaVG.hpp"

#ifdef LINA_PLATFORM_WINDOWS
#include "Graphics/Platform/Win32/Win32Window.hpp"
#endif

namespace Lina::Graphics
{
    RenderEngine* RenderEngine::s_instance = nullptr;
    Model*        cube                     = nullptr;

#define RETURN_NOTINITED                                                                                                                                                                                                   \
    if (!m_initedSuccessfully)                                                                                                                                                                                             \
    return

    void RenderEngine::Initialize(const InitInfo& initInfo)
    {
        LINA_TRACE("[Initialization] -> Render Engine ({0})", typeid(*this).name());

        Event::EventSystem::Get()->Connect<Event::EEngineResourcesLoaded, &RenderEngine::OnEngineResourcesLoaded>(this);
        Event::EventSystem::Get()->Connect<Event::EPreMainLoop, &RenderEngine::OnPreMainLoop>(this);
        Event::EventSystem::Get()->Connect<Event::EWindowPositioned, &RenderEngine::OnWindowPositioned>(this);

        Backend::s_instance = &m_backend;

        m_initedSuccessfully = m_windowManager.Initialize(initInfo.windowProperties, &m_screen);
        m_initedSuccessfully = m_backend.Initialize(initInfo, &m_windowManager);
        m_backend.SetSwapchainPosition(m_windowManager.GetMainWindow().GetHandle(), m_windowManager.GetMainWindow().GetPos());

        if (!m_initedSuccessfully)
        {
            LINA_ERR("[Render Engine] -> Initialization failed, no rendering will occur!");
            return;
        }

        m_screen.Initialize(m_renderer, m_backend.m_swapchains[LINA_MAIN_SWAPCHAIN_ID]);

        DescriptorSetLayoutBinding globalBinding = DescriptorSetLayoutBinding{
            .binding         = 0,
            .descriptorCount = 1,
            .stageFlags      = GetShaderStage(ShaderStage::Fragment) | GetShaderStage(ShaderStage::Vertex),
            .type            = DescriptorType::UniformBuffer,
        };

        m_descriptorLayouts[DescriptorSetType::GlobalSet].AddBinding(globalBinding).Create();

        DescriptorSetLayoutBinding sceneBinding = DescriptorSetLayoutBinding{
            .binding         = 0,
            .descriptorCount = 1,
            .stageFlags      = GetShaderStage(ShaderStage::Fragment) | GetShaderStage(ShaderStage::Vertex),
            .type            = DescriptorType::UniformBuffer,
        };

        DescriptorSetLayoutBinding viewDataBinding = DescriptorSetLayoutBinding{
            .binding         = 1,
            .descriptorCount = 1,
            .stageFlags      = GetShaderStage(ShaderStage::Vertex),
            .type            = DescriptorType::UniformBuffer,
        };

        DescriptorSetLayoutBinding lightDataBinding = DescriptorSetLayoutBinding{
            .binding         = 2,
            .descriptorCount = 1,
            .stageFlags      = GetShaderStage(ShaderStage::Vertex),
            .type            = DescriptorType::UniformBuffer,
        };

        DescriptorSetLayoutBinding objDataBinding = DescriptorSetLayoutBinding{
            .binding         = 3,
            .descriptorCount = 1,
            .stageFlags      = GetShaderStage(ShaderStage::Vertex),
            .type            = DescriptorType::StorageBuffer,
        };

        m_descriptorLayouts[DescriptorSetType::PassSet].AddBinding(sceneBinding).AddBinding(viewDataBinding).AddBinding(lightDataBinding).AddBinding(objDataBinding).Create();

        m_globalAndPassLayout.AddDescriptorSetLayout(m_descriptorLayouts[DescriptorSetType::GlobalSet]).AddDescriptorSetLayout(m_descriptorLayouts[DescriptorSetType::PassSet]).Create();
        m_gpuUploader.Create();

        // Init GUI Backend, LinaVG
        LinaVG::Config.displayPosX           = 0;
        LinaVG::Config.displayPosY           = 0;
        LinaVG::Config.displayWidth          = 0;
        LinaVG::Config.displayHeight         = 0;
        LinaVG::Config.flipTextureUVs        = false;
        LinaVG::Config.framebufferScale.x    = m_screen.GetContentScale().x;
        LinaVG::Config.framebufferScale.y    = m_screen.GetContentScale().y;
        LinaVG::Config.aaMultiplier          = LinaVG::Config.framebufferScale.x * 0.4f;
        LinaVG::Config.aaEnabled             = true;
        LinaVG::Config.textCachingEnabled    = true;
        LinaVG::Config.textCachingSDFEnabled = true;
        LinaVG::Config.textCacheReserve      = 10000;

        LinaVG::Config.errorCallback = [](const std::string& err) { LINA_ERR(err.c_str()); };
        LinaVG::Config.logCallback   = [](const std::string& log) { LINA_TRACE(log.c_str()); };

        m_guiBackend = new GUIBackend();
        LinaVG::Backend::BaseBackend::SetBackend(m_guiBackend);
        LinaVG::Initialize();

        // Engine materials
        m_engineShaderNames[EngineShaderType::LitStandard]   = "LitStandard";
        m_engineShaderNames[EngineShaderType::GUIStandard]   = "GUIStandard";
        m_engineShaderNames[EngineShaderType::GUIText]       = "GUIText";
        m_engineShaderNames[EngineShaderType::SQFinal]       = "ScreenQuads/SQFinal";
        m_engineShaderNames[EngineShaderType::SQPostProcess] = "ScreenQuads/SQPostProcess";

        // Engine models
        m_enginePrimitiveNames[EnginePrimitiveType::Capsule]  = "Capsule";
        m_enginePrimitiveNames[EnginePrimitiveType::Cube]     = "Cube";
        m_enginePrimitiveNames[EnginePrimitiveType::Cylinder] = "Cylinder";
        m_enginePrimitiveNames[EnginePrimitiveType::LinaLogo] = "LinaLogo";
        m_enginePrimitiveNames[EnginePrimitiveType::Plane]    = "Plane";
        m_enginePrimitiveNames[EnginePrimitiveType::Quad]     = "Quad";
        m_enginePrimitiveNames[EnginePrimitiveType::Sphere]   = "Sphere";
        m_enginePrimitiveNames[EnginePrimitiveType::Suzanne]  = "Suzanne";

        // Engine textures
        m_engineTextureNames[EngineTextureType::LogoWithText]    = "LogoWithText";
        m_engineTextureNames[EngineTextureType::LogoColored1024] = "Logo_Colored_1024";
        m_engineTextureNames[EngineTextureType::LogoWhite512]    = "Logo_White_512";
        m_engineTextureNames[EngineTextureType::Grid512]         = "Grid512";
        m_engineTextureNames[EngineTextureType::DummyBlack32]    = "DummyBlack_32";
        
    }

    void RenderEngine::Tick()
    {
        m_renderer->Tick();
    }

    void RenderEngine::Render()
    {
        PROFILER_FUNC(PROFILER_THREAD_RENDER);
        RETURN_NOTINITED;

        auto& window = m_windowManager.GetMainWindow();
        if (window.IsMinimized()) //
            return;

        if (ApplicationInfo::GetAppMode() == ApplicationMode::Standalone)
        {
            if (!window.GetIsAppActive())
                return;
        }

        m_gpuUploader.Poll();
        m_renderer->Render();
    }

    void RenderEngine::Stop()
    {
    }

    void RenderEngine::Join()
    {
        RETURN_NOTINITED;
        m_renderer->Join();
    }

    void RenderEngine::SetRenderer(Renderer* renderer)
    {
        m_renderer         = renderer;
        auto mainSwapchain = m_backend.m_swapchains[LINA_MAIN_SWAPCHAIN_ID];
        m_renderer->Initialize(mainSwapchain, m_guiBackend, &m_windowManager);
        m_renderer->UpdateViewport(mainSwapchain->size);
    }

    void RenderEngine::SyncData()
    {
        m_renderer->SyncData();
    }

    void RenderEngine::Shutdown()
    {
        RETURN_NOTINITED;

        Event::EventSystem::Get()->Disconnect<Event::EEngineResourcesLoaded>(this);
        Event::EventSystem::Get()->Disconnect<Event::EPreMainLoop>(this);
        Event::EventSystem::Get()->Disconnect<Event::EWindowPositioned>(this);

        for (auto pair : m_engineMaterials)
            delete pair.second;

        LinaVG::Terminate();

        LINA_TRACE("[Shutdown] -> Render Engine ({0})", typeid(*this).name());

        m_gpuUploader.Destroy();
        m_mainDeletionQueue.Flush();
        m_renderer->Shutdown();
        delete m_renderer;
        m_windowManager.Shutdown();
        m_backend.Shutdown();
    }

    void RenderEngine::OnEngineResourcesLoaded(const Event::EEngineResourcesLoaded& ev)
    {
        auto* rm = Resources::ResourceManager::Get();

        for (auto& p : m_engineTextureNames)
        {
            const String txtPath      = "Resources/Engine/Textures/" + p.second + ".png";
            Texture*     texture      = rm->GetResource<Texture>(txtPath);
            m_engineTextures[p.first] = texture;
        }

        for (auto& p : m_engineShaderNames)
        {
            const String shaderPath  = "Resources/Engine/Shaders/" + p.second + ".linashader";
            Shader*      shader      = rm->GetResource<Shader>(shaderPath);
            m_engineShaders[p.first] = shader;

            const String materialPath = "Resources/Engine/Materials/" + p.second + ".linamat";
            Material*    material     = new Material();
            material->m_path          = materialPath;
            material->m_sid           = TO_SID(materialPath);
            material->CreateBuffer();
            material->SetShader(shader);
            m_engineMaterials[p.first] = material;
        }

        for (auto& p : m_enginePrimitiveNames)
        {
            const String modelPath  = "Resources/Engine/Models/" + p.second + ".fbx";
            Model*       model      = rm->GetResource<Model>(modelPath);
            m_engineModels[p.first] = model;
        }

        m_placeholderMaterial  = m_engineMaterials[EngineShaderType::LitStandard];
        m_placeholderModel     = m_engineModels[EnginePrimitiveType::Cube];
        m_placeholderModelNode = m_placeholderModel->GetRootNode();
    }

    void RenderEngine::OnPreMainLoop(const Event::EPreMainLoop& ev)
    {
        auto mainSwapchain = m_backend.m_swapchains[LINA_MAIN_SWAPCHAIN_ID];
        m_guiBackend->UpdateProjection(mainSwapchain->size);
    }

    void RenderEngine::OnWindowPositioned(const Event::EWindowPositioned& ev)
    {
        m_backend.SetSwapchainPosition(ev.window, ev.newPos);
    }

    Vector<String> RenderEngine::GetEngineShaderPaths()
    {
        Vector<String> paths;

        for (auto& p : m_engineShaderNames)
        {
            const String path = "Resources/Engine/Shaders/" + p.second + ".linashader";
            paths.push_back(path);
        }

        return paths;
    }

    Vector<String> RenderEngine::GetEnginePrimitivePaths()
    {
        Vector<String> paths;

        for (auto& p : m_enginePrimitiveNames)
        {
            const String path = "Resources/Engine/Models/" + p.second + ".fbx";
            paths.push_back(path);
        }

        return paths;
    }

    Vector<String> RenderEngine::GetEngineTexturePaths()
    {
        Vector<String> paths;

        for (auto& p : m_engineTextureNames)
        {
            const String path = "Resources/Engine/Textures/" + p.second + ".png";
            paths.push_back(path);
        }

        return paths;
    }

    Mesh* RenderEngine::GetPlaceholderMesh()
    {
        return GetPlaceholderModelNode()->GetMeshes()[0];
    }

} // namespace Lina::Graphics
