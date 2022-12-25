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
#include "Core/Renderer.hpp"

#include "Resource/Shader.hpp"
#include "EventSystem/EventSystem.hpp"
#include "EventSystem/MainLoopEvents.hpp"
#include "Core/ResourceManager.hpp"

#include "Resource/Model.hpp"
#include "Resource/ModelNode.hpp"
#include "Resource/Mesh.hpp"
#include "Resource/Material.hpp"
#include "Core/GUIBackend.hpp"
#include "Profiling/Profiler.hpp"
#include "Utility/Vulkan/VulkanUtility.hpp"

#define LINAVG_TEXT_SUPPORT
#include "LinaVG/LinaVG.hpp"

#ifdef LINA_PLATFORM_WINDOWS
#include "Platform/Win32/Win32Window.hpp"
#endif

namespace Lina::Graphics
{
    RenderEngine* RenderEngine::s_instance = nullptr;
    Model* cube = nullptr;

#define RETURN_NOTINITED                                                                                                                                                                                                   \
    if (!m_initedSuccessfully)                                                                                                                                                                                             \
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

        Event::EventSystem::Get()->Connect<Event::EEngineResourcesLoaded, &RenderEngine::OnEngineResourcesLoaded>(this);

        if (!m_initedSuccessfully)
        {
            LINA_ERR("[Render Engine] -> Initialization failed, no rendering will occur!");
            return;
        }

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
        m_gpuUploader.Create();

        // Init GUI Backend, LinaVG
        LinaVG::Config.displayPosX           = 0;
        LinaVG::Config.displayPosY           = 0;
        LinaVG::Config.displayWidth          = 0;
        LinaVG::Config.displayHeight         = 0;
        LinaVG::Config.flipTextureUVs        = false;
        LinaVG::Config.framebufferScale.x    = RuntimeInfo::GetContentScaleWidth();
        LinaVG::Config.framebufferScale.y    = RuntimeInfo::GetContentScaleHeight();
        LinaVG::Config.aaMultiplier          = RuntimeInfo::GetContentScaleWidth() * 0.4f;
        LinaVG::Config.aaEnabled             = true;
        LinaVG::Config.textCachingEnabled    = true;
        LinaVG::Config.textCachingSDFEnabled = true;
        LinaVG::Config.textCacheReserve      = 10000;

        LinaVG::Config.errorCallback = [](const std::string& err) { LINA_ERR(err.c_str()); };
        LinaVG::Config.logCallback = [](const std::string& log) { LINA_TRACE(log.c_str()); };

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
        m_engineTextureNames[EngineTextureType::LogoWhite256]    = "Logo_White_256";
        m_engineTextureNames[EngineTextureType::Grid512]         = "Grid512";
    }

    void RenderEngine::Tick()
    {
        m_renderer->Tick();
    }

    void RenderEngine::Render()
    {
        PROFILER_FUNC(PROFILER_THREAD_RENDER);
        RETURN_NOTINITED;

        if (m_window->IsMinimized() || !m_window->IsActiveWindow())
            return;

        m_gpuUploader.Poll();
        m_renderer->Render();
    }

    void RenderEngine::Stop()
    {
       m_renderer->Stop();
    }

    void RenderEngine::Join()
    {
        RETURN_NOTINITED;
        m_renderer->Join();
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

        LinaVG::Terminate();

        LINA_TRACE("[Shutdown] -> Render Engine ({0})", typeid(*this).name());

        m_gpuUploader.Destroy();
        m_mainDeletionQueue.Flush();
        m_renderer->Shutdown();
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

    //  void RenderEngine::CreateAdditionalWindow(const String& nameID, const Vector2& pos, const Vector2& size)
    //  {
    //     const StringID    sid              = TO_SID(nameID);
    //     AdditionalWindow& additionalWindow = m_additionalWindows[sid];
    //     additionalWindow.sid               = sid;
    //
    //     auto* windowPtr = Window::Get()->CreateAdditionalWindow(nameID.c_str(), pos, size);
    //     auto* swp       = Backend::Get()->CreateAdditionalSwapchain(sid, windowPtr, static_cast<int>(size.x), static_cast<int>(size.y));
    //
    //     additionalWindow.swapchain = swp;
    //     additionalWindow.depthImg  = VulkanUtility::CreateDefaultPassTextureDepth(true, static_cast<int>(size.x), static_cast<int>(size.y));
    //     additionalWindow.waitSemaphore.Create();
    //     additionalWindow.presentSemaphore.Create();
    //
    //     auto& fp = m_renderer.GetRenderPass(RenderPassType::Final);
    //
    //     for (auto& iv : additionalWindow.swapchain->_imageViews)
    //     {
    //         Framebuffer fb = Framebuffer{
    //             .width  = static_cast<uint32>(size.x),
    //             .height = static_cast<uint32>(size.y),
    //             .layers = 1,
    //         };
    //
    //         fb.AttachRenderPass(fp).AddImageView(iv).AddImageView(additionalWindow.depthImg->GetImage()._ptrImgView).Create();
    //         additionalWindow.framebuffers.push_back(fb);
    //     }
    //  }
    //
    //  void RenderEngine::UpdateAdditionalWindow(const String& nameID, const Vector2& pos, const Vector2& size)
    //  {
    //       const StringID sid = TO_SID(nameID);
    //       DestroyAdditionalWindow(nameID);
    //       CreateAdditionalWindow(nameID, pos, size);
    //  }
    //
    //  void RenderEngine::DestroyAdditionalWindow(const String& nameID)
    //  {
    //     const StringID sid = TO_SID(nameID);
    //     auto&          w   = m_additionalWindows[sid];
    //
    //     delete w.depthImg;
    //     w.swapchain->Destroy();
    //     for (auto& fb : w.framebuffers)
    //         fb.Destroy();
    //  }

} // namespace Lina::Graphics
