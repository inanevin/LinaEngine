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
#include "EventSystem/WindowEvents.hpp"
#include "EventSystem/MainLoopEvents.hpp"
#include "Core/ResourceManager.hpp"

#include "Resource/Model.hpp"
#include "Resource/ModelNode.hpp"
#include "Resource/Mesh.hpp"
#include "Resource/Material.hpp"
#include "Core/GUIBackend.hpp"

#ifdef LINA_PLATFORM_WINDOWS
#include "Platform/Win32/Win32Window.hpp"
#endif
#include <LinaVG/LinaVG.hpp>

namespace Lina::Graphics
{
    RenderEngine* RenderEngine::s_instance = nullptr;

    Model* cube = nullptr;

#define RETURN_NOTINITED                                                                                                                                                                               \
    if (!m_initedSuccessfully)                                                                                                                                                                         \
    return

    void RenderEngine::Initialize(const InitInfo& initInfo)
    {
        LINA_TRACE("[Initialization] -> Render Engine ({0})", typeid(*this).name());

#ifdef LINA_PLATFORM_WINDOWS
        m_window = new Win32Window();
#else
        F_ASSERT(false, "Platform window implementation not found!");
#endif
        Window::s_instance  = m_window;
        Backend::s_instance = &m_backend;

        m_initedSuccessfully = m_window->Initialize(initInfo.windowProperties);
        m_initedSuccessfully = m_backend.Initialize(initInfo);

        Event::EventSystem::Get()->Connect<Event::EWindowResized, &RenderEngine::OnWindowResized>(this);
        Event::EventSystem::Get()->Connect<Event::EPreMainLoop, &RenderEngine::OnPreMainLoop>(this);
        Event::EventSystem::Get()->Connect<Event::EWindowPositioned, &RenderEngine::OnWindowPositioned>(this);
        Event::EventSystem::Get()->Connect<Event::EEngineResourcesLoaded, &RenderEngine::OnEngineResourcesLoaded>(this);

        if (!m_initedSuccessfully)
        {
            LINA_ERR("[Render Engine] -> Initialization failed, no rendering will occur!");
            return;
        }

        m_descriptorPool = DescriptorPool{
            .maxSets = 10,
            .flags   = DescriptorPoolCreateFlags::None,
        };
        m_descriptorPool.AddPoolSize(DescriptorType::UniformBuffer, 10)
            .AddPoolSize(DescriptorType::UniformBufferDynamic, 10)
            .AddPoolSize(DescriptorType::StorageBuffer, 10)
            .AddPoolSize(DescriptorType::CombinedImageSampler, 10)
            .Create();

        DescriptorSetLayoutBinding sceneBinding = DescriptorSetLayoutBinding{
            .binding         = 0,
            .descriptorCount = 1,
            .stageFlags      = GetShaderStage(ShaderStage::Fragment) | GetShaderStage(ShaderStage::Vertex),
            .type            = DescriptorType::UniformBuffer,
        };

        DescriptorSetLayoutBinding lightDataBinding = DescriptorSetLayoutBinding{
            .binding         = 1,
            .descriptorCount = 1,
            .stageFlags      = GetShaderStage(ShaderStage::Vertex),
            .type            = DescriptorType::UniformBuffer,
        };

        m_descriptorLayouts[DescriptorSetType::GlobalSet].AddBinding(sceneBinding).AddBinding(lightDataBinding).Create();

        DescriptorSetLayoutBinding viewDataBinding = DescriptorSetLayoutBinding{
            .binding         = 0,
            .descriptorCount = 1,
            .stageFlags      = GetShaderStage(ShaderStage::Vertex),
            .type            = DescriptorType::UniformBuffer,
        };

        DescriptorSetLayoutBinding objDataBinding = DescriptorSetLayoutBinding{
            .binding         = 1,
            .descriptorCount = 1,
            .stageFlags      = GetShaderStage(ShaderStage::Vertex),
            .type            = DescriptorType::StorageBuffer,
        };

        m_descriptorLayouts[DescriptorSetType::PassSet].AddBinding(viewDataBinding).AddBinding(objDataBinding).Create();

        m_globalAndPassLayout.AddDescriptorSetLayout(m_descriptorLayouts[DescriptorSetType::GlobalSet]).AddDescriptorSetLayout(m_descriptorLayouts[DescriptorSetType::PassSet]).Create();

        m_renderer.Initialize();
        m_gpuUploader.Create();

        // Init GUI Backend, LinaVG
        LinaVG::Config.displayPosX        = 0;
        LinaVG::Config.displayPosY        = 0;
        LinaVG::Config.displayWidth       = 0;
        LinaVG::Config.displayHeight      = 0;
        LinaVG::Config.flipTextureUVs     = false;
        LinaVG::Config.framebufferScale.x = ApplicationInfo::GetContentScaleWidth();
        LinaVG::Config.framebufferScale.y = ApplicationInfo::GetContentScaleHeight();
        LinaVG::Config.aaMultiplier = ApplicationInfo::GetContentScaleWidth() * 2.5f, LinaVG::Config.aaEnabled = true;

        LinaVG::Config.errorCallback = [](const std::string& err) { LINA_ERR(err.c_str()); };

        LinaVG::Config.logCallback = [](const std::string& log) { LINA_TRACE(log.c_str()); };

        m_guiBackend = new GUIBackend();
        LinaVG::Backend::BaseBackend::SetBackend(m_guiBackend);
        LinaVG::Initialize();

        // Engine materials
        m_engineShaderNames[EngineShaderType::LitStandard]   = "LitStandard";
        m_engineShaderNames[EngineShaderType::GUIStandard]   = "GUIStandard";
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
        m_engineTextureNames[EngineTextureType::DefaultLina] = "DefaultLina";
        m_engineTextureNames[EngineTextureType::Grid512]     = "Grid512";
    }

    void RenderEngine::Clear()
    {
        RETURN_NOTINITED;
    }

    void RenderEngine::Tick()
    {
        m_renderer.Tick();
    }

    void RenderEngine::Render()
    {
        PROFILER_FUNC(PROFILER_THREAD_RENDER);
        RETURN_NOTINITED;

        if (m_window->IsMinimized())
            return;

        m_gpuUploader.Poll();
        m_renderer.Render();
    }

    void RenderEngine::Join()
    {
        RETURN_NOTINITED;
        m_renderer.Join();
    }

    void RenderEngine::Shutdown()
    {
        RETURN_NOTINITED;

        Event::EventSystem::Get()->Disconnect<Event::EWindowResized>(this);
        Event::EventSystem::Get()->Disconnect<Event::EWindowPositioned>(this);
        Event::EventSystem::Get()->Disconnect<Event::EEngineResourcesLoaded>(this);
        Event::EventSystem::Get()->Disconnect<Event::EPreMainLoop>(this);

        LinaVG::Terminate();

        LINA_TRACE("[Shutdown] -> Render Engine ({0})", typeid(*this).name());

        m_gpuUploader.Destroy();
        m_mainDeletionQueue.Flush();

        m_renderer.Shutdown();
        m_window->Shutdown();
        m_backend.Shutdown();
    }

    void RenderEngine::OnEngineResourcesLoaded(const Event::EEngineResourcesLoaded& ev)
    {
        auto* rm               = Resources::ResourceManager::Get();
        m_placeholderMaterial  = rm->GetResource<Material>("Resources/Engine/Materials/LitStandard.linamat");
        m_placeholderModel     = rm->GetResource<Model>("Resources/Engine/Models/Cube.fbx");
        m_placeholderModelNode = m_placeholderModel->GetRootNode();

        for (auto& p : m_engineTextureNames)
        {
            const String txtPath      = "Resources/Engine/Textures/" + p.second + ".png";
            Texture*     texture      = rm->GetResource<Texture>(txtPath);
            m_engineTextures[p.first] = texture;
        }

        for (auto& p : m_engineShaderNames)
        {
            const String shaderPath   = "Resources/Engine/Shaders/" + p.second + ".linashader";
            const String materialPath = "Resources/Engine/Materials/" + p.second + ".linamat";
            Shader*      shader       = rm->GetResource<Shader>(shaderPath);
            Material*    material     = rm->GetResource<Material>(materialPath);

            if (material->GetShaderHandle().sid == 0 && ApplicationInfo::GetAppMode() == ApplicationMode::Editor)
            {
                material->m_path = materialPath;
                material->m_sid  = TO_SID(materialPath);
                material->SetShader(shader);
                material->SaveToFile();
            }

            m_engineShaders[p.first]   = shader;
            m_engineMaterials[p.first] = material;
        }

        for (auto& p : m_enginePrimitiveNames)
        {
            const String modelPath  = "Resources/Engine/Models/" + p.second + ".fbx";
            Model*       model      = rm->GetResource<Model>(modelPath);
            m_engineModels[p.first] = model;
        }
    }

    void RenderEngine::OnWindowResized(const Event::EWindowResized& ev)
    {
        m_renderer.OnWindowResized(ev.newSize);
        const Vector2i size = Backend::Get()->GetSwapchain().size;

        m_viewport.width             = static_cast<float>(size.x);
        m_viewport.height            = static_cast<float>(size.y);
        m_scissors.size              = size;
        LinaVG::Config.displayWidth  = static_cast<unsigned int>(m_viewport.width);
        LinaVG::Config.displayHeight = static_cast<unsigned int>(m_viewport.height);
    }

    void RenderEngine::OnWindowPositioned(const Event::EWindowPositioned& ev)
    {
        m_renderer.OnWindowPositioned(ev.newPos);

        m_viewport.x               = static_cast<float>(ev.newPos.x);
        m_viewport.y               = static_cast<float>(ev.newPos.y);
        m_scissors.pos             = ev.newPos;
        LinaVG::Config.displayPosX = static_cast<unsigned int>(m_viewport.x);
        LinaVG::Config.displayPosY = static_cast<unsigned int>(m_viewport.y);
    }

    void RenderEngine::OnPreMainLoop(const Event::EPreMainLoop& ev)
    {
        const Vector2i size = Backend::Get()->GetSwapchain().size;

        m_viewport = Viewport{
            .x        = 0.0f,
            .y        = 0.0f,
            .width    = static_cast<float>(size.x),
            .height   = static_cast<float>(size.y),
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
        };

        m_scissors = Recti{
            .pos  = Vector2(m_viewport.x, m_viewport.y),
            .size = size,
        };

        LinaVG::Config.displayPosX   = static_cast<unsigned int>(m_viewport.x);
        LinaVG::Config.displayPosX   = static_cast<unsigned int>(m_viewport.y);
        LinaVG::Config.displayWidth  = static_cast<unsigned int>(m_viewport.width);
        LinaVG::Config.displayHeight = static_cast<unsigned int>(m_viewport.height);
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

    Vector<String> RenderEngine::GetEngineMaterialPaths()
    {
        Vector<String> paths;

        for (auto& p : m_engineShaderNames)
        {
            const String path = "Resources/Engine/Materials/" + p.second + ".linamat";
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
