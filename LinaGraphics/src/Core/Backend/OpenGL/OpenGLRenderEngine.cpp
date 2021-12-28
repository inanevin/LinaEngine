/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

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

#include "Core/Backend/OpenGL/OpenGLRenderEngine.hpp"

#include "Core/Timer.hpp"
#include "ECS/Components/CameraComponent.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "ECS/Components/LightComponent.hpp"
#include "ECS/Components/MeshRendererComponent.hpp"
#include "ECS/Components/ModelRendererComponent.hpp"
#include "ECS/Components/SpriteRendererComponent.hpp"
#include "ECS/Registry.hpp"
#include "EventSystem/EventSystem.hpp"
#include "EventSystem/GraphicsEvents.hpp"
#include "EventSystem/ResourceEvents.hpp"
#include "EventSystem/WindowEvents.hpp"
#include "Helpers/DrawParameterHelper.hpp"
#include "Log/Log.hpp"
#include "Math/Math.hpp"
#include "Math/Transformation.hpp"
#include "Rendering/ArrayBitmap.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/RenderConstants.hpp"
#include "Rendering/Shader.hpp"
#include "Utility/ModelLoader.hpp"
#include "Utility/UtilityFunctions.hpp"

namespace Lina::Graphics
{
    OpenGLRenderEngine* OpenGLRenderEngine::s_renderEngine = nullptr;

    constexpr size_t UNIFORMBUFFER_VIEWDATA_SIZE      = (sizeof(Matrix) * 4) + (sizeof(Vector4)) + (sizeof(float) * 2);
    constexpr int    UNIFORMBUFFER_VIEWDATA_BINDPOINT = 0;
    constexpr auto   UNIFORMBUFFER_VIEWDATA_NAME      = "ViewData";

    constexpr size_t UNIFORMBUFFER_LIGHTDATA_SIZE      = (sizeof(int) * 2) + sizeof(float) * 4;
    constexpr int    UNIFORMBUFFER_LIGHTDATA_BINDPOINT = 1;
    constexpr auto   UNIFORMBUFFER_LIGHTDATA_NAME      = "LightData";

    constexpr size_t UNIFORMBUFFER_DEBUGDATA_SIZE      = (sizeof(bool) * 1);
    constexpr int    UNIFORMBUFFER_DEBUGDATA_BINDPOINT = 2;
    constexpr auto   UNIFORMBUFFER_DEBUGDATA_NAME      = "DebugData";

    void OpenGLRenderEngine::ConnectEvents()
    {
        // Flip loaded images.
        ArrayBitmap::SetImageFlip(true);

        m_eventSystem = Event::EventSystem::Get();
        m_eventSystem->Connect<Event::EWindowResized, &OpenGLRenderEngine::OnWindowResized>(this);
        m_eventSystem->Connect<Event::EDrawLine, &OpenGLRenderEngine::OnDrawLine>(this);
        m_eventSystem->Connect<Event::EDrawBox, &OpenGLRenderEngine::OnDrawBox>(this);
        m_eventSystem->Connect<Event::EDrawCircle, &OpenGLRenderEngine::OnDrawCircle>(this);
        m_eventSystem->Connect<Event::EDrawSphere, &OpenGLRenderEngine::OnDrawSphere>(this);
        m_eventSystem->Connect<Event::EDrawHemiSphere, &OpenGLRenderEngine::OnDrawHemiSphere>(this);
        m_eventSystem->Connect<Event::EDrawCapsule, &OpenGLRenderEngine::OnDrawCapsule>(this);
        m_eventSystem->Connect<Event::ELoadResourceFromFile, &OpenGLRenderEngine::OnLoadResourceFromFile>(this);
        m_eventSystem->Connect<Event::ELoadResourceFromMemory, &OpenGLRenderEngine::OnLoadResourceFromMemory>(this);
    }

    void OpenGLRenderEngine::Initialize(ApplicationMode appMode)
    {
        LINA_TRACE("[Initialization] -> OpenGLRenderEngine ({0})", typeid(*this).name());

        if (Utility::FileExists(RENDERSETTINGS_FULLPATH))
            m_renderSettings = RenderSettings::DeserializeRenderSettings(RENDERSETTINGS_FOLDERPATH, RENDERSETTINGS_FILE);

        // Set references.
        m_appWindow = OpenGLWindow::Get();
        m_appMode   = appMode;

        // Initialize the render device.
        m_renderDevice.Initialize(m_appWindow->GetWidth(), m_appWindow->GetHeight(), m_defaultDrawParams);

        // Setup draw parameters.
        m_defaultDrawParams   = DrawParameterHelper::GetDefault();
        m_skyboxDrawParams    = DrawParameterHelper::GetSkybox();
        m_fullscreenQuadDP    = DrawParameterHelper::GetFullScreenQuad();
        m_shadowMapDrawParams = DrawParameterHelper::GetShadowMap();

        // Construct the uniform buffer for global matrices.
        m_globalDataBuffer.Construct(UNIFORMBUFFER_VIEWDATA_SIZE, BufferUsage::USAGE_DYNAMIC_DRAW, NULL);
        m_globalDataBuffer.Bind(UNIFORMBUFFER_VIEWDATA_BINDPOINT);

        // Construct the uniform buffer for lights.
        m_globalLightBuffer.Construct(UNIFORMBUFFER_LIGHTDATA_SIZE, BufferUsage::USAGE_DYNAMIC_DRAW, NULL);
        m_globalLightBuffer.Bind(UNIFORMBUFFER_LIGHTDATA_BINDPOINT);

        // Construct the uniform buffer for debugging.
        m_globalDebugBuffer.Construct(UNIFORMBUFFER_DEBUGDATA_SIZE, BufferUsage::USAGE_DYNAMIC_DRAW, NULL);
        m_globalDebugBuffer.Bind(UNIFORMBUFFER_DEBUGDATA_BINDPOINT);

        // Initialize engine materials
        // ConstructEngineMaterials();

        // Initialize built-in vertex array objects.
        m_skyboxVAO     = m_renderDevice.CreateSkyboxVertexArray();
        m_hdriCubeVAO   = m_renderDevice.CreateHDRICubeVertexArray();
        m_screenQuadVAO = m_renderDevice.CreateScreenQuadVertexArray();
        m_lineVAO       = m_renderDevice.CreateLineVertexArray();

        // Meshes
        Graphics::ModelLoader::LoadSpriteQuad(m_quadMesh);
        m_quadMesh.CreateVertexArray(Graphics::BufferUsage::USAGE_STATIC_COPY);

        // Construct render targets
        ConstructRenderTargets();

        // Build default textures.
        m_defaultTexture.ConstructEmpty();
        m_defaultCubemapTexture.ConstructRTCubemapTexture(m_screenSize, SamplerParameters());

        // Add the ECS systems into the pipeline.
        m_cameraSystem.Initialize("Camera System", (float)m_screenSize.x / (float)m_screenSize.y);
        m_lightingSystem.Initialize("Lighting System", m_appMode);
        m_modelNodeSystem.Initialize("Model Node System", m_appMode);
        m_spriteRendererSystem.Initialize("Sprite System");
        m_frustumSystem.Initialize("Frustum System");

        AddToRenderingPipeline(m_cameraSystem);
        AddToRenderingPipeline(m_modelNodeSystem);
        AddToRenderingPipeline(m_spriteRendererSystem);
        AddToRenderingPipeline(m_lightingSystem);
        AddToRenderingPipeline(m_frustumSystem);

        // Animation pipeline
        m_animationSystem.Initialize("Animation System");
        m_animationPipeline.AddSystem(m_animationSystem);

        // Set debug values.
        m_debugData.visualizeDepth = false;

        // Shader::ClearShaderIncludes();
    }

    void OpenGLRenderEngine::ConstructShader(const std::string& path, unsigned char* data, size_t dataSize)
    {
        if (path.compare("Resources/Engine/Shaders/Unlit/Unlit.glsl") == 0)
        {
            m_standardUnlitShader = &Shader::CreateShader(path, false, data, dataSize);
            m_standardUnlitShader->BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
            m_standardUnlitShader->BindBlockToBuffer(UNIFORMBUFFER_LIGHTDATA_BINDPOINT, UNIFORMBUFFER_LIGHTDATA_NAME);
            m_standardUnlitShader->BindBlockToBuffer(UNIFORMBUFFER_DEBUGDATA_BINDPOINT, UNIFORMBUFFER_DEBUGDATA_NAME);
        }
        else if (path.compare("Resources/Engine/Shaders/PBR/PBRLitStandard.glsl") == 0)
        {
            m_standardLitShader = &Shader::CreateShader(path, false, data, dataSize);
            m_standardLitShader->BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
            m_standardLitShader->BindBlockToBuffer(UNIFORMBUFFER_LIGHTDATA_BINDPOINT, UNIFORMBUFFER_LIGHTDATA_NAME);
            m_standardLitShader->BindBlockToBuffer(UNIFORMBUFFER_DEBUGDATA_BINDPOINT, UNIFORMBUFFER_DEBUGDATA_NAME);
        }
        else if (path.compare("Resources/Engine/Shaders/PBR/PointShadowsDepth.glsl") == 0)
        {
            m_pointShadowsDepthShader = &Shader::CreateShader(path, true, data, dataSize);
        }
        else if (path.compare("Resources/Engine/Shaders/Skybox/SkyboxColor.glsl") == 0)
        {
            m_skyboxSingleColorShader = &Shader::CreateShader(path, false, data, dataSize);
            m_skyboxSingleColorShader->BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
        }
        else if (path.compare("Resources/Engine/Shaders/Skybox/SkyboxGradient.glsl") == 0 || path.compare("Resources/Engine/Shaders/Skybox/SkyboxCubemap.glsl") == 0 || path.compare("Resources/Engine/Shaders/Skybox/SkyboxProcedural.glsl") == 0 ||
                 path.compare("Resources/Engine/Shaders/Skybox/SkyboxHDRI.glsl") == 0 || path.compare("Resources/Engine/Shaders/Skybox/SkyboxAtmospheric.glsl") == 0)
        {
            Shader& shader = Shader::CreateShader(path, false, data, dataSize);
            shader.BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
        }

        else if (path.compare("Resources/Engine/Shaders/HDRI/HDRIEquirectangular.glsl") == 0)
        {
            m_hdriEquirectangularShader = &Shader::CreateShader(path, false, data, dataSize);
        }
        else if (path.compare("Resources/Engine/Shaders/HDRI/HDRIIrradiance.glsl") == 0)
        {
            m_hdriIrradianceShader = &Shader::CreateShader(path, false, data, dataSize);
        }
        else if (path.compare("Resources/Engine/Shaders/HDRI/HDRIPrefilter.glsl") == 0)
        {
            m_hdriPrefilterShader = &Shader::CreateShader(path, false, data, dataSize);
        }
        else if (path.compare("Resources/Engine/Shaders/HDRI/HDRIBRDF.glsl") == 0)
        {
            m_hdriBRDFShader = &Shader::CreateShader(path, false, data, dataSize);
        }
        else if (path.compare("Resources/Engine/Shaders/ScreenQuads/SQFinal.glsl") == 0)
        {
            m_sqFinalShader = &Shader::CreateShader(path, false, data, dataSize);
            m_sqFinalShader->BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
        }
        else if (path.compare("Resources/Engine/Shaders/ScreenQuads/SQBlur.glsl") == 0)
        {
            m_sqBlurShader = &Shader::CreateShader(path, false, data, dataSize);
            m_sqBlurShader->BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
        }
        else if (path.compare("Resources/Engine/Shaders/ScreenQuads/SQShadowMap.glsl") == 0)
        {
            m_sqShadowMapShader = &Shader::CreateShader(path, false, data, dataSize);
            m_sqShadowMapShader->BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
        }
        else if (path.compare("Resources/Engine/Shaders/Debug/DebugLine.glsl") == 0)
        {
            m_debugLineShader = &Shader::CreateShader(path, false, data, dataSize);
            m_debugLineShader->BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
        }
        else if (path.compare("Resources/Engine/Shaders/Debug/DebugIcon.glsl") == 0)
        {
            m_debugIconShader = &Shader::CreateShader(path, false, data, dataSize);
            m_debugIconShader->BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
        }
        else if (path.compare("Resources/Engine/Shaders/2D/Sprite.glsl") == 0)
        {
            Shader& shader = Shader::CreateShader(path, false, data, dataSize);
            shader.BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
        }
        else
        {
            Shader& shader = Shader::CreateShader(path, false, data, dataSize);
            shader.BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
            shader.BindBlockToBuffer(UNIFORMBUFFER_LIGHTDATA_BINDPOINT, UNIFORMBUFFER_LIGHTDATA_NAME);
            shader.BindBlockToBuffer(UNIFORMBUFFER_DEBUGDATA_BINDPOINT, UNIFORMBUFFER_DEBUGDATA_NAME);
        }
    }

    bool OpenGLRenderEngine::ValidateEngineShaders()
    {
        bool validated = false;

        std::map<StringIDType, Shader*>& loadedShaders = Shader::GetLoadedShaders();

        for (std::map<StringIDType, Shader*>::iterator it = loadedShaders.begin(); it != loadedShaders.end(); ++it)
        {
            LINA_TRACE("Validating {0}", it->second->GetPath());
            bool success = m_renderDevice.ValidateShaderProgram(it->second->GetID());

            if (!success)
                LINA_TRACE("Failed validation");

            validated |= success;
        }

        return !validated;
    }

    void OpenGLRenderEngine::ConstructEngineMaterials()
    {
        Material::SetMaterialShader(m_screenQuadFinalMaterial, *m_sqFinalShader);
        Material::SetMaterialShader(m_screenQuadBlurMaterial, *m_sqBlurShader);
        Material::SetMaterialShader(m_hdriMaterial, *m_hdriEquirectangularShader);
        Material::SetMaterialShader(m_debugLineMaterial, *m_debugLineShader);
        Material::SetMaterialShader(m_debugIconMaterial, *m_debugIconShader);
        Material::SetMaterialShader(m_shadowMapMaterial, *m_sqShadowMapShader);
        Material::SetMaterialShader(m_defaultSkyboxMaterial, *m_skyboxSingleColorShader);
        Material::SetMaterialShader(m_pLightShadowDepthMaterial, *m_pointShadowsDepthShader);
        UpdateRenderSettings();
    }

    void OpenGLRenderEngine::ConstructRenderTargets()
    {
        // Primary
        m_primaryRTParams.m_textureParams.m_pixelFormat         = PixelFormat::FORMAT_RGB;
        m_primaryRTParams.m_textureParams.m_internalPixelFormat = PixelFormat::FORMAT_RGBA16F;
        m_primaryRTParams.m_textureParams.m_minFilter = m_primaryRTParams.m_textureParams.m_magFilter = SamplerFilter::FILTER_LINEAR;
        m_primaryRTParams.m_textureParams.m_wrapS = m_primaryRTParams.m_textureParams.m_wrapT = SamplerWrapMode::WRAP_CLAMP_EDGE;

        // Ping pong
        m_pingPongRTParams.m_textureParams.m_pixelFormat         = PixelFormat::FORMAT_RGB;
        m_pingPongRTParams.m_textureParams.m_internalPixelFormat = PixelFormat::FORMAT_RGBA16F;
        m_pingPongRTParams.m_textureParams.m_minFilter = m_pingPongRTParams.m_textureParams.m_magFilter = SamplerFilter::FILTER_LINEAR;
        m_pingPongRTParams.m_textureParams.m_wrapS = m_pingPongRTParams.m_textureParams.m_wrapT = SamplerWrapMode::WRAP_CLAMP_EDGE;

        // Shadows depth.
        m_shadowsRTParams.m_textureParams.m_pixelFormat         = PixelFormat::FORMAT_DEPTH;
        m_shadowsRTParams.m_textureParams.m_internalPixelFormat = PixelFormat::FORMAT_DEPTH;
        m_shadowsRTParams.m_textureParams.m_minFilter = m_shadowsRTParams.m_textureParams.m_magFilter = SamplerFilter::FILTER_NEAREST;
        m_shadowsRTParams.m_textureParams.m_wrapS = m_shadowsRTParams.m_textureParams.m_wrapR = m_shadowsRTParams.m_textureParams.m_wrapT = SamplerWrapMode::WRAP_CLAMP_EDGE;

        // Initialize primary RT textures
        m_primaryMSAARTTexture0.ConstructRTTextureMSAA(m_screenSize, m_primaryRTParams, 4);
        m_primaryMSAARTTexture1.ConstructRTTextureMSAA(m_screenSize, m_primaryRTParams, 4);
        m_primaryRTTexture0.ConstructRTTexture(m_screenSize, m_primaryRTParams, false);
        m_primaryRTTexture1.ConstructRTTexture(m_screenSize, m_primaryRTParams, false);

        // Initialize ping pong rt texture
        m_pingPongRTTexture1.ConstructRTTexture(m_screenSize, m_pingPongRTParams, false);
        m_pingPongRTTexture2.ConstructRTTexture(m_screenSize, m_pingPongRTParams, false);

        // Shadow map RT texture
        m_shadowMapRTTexture.ConstructRTTexture(m_shadowMapResolution, m_shadowsRTParams, true);

        // Point light RT texture
        for (int i = 0; i < MAX_POINT_LIGHTS; i++)
            m_pLightShadowTextures[i].ConstructRTCubemapTexture(m_pLightShadowResolution, m_shadowsRTParams);

        // Initialize primary render buffer
        m_primaryBuffer.Construct(RenderBufferStorage::STORAGE_DEPTH, m_screenSize);
        m_primaryMSAABuffer.Construct(RenderBufferStorage::STORAGE_DEPTH, m_screenSize, 4);

        // Initialize hdri render buffer
        m_hdriCaptureRenderBuffer.Construct(RenderBufferStorage::STORAGE_DEPTH_COMP24, m_hdriResolution);

        // Initialize primary render target.
        m_primaryRenderTarget.Construct(m_primaryRTTexture0, TextureBindMode::BINDTEXTURE_TEXTURE2D, FrameBufferAttachment::ATTACHMENT_COLOR);
        m_primaryMSAATarget.Construct(m_primaryMSAARTTexture0, TextureBindMode::BINDTEXTURE_TEXTURE2D_MULTISAMPLE, FrameBufferAttachment::ATTACHMENT_COLOR, FrameBufferAttachment::ATTACHMENT_DEPTH, m_primaryMSAABuffer.GetID(), 0);

        // Bind the extre texture to primary render target, also tell open gl that we are running mrts.
        m_renderDevice.BindTextureToRenderTarget(m_primaryRenderTarget.GetID(), m_primaryRTTexture1.GetID(), TextureBindMode::BINDTEXTURE_TEXTURE2D, FrameBufferAttachment::ATTACHMENT_COLOR, 1);
        m_renderDevice.BindTextureToRenderTarget(m_primaryMSAATarget.GetID(), m_primaryMSAARTTexture1.GetID(), TextureBindMode::BINDTEXTURE_TEXTURE2D_MULTISAMPLE, FrameBufferAttachment::ATTACHMENT_COLOR, 1);
        uint32 attachments[2] = {FrameBufferAttachment::ATTACHMENT_COLOR, (FrameBufferAttachment::ATTACHMENT_COLOR + (uint32)1)};
        m_renderDevice.MultipleDrawBuffersCommand(m_primaryRenderTarget.GetID(), 2, attachments);
        m_renderDevice.MultipleDrawBuffersCommand(m_primaryMSAATarget.GetID(), 2, attachments);

        // Initialize ping pong render targets
        m_pingPongRenderTarget1.Construct(m_pingPongRTTexture1, TextureBindMode::BINDTEXTURE_TEXTURE2D, FrameBufferAttachment::ATTACHMENT_COLOR);
        m_pingPongRenderTarget2.Construct(m_pingPongRTTexture2, TextureBindMode::BINDTEXTURE_TEXTURE2D, FrameBufferAttachment::ATTACHMENT_COLOR);

        // Initialize HDRI render target
        m_hdriCaptureRenderTarget.Construct(FrameBufferAttachment::ATTACHMENT_DEPTH, m_hdriCaptureRenderBuffer.GetID());

        // Initialize depth map for shadows
        m_shadowMapTarget.Construct(m_shadowMapRTTexture, TextureBindMode::BINDTEXTURE_TEXTURE2D, FrameBufferAttachment::ATTACHMENT_DEPTH, true);

        // Initialize depth map for point light shadows.
        for (int i = 0; i < MAX_POINT_LIGHTS; i++)
            m_pLightShadowTargets[i].Construct(m_pLightShadowTextures[i], TextureBindMode::BINDTEXTURE_TEXTURE, FrameBufferAttachment::ATTACHMENT_DEPTH, true);

        if (m_appMode == ApplicationMode::Editor)
        {
            m_secondaryRTTexture.ConstructRTTexture(m_screenSize, m_primaryRTParams, false);
            m_secondaryRenderBuffer.Construct(RenderBufferStorage::STORAGE_DEPTH, m_screenSize);
            m_secondaryRenderTarget.Construct(m_secondaryRTTexture, TextureBindMode::BINDTEXTURE_TEXTURE2D, FrameBufferAttachment::ATTACHMENT_COLOR, FrameBufferAttachment::ATTACHMENT_DEPTH, m_secondaryRenderBuffer.GetID());
        }
    }

    void OpenGLRenderEngine::Shutdown()
    {
        LINA_TRACE("[Shutdown] -> OpenGLRenderEngine ({0})", typeid(*this).name());

        // Dump the remaining memory.
        DumpMemory();

        // Release Vertex Array Objects
        m_skyboxVAO     = m_renderDevice.ReleaseVertexArray(m_skyboxVAO);
        m_screenQuadVAO = m_renderDevice.ReleaseVertexArray(m_screenQuadVAO);
        m_hdriCubeVAO   = m_renderDevice.ReleaseVertexArray(m_hdriCubeVAO);
        m_lineVAO       = m_renderDevice.ReleaseVertexArray(m_lineVAO);
    }

    void OpenGLRenderEngine::Tick(float delta)
    {
        m_animationPipeline.UpdateSystems(delta);
    }

    void OpenGLRenderEngine::Render(float interpolation)
    {
        m_eventSystem->Trigger<Event::EPreRender>(Event::EPreRender{});

        Draw();

        if (!m_firstFrameDrawn)
            m_firstFrameDrawn = true;

        // Reset the viewport & fbo to allow any post render drawing, like GUI.
        m_renderDevice.SetFBO(0);
        m_renderDevice.SetViewport(m_screenPos, m_screenSize);
        m_renderDevice.Clear(true, true, true, m_cameraSystem.GetCurrentClearColor(), 0xFF);
        m_eventSystem->Trigger<Event::EPostRender>(Event::EPostRender{});
    }

    void OpenGLRenderEngine::AddToRenderingPipeline(ECS::System& system)
    {
        m_renderingPipeline.AddSystem(system);
    }

    void OpenGLRenderEngine::AddToAnimationPipeline(ECS::System& system)
    {
        m_animationPipeline.AddSystem(system);
    }

    void OpenGLRenderEngine::SetScreenDisplay(Vector2ui pos, Vector2ui size)
    {
        m_renderDevice.SetViewport(pos, size);
        m_screenPos  = pos;
        m_screenSize = size;
        m_cameraSystem.SetAspectRatio((float)m_screenSize.x / (float)m_screenSize.y);

        // Resize render buffers & frame buffer textures
        m_renderDevice.ResizeRTTexture(m_pingPongRTTexture1.GetID(), m_screenSize, m_pingPongRTParams.m_textureParams.m_internalPixelFormat, m_pingPongRTParams.m_textureParams.m_pixelFormat);
        m_renderDevice.ResizeRTTexture(m_pingPongRTTexture1.GetID(), m_screenSize, m_pingPongRTParams.m_textureParams.m_internalPixelFormat, m_pingPongRTParams.m_textureParams.m_pixelFormat);
        m_renderDevice.ResizeRTTexture(m_primaryMSAARTTexture0.GetID(), m_screenSize, m_primaryRTParams.m_textureParams.m_internalPixelFormat, m_primaryRTParams.m_textureParams.m_pixelFormat);
        m_renderDevice.ResizeRTTexture(m_primaryMSAARTTexture1.GetID(), m_screenSize, m_primaryRTParams.m_textureParams.m_internalPixelFormat, m_primaryRTParams.m_textureParams.m_pixelFormat);
        m_renderDevice.ResizeRTTexture(m_primaryRTTexture0.GetID(), m_screenSize, m_primaryRTParams.m_textureParams.m_internalPixelFormat, m_primaryRTParams.m_textureParams.m_pixelFormat);
        m_renderDevice.ResizeRTTexture(m_primaryRTTexture1.GetID(), m_screenSize, m_primaryRTParams.m_textureParams.m_internalPixelFormat, m_primaryRTParams.m_textureParams.m_pixelFormat);

        for (auto& p : m_postProcessMap)
        {
            SamplerParameters sp = p.second.GetParams();
            m_renderDevice.ResizeRTTexture(p.second.GetTexture().GetID(), m_screenSize, sp.m_textureParams.m_internalPixelFormat, sp.m_textureParams.m_pixelFormat);
        }

        if (m_appMode == ApplicationMode::Editor)
            m_renderDevice.ResizeRTTexture(m_secondaryRTTexture.GetID(), m_screenSize, m_primaryRTParams.m_textureParams.m_internalPixelFormat, m_primaryRTParams.m_textureParams.m_pixelFormat);
    }

    void OpenGLRenderEngine::MaterialUpdated(Material& mat)
    {
        std::set<Material*>& hdriMaterials = Material::GetHDRIMaterials();
        if (!mat.m_usesHDRI && hdriMaterials.find(&mat) != hdriMaterials.end())
        {
            hdriMaterials.erase(&mat);
            RemoveHDRIData(&mat);
        }
        else if (mat.m_usesHDRI && hdriMaterials.find(&mat) == hdriMaterials.end())
        {
            hdriMaterials.emplace(&mat);
            SetHDRIData(&mat);
        }

        std::set<Material*>& shadowMappedMaterials = Material::GetShadowMappedMaterials();
        if (!mat.m_isShadowMapped && shadowMappedMaterials.find(&mat) != shadowMappedMaterials.end())
        {
            shadowMappedMaterials.erase(&mat);
            mat.RemoveTexture(MAT_TEXTURE2D_SHADOWMAP);
        }
        else if (mat.m_isShadowMapped && shadowMappedMaterials.find(&mat) == shadowMappedMaterials.end())
        {
            shadowMappedMaterials.emplace(&mat);
            mat.SetTexture(MAT_TEXTURE2D_SHADOWMAP, nullptr);
        }
    }

    void OpenGLRenderEngine::OnLoadResourceFromFile(const Event::ELoadResourceFromFile& event)
    {
        if (event.m_resourceType == Resources::ResourceType::GLSL)
        {
            if (!Shader::ShaderExists(event.m_path))
            {
                LINA_TRACE("[Shader Loader] -> Loading (file): {0}", event.m_path);
                ConstructShader(event.m_path, nullptr, 0);
            }
        }
        else if (event.m_resourceType == Resources::ResourceType::GLH)
        {
            const std::string& name = Utility::GetFileNameOnly(Utility::GetFileWithoutExtension(event.m_path));
            const std::string& text = Utility::GetFileContents(event.m_path);
            Shader::PushShaderInclude(name, text);
        }
        else if (event.m_resourceType == Resources::ResourceType::Model)
        {
            ModelAssetData params;

            LINA_TRACE("[Model Loader] -> Loading (file): {0}", event.m_path);

            bool assetDataExists = Utility::FileExists(event.m_assetDataPath);
            if (assetDataExists)
                params = Model::LoadAssetData(event.m_assetDataPath);

            auto& model = Model::CreateModel(event.m_path, params, event.m_assetDataPath);

            if (!assetDataExists)
                model.SaveAssetData(event.m_assetDataPath);
        }
        else if (event.m_resourceType == Resources::ResourceType::Material)
        {
            LINA_TRACE("[Material Loader] -> Loading (file): {0}", event.m_path);
            Material::LoadMaterialFromFile(event.m_path);

            if (m_defaultLit == nullptr && event.m_path.compare("Resources/Engine/Materials/DefaultLit.mat") == 0)
                m_defaultLit = &Material::GetMaterial("Resources/Engine/Materials/DefaultLit.mat");
            if (m_defaultUnlit == nullptr && event.m_path.compare("Resources/Engine/Materials/DefaultUnlit.mat") == 0)
                m_defaultUnlit = &Material::GetMaterial("Resources/Engine/Materials/DefaultUnlit.mat");
        }
        else if (event.m_resourceType == Resources::ResourceType::Image)
        {
            if (Texture::TextureExists(event.m_path))
                return;

            LINA_TRACE("[Texture Loader] -> Loading (file): {0}", event.m_path);
            ImageAssetData assetData;
            bool           paramsExists = Utility::FileExists(event.m_assetDataPath);

            if (paramsExists)
                assetData = Texture::LoadAssetData(event.m_assetDataPath);

            auto& texture = Texture::CreateTexture2D(event.m_path, assetData.m_samplerParameters, false, false, event.m_assetDataPath);
            if (!paramsExists)
                texture.SaveAssetData(event.m_assetDataPath);
        }
        else if (event.m_resourceType == Resources::ResourceType::HDR)
        {
            if (Texture::TextureExists(event.m_path))
                return;
            Texture::CreateTextureHDRI(event.m_path);
        }
    }

    void OpenGLRenderEngine::OnLoadResourceFromMemory(const Event::ELoadResourceFromMemory& event)
    {
        if (event.m_resourceType == Resources::ResourceType::GLSL)
        {
            if (!Shader::ShaderExists(event.m_path))
            {
                LINA_TRACE("[Shader Loader] -> Loading (memory): {0}", event.m_path);
                ConstructShader(event.m_path, event.m_data, event.m_dataSize);
            }
        }
        else if (event.m_resourceType == Resources::ResourceType::GLH)
        {
            const std::string& name = Utility::GetFileNameOnly(Utility::GetFileWithoutExtension(event.m_path));
            const std::string& text = std::string(reinterpret_cast<char*>(event.m_data), event.m_dataSize);
            Shader::PushShaderInclude(name, text);
        }
        else if (event.m_resourceType == Resources::ResourceType::Model)
        {
            LINA_TRACE("[Model Loader] -> Loading (memory): {0}", event.m_path);

            ModelAssetData assetData;

            if (event.m_assetDataBuffer != nullptr)
                assetData = Model::LoadAssetDataFromMemory(event.m_assetDataBuffer, event.m_assetDataSize);

            Model::CreateModel(event.m_path, event.m_assetDataPath, event.m_data, event.m_dataSize, assetData);
        }
        else if (event.m_resourceType == Resources::ResourceType::Material)
        {
            LINA_TRACE("[Material Loader] -> Loading (memory): {0}", event.m_path);
            Material::LoadMaterialFromMemory(event.m_path, event.m_data, event.m_dataSize);

            if (m_defaultLit == nullptr && event.m_path.compare("Resources/Engine/Materials/DefaultLit.mat") == 0)
                m_defaultLit = &Material::GetMaterial("Resources/Engine/Materials/DefaultLit.mat");
            if (m_defaultUnlit == nullptr && event.m_path.compare("Resources/Engine/Materials/DefaultUnlit.mat") == 0)
                m_defaultUnlit = &Material::GetMaterial("Resources/Engine/Materials/DefaultUnlit.mat");
        }
        else if (event.m_resourceType == Resources::ResourceType::Image)
        {
            LINA_TRACE("[Texture Loader] -> Loading (memory): {0}", event.m_path);

            ImageAssetData assetData;

            if (event.m_assetDataBuffer != nullptr)
                assetData = Texture::LoadAssetDataFromMemory(event.m_assetDataBuffer, event.m_assetDataSize);

            Texture::CreateTexture2D(event.m_path, event.m_assetDataPath, event.m_data, event.m_dataSize, assetData.m_samplerParameters, false, false);
        }
        else if (event.m_resourceType == Resources::ResourceType::HDR)
        {
            Texture::CreateTextureHDRI(event.m_path, event.m_data, event.m_dataSize);
        }
    }

    void OpenGLRenderEngine::OnDrawLine(const Event::EDrawLine& event)
    {
        DrawLine(event.m_from, event.m_to, event.m_color, event.m_lineWidth);
    }

    void OpenGLRenderEngine::OnDrawBox(const Event::EDrawBox& event)
    {
        const Vector3 pos         = event.m_position;
        const Vector3 halfExtents = event.m_halfExtents;

        const Vector3 bottomLB = pos - halfExtents;
        const Vector3 bottomLF = pos + Vector3(-halfExtents.x, -halfExtents.y, halfExtents.z);
        const Vector3 bottomRB = pos + Vector3(halfExtents.x, -halfExtents.y, -halfExtents.z);
        const Vector3 bottomRF = pos + Vector3(halfExtents.x, -halfExtents.y, halfExtents.z);

        const Vector3 topLB = pos + Vector3(-halfExtents.x, halfExtents.y, -halfExtents.z);
        const Vector3 topLF = pos + Vector3(-halfExtents.x, halfExtents.y, halfExtents.z);
        const Vector3 topRB = pos + Vector3(halfExtents.x, halfExtents.y, -halfExtents.z);
        const Vector3 topRF = pos + Vector3(halfExtents.x, halfExtents.y, halfExtents.z);

        std::vector<Vector3> lines{bottomLB, bottomLF, bottomLF, bottomRF, bottomRF, bottomRB, bottomRB, bottomLB, topLB, topLF, topLF, topRF, topRF, topRB, topRB, topLB, bottomLB, topLB, bottomLF, topLF, bottomRF, topRF, bottomRB, topRB};

        for (int i = 0; i < lines.size(); i += 2)
            DrawLine(lines[i], lines[i + 1], event.m_color, event.m_lineWidth);

        lines.clear();
    }

    void OpenGLRenderEngine::OnDrawCircle(const Event::EDrawCircle& event)
    {
        Vector3          previousPos = (event.m_radius * Vector3(1, 0, 0));
        const Quaternion rot         = event.m_rotation;

        std::vector<std::pair<Vector3, Vector3>> lines;

        const float end = event.m_half ? 200.0f : 380.0f;
        for (float angle = 20.0f; angle < end; angle += 20.0f)
        {
            float         radians = Math::ToRadians(angle);
            float         x       = event.m_radius * Math::Cos(radians);
            float         y       = event.m_radius * Math::Sin(radians);
            const Vector3 pos     = Vector3(x, 0, y);
            const Vector3 start   = rot * previousPos;
            const Vector3 target  = rot * pos;
            lines.push_back(std::make_pair(start, target));
            previousPos = pos;
        }

        for (auto& l : lines)
            DrawLine(event.m_position + l.first, event.m_position + l.second, event.m_color, event.m_lineWidth);
    }

    void OpenGLRenderEngine::OnDrawSphere(const Event::EDrawSphere& event)
    {
        OnDrawCircle(Event::EDrawCircle{event.m_position, event.m_radius, event.m_color, event.m_lineWidth, false, Quaternion()});
        OnDrawCircle(Event::EDrawCircle{event.m_position, event.m_radius, event.m_color, event.m_lineWidth, false, Quaternion(Vector3(0, 0, 1), 90)});
        OnDrawCircle(Event::EDrawCircle{event.m_position, event.m_radius, event.m_color, event.m_lineWidth, false, Quaternion(Vector3(1, 0, 0), 90)});
    }

    void OpenGLRenderEngine::OnDrawHemiSphere(const Event::EDrawHemiSphere& event)
    {
        Quaternion q1 = Quaternion(Vector3(1, 0, 0), event.m_top ? -90.0f : 90.0f);
        Quaternion q2 = q1 * Quaternion(Vector3(0, 0, 1), 90);
        OnDrawCircle(Event::EDrawCircle{event.m_position, event.m_radius, event.m_color, event.m_lineWidth, false, Quaternion()});
        OnDrawCircle(Event::EDrawCircle{event.m_position, event.m_radius, event.m_color, event.m_lineWidth, true, q1});
        OnDrawCircle(Event::EDrawCircle{event.m_position, event.m_radius, event.m_color, event.m_lineWidth, true, q2});
    }

    void OpenGLRenderEngine::OnDrawCapsule(const Event::EDrawCapsule& event)
    {
        const Vector3 pos    = event.m_position;
        const float   rad    = event.m_radius;
        const float   height = event.m_height;
        OnDrawHemiSphere(Event::EDrawHemiSphere{pos + Vector3(0, height, 0), rad, event.m_color, event.m_lineWidth, true});
        OnDrawHemiSphere(Event::EDrawHemiSphere{pos - Vector3(0, height, 0), rad, event.m_color, event.m_lineWidth, false});
        DrawLine(pos + Vector3(-rad, -height, 0.0f), pos + Vector3(-rad, height, 0.0f), event.m_color, event.m_lineWidth);
        DrawLine(pos + Vector3(rad, -height, 0.0f), pos + Vector3(rad, height, 0.0f), event.m_color, event.m_lineWidth);
        DrawLine(pos + Vector3(0.0f, -height, -rad), pos + Vector3(0.0f, height, -rad), event.m_color, event.m_lineWidth);
        DrawLine(pos + Vector3(0.0f, -height, rad), pos + Vector3(0.0f, height, rad), event.m_color, event.m_lineWidth);
    }

    void OpenGLRenderEngine::OnWindowResized(const Event::EWindowResized& event)
    {
        SetScreenDisplay(Vector2ui(0, 0), Vector2((float)event.m_windowProps.m_width, (float)event.m_windowProps.m_height));
    }

    void OpenGLRenderEngine::DumpMemory()
    {
        while (!m_debugLineQueue.empty())
        {
            m_debugLineQueue.pop();
        }

        while (!m_debugIconQueue.empty())
        {
            m_debugIconQueue.pop();
        }

        // Clear dumps.
        Model::UnloadAll();
        Texture::UnloadAll();
        Material::UnloadAll();
        Shader::UnloadAll();
    }

    void OpenGLRenderEngine::Draw()
    {
        UpdateSystems(0.0f);

        // Set render targets for point light shadows & calculate all the depth textures.
        auto& tuple = m_lightingSystem.GetPointLights();
        for (int i = 0; i < m_lightingSystem.GetPointLights().size(); i++)
        {
            if (std::get<1>(tuple[i])->m_castsShadows)
            {
                Vector3 lightPos  = std::get<0>(tuple[i])->GetLocation();
                float   farPlane  = std::get<1>(tuple[i])->m_shadowFar;
                float   nearPlane = std::get<1>(tuple[i])->m_shadowNear;

                std::vector<Matrix> shadowTransforms = m_lightingSystem.GetPointLightMatrices(lightPos, m_pLightShadowResolution, nearPlane, farPlane);

                // Set render target
                m_renderDevice.SetFBO(m_pLightShadowTargets[i].GetID());
                m_renderDevice.SetViewport(Vector2::Zero, m_pLightShadowResolution);

                // Clear color bit.
                m_renderDevice.Clear(false, true, false, Color::White, 0xFF);

                // Update depth shader data.
                m_pLightShadowDepthMaterial.SetVector3(UF_LIGHTPOS, lightPos);
                m_pLightShadowDepthMaterial.SetFloat(UF_LIGHTFARPLANE, farPlane);

                for (unsigned int j = 0; j < 6; j++)
                    m_pLightShadowDepthMaterial.SetMatrix4(std::string(UF_SHADOWMATRICES) + "[" + std::to_string(j) + "]", shadowTransforms[j]);

                // Draw scene
                DrawSceneObjects(m_shadowMapDrawParams, &m_pLightShadowDepthMaterial, false);
            }
        }

        m_renderDevice.SetFBO(m_primaryMSAATarget.GetID());
        m_renderDevice.SetViewport(Vector2::Zero, m_screenSize);

        if (!Event::EventSystem::Get()->IsEmpty<Event::ECustomRender>())
            Event::EventSystem::Get()->Trigger<Event::ECustomRender>(Event::ECustomRender{});
        else
        {
            m_renderDevice.Clear(true, true, true, m_cameraSystem.GetCurrentClearColor(), 0xFF);
            DrawSkybox();
            DrawSceneObjects(m_defaultDrawParams);
        }

        // Draw debugs
        ProcessDebugQueue();

        // Finalize drawing.
        DrawFinalize();
    }

    void OpenGLRenderEngine::DrawFinalize()
    {
        // Frag color
        m_renderDevice.BlitRenderTargets(m_primaryMSAATarget.GetID(), m_screenSize.x, m_screenSize.y, m_primaryRenderTarget.GetID(), m_screenSize.x, m_screenSize.y, BufferBit::BIT_COLOR, SamplerFilter::FILTER_NEAREST, FrameBufferAttachment::ATTACHMENT_COLOR, (uint32)0);

        // Bright color - HDR
        m_renderDevice.BlitRenderTargets(m_primaryMSAATarget.GetID(), m_screenSize.x, m_screenSize.y, m_primaryRenderTarget.GetID(), m_screenSize.x, m_screenSize.y, BufferBit::BIT_COLOR, SamplerFilter::FILTER_NEAREST, FrameBufferAttachment::ATTACHMENT_COLOR, (uint32)1);

        // Below we process bloom post fx based on brightColor output in the shaders.
        bool horizontal = true;
        if (m_renderSettings.m_bloomEnabled)
        {
            // Write to the pingpong buffers to apply 2 pass gaussian blur.
            bool         firstIteration = true;
            unsigned int amount         = 10;
            for (unsigned int i = 0; i < amount; i++)
            {
                // Select FBO
                m_renderDevice.SetFBO(horizontal ? m_pingPongRenderTarget1.GetID() : m_pingPongRenderTarget2.GetID());

                // Setup material & use.
                m_screenQuadBlurMaterial.SetBool(MAT_ISHORIZONTAL, horizontal);
                if (firstIteration)
                    m_screenQuadBlurMaterial.SetTexture(MAT_MAP_SCREEN, &m_primaryRTTexture1);
                else
                {
                    if (horizontal)
                        m_screenQuadBlurMaterial.SetTexture(MAT_MAP_SCREEN, &m_pingPongRTTexture2);
                    else
                        m_screenQuadBlurMaterial.SetTexture(MAT_MAP_SCREEN, &m_pingPongRTTexture1);
                }

                // Update shader data & draw.
                UpdateShaderData(&m_screenQuadBlurMaterial);
                m_renderDevice.Draw(m_screenQuadVAO, m_fullscreenQuadDP, 0, 6, true);
                horizontal = !horizontal;
                if (firstIteration)
                    firstIteration = false;
            }
        }

        // Take the final image (without bloom), run it through a custom post processing pipeline.
        Texture* screenMap = &m_primaryRTTexture0;
        for (auto& pp : m_postProcessMap)
        {
            pp.second.Draw(screenMap);
            screenMap = &pp.second.GetTexture();
        }

        // After we've applied custom post processing, draw the final image either to the screen, or to a secondary frame buffer to display it in editor.
        if (m_appMode == ApplicationMode::Editor)
            m_renderDevice.SetFBO(m_secondaryRenderTarget.GetID());
        else
            // Back to default buffer
            m_renderDevice.SetFBO(0);

        m_renderDevice.SetViewport(m_screenPos, m_screenSize);
        m_renderDevice.Clear(true, true, true, Color::White, 0xFF);

        // Set frame buffer texture on the material.
        m_screenQuadFinalMaterial.SetTexture(MAT_MAP_SCREEN, screenMap, TextureBindMode::BINDTEXTURE_TEXTURE2D);

        if (m_screenQuadFinalMaterial.m_bools[MAT_BLOOMENABLED])
            m_screenQuadFinalMaterial.SetTexture(MAT_MAP_BLOOM, horizontal ? &m_pingPongRTTexture1 : &m_pingPongRTTexture2, TextureBindMode::BINDTEXTURE_TEXTURE2D);

        Vector2 inverseMapSize = 1.0f / Vector2((float)m_primaryRTTexture0.GetSize().x, (float)(m_primaryRTTexture0.GetSize().y));
        m_screenQuadFinalMaterial.SetVector3(MAT_INVERSESCREENMAPSIZE, Vector3(inverseMapSize.x, inverseMapSize.y, 0.0));

        UpdateShaderData(&m_screenQuadFinalMaterial);
        m_renderDevice.Draw(m_screenQuadVAO, m_fullscreenQuadDP, 0, 6, true);
    }

    void OpenGLRenderEngine::DrawIcon(Vector3 position, StringIDType textureID, float size)
    {
        m_debugIconQueue.push(DebugIcon{position, textureID, size});
    }

    void OpenGLRenderEngine::DrawLine(Vector3 p1, Vector3 p2, Color col, float width)
    {
        m_debugLineQueue.push(DebugLine{p1, p2, col, width});
    }

    void OpenGLRenderEngine::ProcessDebugQueue()
    {
        while (!m_debugLineQueue.empty())
        {
            DebugLine line = m_debugLineQueue.front();
            m_renderDevice.SetShader(m_debugLineMaterial.m_shaderID);
            m_renderDevice.UpdateShaderUniformColor(m_debugLineMaterial.m_shaderID, MAT_COLOR, line.m_color);
            m_renderDevice.DrawLine(m_debugLineMaterial.m_shaderID, Matrix::Identity(), line.m_from, line.m_to, line.m_width);
            m_debugLineQueue.pop();
        }

        while (!m_debugIconQueue.empty())
        {
            DebugIcon      icon = m_debugIconQueue.front();
            Transformation tr;
            tr.m_location = icon.m_center;
            tr.m_scale    = Vector3(icon.m_size);
            tr.m_rotation = Quaternion::LookAt(icon.m_center, m_cameraSystem.GetCameraLocation(), Vector3::Up);
            Matrix model  = tr.ToMatrix();
            m_quadMesh.GetVertexArray().UpdateBuffer(2, &model, 1 * sizeof(Matrix));
            m_debugIconMaterial.SetTexture(MAT_TEXTURE2D_DIFFUSE, &Texture::GetTexture(icon.m_textureID));
            UpdateShaderData(&m_debugIconMaterial);
            m_renderDevice.Draw(m_quadMesh.GetVertexArray().GetID(), m_defaultDrawParams, 1, m_quadMesh.GetVertexArray().GetIndexCount(), false);
            m_debugIconQueue.pop();
        }
    }

    void OpenGLRenderEngine::SetDrawParameters(const DrawParams& params)
    {
        m_renderDevice.SetDrawParameters(params);
    }

    void OpenGLRenderEngine::UpdateRenderSettings()
    {
        m_screenQuadFinalMaterial.SetBool(MAT_FXAAENABLED, m_renderSettings.m_fxaaEnabled);
        m_screenQuadFinalMaterial.SetBool(MAT_BLOOMENABLED, m_renderSettings.m_bloomEnabled);
        m_screenQuadFinalMaterial.SetBool(MAT_VIGNETTEENABLED, m_renderSettings.m_vignetteEnabled);
        m_screenQuadFinalMaterial.SetFloat(MAT_FXAAREDUCEMIN, m_renderSettings.m_fxaaReduceMin);
        m_screenQuadFinalMaterial.SetFloat(MAT_FXAAREDUCEMUL, m_renderSettings.m_fxaaReduceMul);
        m_screenQuadFinalMaterial.SetFloat(MAT_FXAASPANMAX, m_renderSettings.m_fxaaSpanMax);
        m_screenQuadFinalMaterial.SetFloat(MAT_GAMMA, m_renderSettings.m_gamma);
        m_screenQuadFinalMaterial.SetFloat(MAT_EXPOSURE, m_renderSettings.m_exposure);
        m_screenQuadFinalMaterial.SetFloat(MAT_VIGNETTEAMOUNT, m_renderSettings.m_vignetteAmount);
        m_screenQuadFinalMaterial.SetFloat(MAT_VIGNETTEPOW, m_renderSettings.m_vignettePow);
    }

    void OpenGLRenderEngine::DrawSkybox()
    {
        if (m_skyboxMaterial != nullptr)
        {
            UpdateShaderData(m_skyboxMaterial);
            m_renderDevice.Draw(m_skyboxVAO, m_skyboxDrawParams, 1, 4, true);
        }
        else
        {
            UpdateShaderData(&m_defaultSkyboxMaterial);
            m_renderDevice.Draw(m_skyboxVAO, m_skyboxDrawParams, 1, 4, true);
        }
    }

    PostProcessEffect& OpenGLRenderEngine::AddPostProcessEffect(Shader& shader)
    {
        if (m_postProcessMap.find(&shader) == m_postProcessMap.end())
        {
            m_postProcessMap[&shader] = PostProcessEffect();
            m_postProcessMap[&shader].Construct(shader);
        }

        return m_postProcessMap[&shader];
    }

    void OpenGLRenderEngine::DrawSceneObjects(DrawParams& drawParams, Material* overrideMaterial, bool completeFlush)
    {
        m_modelNodeSystem.FlushOpaque(drawParams, overrideMaterial, completeFlush);
        m_modelNodeSystem.FlushTransparent(drawParams, overrideMaterial, completeFlush);
        m_spriteRendererSystem.Flush(drawParams, overrideMaterial, completeFlush);
        Event::EventSystem::Get()->Trigger<Event::EPostSceneDraw>(Event::EPostSceneDraw{});
    }

    void OpenGLRenderEngine::UpdateUniformBuffers()
    {
        Vector3 cameraLocation = m_cameraSystem.GetCameraLocation();
        Vector4 viewPos        = Vector4(cameraLocation.x, cameraLocation.y, cameraLocation.z, 1.0f);

        // Update global matrix buffer
        uintptr currentGlobalDataOffset = 0;

        m_globalDataBuffer.Update(&m_cameraSystem.GetProjectionMatrix()[0][0], currentGlobalDataOffset, sizeof(Matrix));
        currentGlobalDataOffset += sizeof(Matrix);

        m_globalDataBuffer.Update(&m_cameraSystem.GetViewMatrix()[0][0], currentGlobalDataOffset, sizeof(Matrix));
        currentGlobalDataOffset += sizeof(Matrix);

        m_globalDataBuffer.Update(&m_lightingSystem.GetDirectionalLightMatrix()[0][0], currentGlobalDataOffset, sizeof(Matrix));
        currentGlobalDataOffset += sizeof(Matrix);

        Matrix VP = m_cameraSystem.GetProjectionMatrix() * m_cameraSystem.GetViewMatrix();
        m_globalDataBuffer.Update(&VP[0][0], currentGlobalDataOffset, sizeof(Matrix));
        currentGlobalDataOffset += sizeof(Matrix);

        m_globalDataBuffer.Update(&viewPos, currentGlobalDataOffset, sizeof(Vector4));
        currentGlobalDataOffset += sizeof(Vector4);

        ECS::CameraComponent* cameraComponent = m_cameraSystem.GetActiveCameraComponent();

        if (cameraComponent != nullptr)
        {
            // Update only if changed.
            if (m_bufferValueRecord.zNear != cameraComponent->m_zNear)
            {
                m_bufferValueRecord.zNear = cameraComponent->m_zNear;
                m_globalDataBuffer.Update(&cameraComponent->m_zNear, currentGlobalDataOffset, sizeof(float));
            }
            currentGlobalDataOffset += sizeof(float);

            // Update only if changed.
            if (m_bufferValueRecord.zFar != cameraComponent->m_zFar)
            {
                m_bufferValueRecord.zFar = cameraComponent->m_zFar;
                m_globalDataBuffer.Update(&cameraComponent->m_zNear, currentGlobalDataOffset, sizeof(float));
            }
            currentGlobalDataOffset += sizeof(float);
        }

        // Update lights buffer.
        Color   color        = m_lightingSystem.GetAmbientColor();
        Vector4 ambientColor = Vector4(color.r, color.g, color.b, 1.0f);
        m_globalLightBuffer.Update(&ambientColor, 0, sizeof(float) * 4);
        m_globalLightBuffer.Update(&m_currentPointLightCount, sizeof(float) * 4, sizeof(int));
        m_globalLightBuffer.Update(&m_currentSpotLightCount, sizeof(float) * 4 + sizeof(int), sizeof(int));

        // Update debug fufer.
        m_globalDebugBuffer.Update(&m_debugData.visualizeDepth, 0, sizeof(bool));
    }

    void OpenGLRenderEngine::UpdateShaderData(Material* data)
    {

        m_renderDevice.SetShader(data->GetShaderID());

        for (auto const& d : (*data).m_floats)
            m_renderDevice.UpdateShaderUniformFloat(data->m_shaderID, d.first, d.second);

        for (auto const& d : (*data).m_bools)
            m_renderDevice.UpdateShaderUniformInt(data->m_shaderID, d.first, d.second);

        for (auto const& d : (*data).m_colors)
            m_renderDevice.UpdateShaderUniformColor(data->m_shaderID, d.first, d.second);

        for (auto const& d : (*data).m_ints)
            m_renderDevice.UpdateShaderUniformInt(data->m_shaderID, d.first, d.second);

        for (auto const& d : (*data).m_vector2s)
            m_renderDevice.UpdateShaderUniformVector2(data->m_shaderID, d.first, d.second);

        for (auto const& d : (*data).m_vector3s)
            m_renderDevice.UpdateShaderUniformVector3(data->m_shaderID, d.first, d.second);

        for (auto const& d : (*data).m_vector4s)
            m_renderDevice.UpdateShaderUniformVector4F(data->m_shaderID, d.first, d.second);

        for (auto const& d : (*data).m_matrices)
            m_renderDevice.UpdateShaderUniformMatrix(data->m_shaderID, d.first, d.second);

        // Set material's shadow textures to the FBO textures.
        if (data->m_isPBR)
        {
            auto& tuple = m_lightingSystem.GetPointLights();

            for (int i = 0; i < m_lightingSystem.GetPointLights().size(); i++)
            {
                std::string textureName = std::string(MAT_MAPS_SHADOWDEPTH) + "[" + std::to_string(i) + "]";
                if (std::get<1>(tuple[i])->m_castsShadows)
                    data->SetTexture(textureName, &m_pLightShadowTextures[i], TextureBindMode::BINDTEXTURE_CUBEMAP);
                else
                    data->RemoveTexture(textureName);
            }
        }

        for (auto const& d : (*data).m_sampler2Ds)
        {
            // Set whether the texture is active or not.
            bool isActive = (d.second.m_isActive && d.second.m_boundTexture != nullptr && !d.second.m_boundTexture->GetIsEmpty()) ? true : false;
            m_renderDevice.UpdateShaderUniformInt(data->m_shaderID, d.first + MAT_EXTENSION_ISACTIVE, isActive);

            // Set the texture to corresponding active unit.
            m_renderDevice.UpdateShaderUniformInt(data->m_shaderID, d.first + MAT_EXTENSION_TEXTURE2D, d.second.m_unit);

            // Set texture
            if (isActive)
                m_renderDevice.SetTexture(d.second.m_boundTexture->GetID(), d.second.m_boundTexture->GetSamplerID(), d.second.m_unit, d.second.m_bindMode, true);
            else
            {

                if (d.second.m_bindMode == TextureBindMode::BINDTEXTURE_TEXTURE2D)
                    m_renderDevice.SetTexture(m_defaultTexture.GetID(), m_defaultTexture.GetSamplerID(), d.second.m_unit, BINDTEXTURE_TEXTURE2D);
                else
                    m_renderDevice.SetTexture(m_defaultCubemapTexture.GetID(), m_defaultCubemapTexture.GetSamplerID(), d.second.m_unit, BINDTEXTURE_CUBEMAP);
            }
        }

        if (data->m_isPBR)
            m_lightingSystem.SetLightingShaderData(data->GetShaderID());

        if (!m_firstFrameDrawn)
        {
            m_renderDevice.ValidateShaderProgram(data->GetShaderID());
        }
    }

    void OpenGLRenderEngine::CaptureCalculateHDRI(Texture& hdriTexture)
    {
        // Build projection & view matrices for capturing HDRI data.
        Matrix captureProjection = Matrix::PerspectiveRH(90.0f, 1.0f, 0.1f, 10.0f);
        Matrix captureViews[]    = {Matrix::InitLookAtRH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)), Matrix::InitLookAtRH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
                                 Matrix::InitLookAtRH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),  Matrix::InitLookAtRH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
                                 Matrix::InitLookAtRH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)), Matrix::InitLookAtRH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))};

        // Calculate HDRI, Irradiance, Prefilter and BRDF
        CalculateHDRICubemap(hdriTexture, captureProjection, captureViews);
        CalculateHDRIIrradiance(captureProjection, captureViews);
        CalculateHDRIPrefilter(captureProjection, captureViews);
        CalculateHDRIBRDF(captureProjection, captureViews);
        m_renderDevice.SetFBO(0);
        m_renderDevice.SetViewport(m_screenPos, m_screenSize);

        // Set flag
        m_hdriDataCaptured = true;

        std::set<Material*>& hdriMaterials = Material::GetHDRIMaterials();
        for (Material* mat : hdriMaterials)
        {
            if (mat != nullptr)
                SetHDRIData(mat);
        }
    }

    void OpenGLRenderEngine::CalculateHDRICubemap(Texture& hdriTexture, glm::mat4& captureProjection, glm::mat4 views[6])
    {
        // Generate sampler.
        SamplerParameters samplerParams;
        samplerParams.m_textureParams.m_wrapR = samplerParams.m_textureParams.m_wrapS = samplerParams.m_textureParams.m_wrapT = SamplerWrapMode::WRAP_CLAMP_EDGE;
        samplerParams.m_textureParams.m_magFilter                                                                             = SamplerFilter::FILTER_LINEAR;
        samplerParams.m_textureParams.m_minFilter                                                                             = SamplerFilter::FILTER_LINEAR_MIPMAP_LINEAR;
        samplerParams.m_textureParams.m_internalPixelFormat                                                                   = PixelFormat::FORMAT_RGB16F;
        samplerParams.m_textureParams.m_pixelFormat                                                                           = PixelFormat::FORMAT_RGB;

        // Set resolution.
        m_hdriResolution = Vector2(512, 512);

        // Construct Cubemap texture.
        m_hdriCubemap.ConstructRTCubemapTexture(m_hdriResolution, samplerParams);

        // Setup shader data.
        uint32 equirectangularShader = m_hdriEquirectangularShader->GetID();
        m_renderDevice.SetShader(equirectangularShader);
        m_renderDevice.UpdateShaderUniformInt(equirectangularShader, MAT_MAP_EQUIRECTANGULAR + std::string(MAT_EXTENSION_TEXTURE2D), 0);
        m_renderDevice.UpdateShaderUniformInt(equirectangularShader, MAT_MAP_EQUIRECTANGULAR + std::string(MAT_EXTENSION_ISACTIVE), 1);
        m_renderDevice.UpdateShaderUniformMatrix(equirectangularShader, UF_MATRIX_PROJECTION, captureProjection);
        m_renderDevice.SetTexture(hdriTexture.GetID(), hdriTexture.GetSamplerID(), 0);
        m_renderDevice.SetFBO(m_hdriCaptureRenderTarget.GetID());
        m_renderDevice.SetViewport(Vector2::Zero, m_hdriResolution);

        // Draw the cubemap.
        for (uint32 i = 0; i < 6; ++i)
        {
            m_renderDevice.UpdateShaderUniformMatrix(equirectangularShader, UF_MATRIX_VIEW, views[i]);
            m_renderDevice.BindTextureToRenderTarget(m_hdriCaptureRenderTarget.GetID(), m_hdriCubemap.GetID(), TextureBindMode::BINDTEXTURE_CUBEMAP_POSITIVE_X, FrameBufferAttachment::ATTACHMENT_COLOR, 0, i, 0, false);
            m_renderDevice.SetFBO(m_hdriCaptureRenderTarget.GetID());
            m_renderDevice.Clear(true, true, true, m_cameraSystem.GetCurrentClearColor(), 0xFF);
            m_renderDevice.Draw(m_hdriCubeVAO, m_defaultDrawParams, 0, 36, true);
        }

        // Generate mipmaps & check errors.
        m_renderDevice.GenerateTextureMipmaps(m_hdriCubemap.GetID(), TextureBindMode::BINDTEXTURE_CUBEMAP);
        m_renderDevice.IsRenderTargetComplete(m_hdriCaptureRenderTarget.GetID());
    }

    void OpenGLRenderEngine::CalculateHDRIIrradiance(Matrix& captureProjection, Matrix views[6])
    {
        // Generate sampler.
        SamplerParameters irradianceParams;
        irradianceParams.m_textureParams.m_wrapR = irradianceParams.m_textureParams.m_wrapS = irradianceParams.m_textureParams.m_wrapT = SamplerWrapMode::WRAP_CLAMP_EDGE;
        irradianceParams.m_textureParams.m_magFilter                                                                                   = SamplerFilter::FILTER_LINEAR;
        irradianceParams.m_textureParams.m_minFilter                                                                                   = SamplerFilter::FILTER_LINEAR_MIPMAP_LINEAR;
        irradianceParams.m_textureParams.m_internalPixelFormat                                                                         = PixelFormat::FORMAT_RGB16F;
        irradianceParams.m_textureParams.m_pixelFormat                                                                                 = PixelFormat::FORMAT_RGB;

        // Set resolution
        Vector2 irradianceMapResolsution = Vector2(32, 32);

        // Build irradiance texture & scale render buffer according to the resolution.
        m_hdriIrradianceMap.ConstructRTCubemapTexture(irradianceMapResolsution, irradianceParams);
        m_renderDevice.SetFBO(m_hdriCaptureRenderTarget.GetID());
        m_renderDevice.ResizeRenderBuffer(m_hdriCaptureRenderTarget.GetID(), m_hdriCaptureRenderBuffer.GetID(), irradianceMapResolsution, RenderBufferStorage::STORAGE_DEPTH_COMP24);

        // Build & setup shader info.
        uint32 irradianceShader = m_hdriIrradianceShader->GetID();
        m_renderDevice.SetShader(irradianceShader);
        m_renderDevice.UpdateShaderUniformInt(irradianceShader, MAT_MAP_ENVIRONMENT + std::string(MAT_EXTENSION_TEXTURE2D), 0);
        m_renderDevice.UpdateShaderUniformInt(irradianceShader, MAT_MAP_ENVIRONMENT + std::string(MAT_EXTENSION_ISACTIVE), 1);
        m_renderDevice.UpdateShaderUniformMatrix(irradianceShader, UF_MATRIX_PROJECTION, captureProjection);
        m_renderDevice.SetTexture(m_hdriCubemap.GetID(), m_hdriCubemap.GetSamplerID(), 0, TextureBindMode::BINDTEXTURE_CUBEMAP);
        m_renderDevice.SetViewport(Vector2::Zero, irradianceMapResolsution);

        // Draw cubemap.
        for (uint32 i = 0; i < 6; ++i)
        {
            m_renderDevice.UpdateShaderUniformMatrix(irradianceShader, UF_MATRIX_VIEW, views[i]);
            m_renderDevice.BindTextureToRenderTarget(m_hdriCaptureRenderTarget.GetID(), m_hdriIrradianceMap.GetID(), TextureBindMode::BINDTEXTURE_CUBEMAP_POSITIVE_X, FrameBufferAttachment::ATTACHMENT_COLOR, 0, i, 0, false, false);
            m_renderDevice.Clear(true, true, true, m_cameraSystem.GetCurrentClearColor(), 0xFF);
            m_renderDevice.Draw(m_hdriCubeVAO, m_defaultDrawParams, 0, 36, true);
        }
    }

    void OpenGLRenderEngine::CalculateHDRIPrefilter(Matrix& captureProjection, Matrix views[6])
    {
        // Generate sampler.
        SamplerParameters prefilterParams;
        prefilterParams.m_textureParams.m_generateMipMaps = true;
        prefilterParams.m_textureParams.m_wrapR = prefilterParams.m_textureParams.m_wrapS = prefilterParams.m_textureParams.m_wrapT = SamplerWrapMode::WRAP_CLAMP_EDGE;
        prefilterParams.m_textureParams.m_minFilter                                                                                 = SamplerFilter::FILTER_LINEAR_MIPMAP_LINEAR;
        prefilterParams.m_textureParams.m_magFilter                                                                                 = SamplerFilter::FILTER_LINEAR;
        prefilterParams.m_textureParams.m_internalPixelFormat                                                                       = PixelFormat::FORMAT_RGB16F;
        prefilterParams.m_textureParams.m_pixelFormat                                                                               = PixelFormat::FORMAT_RGB;

        // Set resolution
        Vector2 prefilterResolution = Vector2(128, 128);

        // Construct prefilter texture.
        m_hdriPrefilterMap.ConstructRTCubemapTexture(prefilterResolution, prefilterParams);

        // Setup shader data.
        uint32 prefilterShader = m_hdriPrefilterShader->GetID();
        m_renderDevice.SetShader(prefilterShader);
        m_renderDevice.UpdateShaderUniformInt(prefilterShader, MAT_MAP_ENVIRONMENT + std::string(MAT_EXTENSION_TEXTURE2D), 0);
        m_renderDevice.UpdateShaderUniformInt(prefilterShader, MAT_MAP_ENVIRONMENT + std::string(MAT_EXTENSION_ISACTIVE), 1);
        m_renderDevice.UpdateShaderUniformFloat(prefilterShader, MAT_ENVIRONMENTRESOLUTION, 512.0f);
        m_renderDevice.UpdateShaderUniformMatrix(prefilterShader, UF_MATRIX_PROJECTION, captureProjection);
        m_renderDevice.SetTexture(m_hdriCubemap.GetID(), m_hdriCubemap.GetSamplerID(), 0, TextureBindMode::BINDTEXTURE_CUBEMAP);

        // Setup mip levels & switch fbo.
        uint32 maxMipLevels = 5;
        m_renderDevice.SetFBO(m_hdriCaptureRenderTarget.GetID());

        for (uint32 mip = 0; mip < maxMipLevels; ++mip)
        {
            // reisze framebuffer according to mip-level size.
            unsigned int mipWidth  = (unsigned int)(128 * std::pow(0.5, mip));
            unsigned int mipHeight = (unsigned int)(128 * std::pow(0.5, mip));
            m_renderDevice.ResizeRenderBuffer(m_hdriCaptureRenderTarget.GetID(), m_hdriCaptureRenderBuffer.GetID(), Vector2ui((int)mipWidth, (int)mipHeight), RenderBufferStorage::STORAGE_DEPTH_COMP24);
            m_renderDevice.SetViewport(Vector2ui(0, 0), Vector2ui((int)mipWidth, (int)mipHeight));

            // Draw prefiltered map
            float roughness = (float)mip / (float)(maxMipLevels - 1);
            m_renderDevice.UpdateShaderUniformFloat(prefilterShader, MAT_ROUGHNESSMULTIPLIER, roughness);
            for (unsigned int i = 0; i < 6; ++i)
            {
                m_renderDevice.UpdateShaderUniformMatrix(prefilterShader, UF_MATRIX_VIEW, views[i]);
                m_renderDevice.BindTextureToRenderTarget(m_hdriCaptureRenderTarget.GetID(), m_hdriPrefilterMap.GetID(), TextureBindMode::BINDTEXTURE_CUBEMAP_POSITIVE_X, FrameBufferAttachment::ATTACHMENT_COLOR, 0, i, mip, false, false);
                m_renderDevice.Clear(true, true, true, m_cameraSystem.GetCurrentClearColor(), 0xFF);
                m_renderDevice.Draw(m_hdriCubeVAO, m_defaultDrawParams, 0, 36, true);
            }
        }
    }

    void OpenGLRenderEngine::CalculateHDRIBRDF(Matrix& captureProjection, Matrix views[6])
    {
        // Generate sampler.
        SamplerParameters samplerParams;
        samplerParams.m_textureParams.m_wrapR = samplerParams.m_textureParams.m_wrapS = samplerParams.m_textureParams.m_wrapT = SamplerWrapMode::WRAP_CLAMP_EDGE;
        samplerParams.m_textureParams.m_magFilter                                                                             = SamplerFilter::FILTER_LINEAR;
        samplerParams.m_textureParams.m_minFilter                                                                             = SamplerFilter::FILTER_LINEAR;
        samplerParams.m_textureParams.m_internalPixelFormat                                                                   = PixelFormat::FORMAT_RGB16F;
        samplerParams.m_textureParams.m_pixelFormat                                                                           = PixelFormat::FORMAT_RGB;

        // Set resolution.
        Vector2 brdfLutSize = Vector2(512, 512);

        // Build BRDF texture.
        m_HDRILutMap.ConstructHDRI(samplerParams, brdfLutSize, NULL);

        // Scale render buffer according to the resolution & bind lut map to frame buffer.
        m_renderDevice.ResizeRenderBuffer(m_hdriCaptureRenderTarget.GetID(), m_hdriCaptureRenderBuffer.GetID(), brdfLutSize, RenderBufferStorage::STORAGE_DEPTH_COMP24);
        m_renderDevice.BindTextureToRenderTarget(m_hdriCaptureRenderTarget.GetID(), m_HDRILutMap.GetID(), TextureBindMode::BINDTEXTURE_TEXTURE2D, FrameBufferAttachment::ATTACHMENT_COLOR, 0, 0, 0, true, false);

        // Setup shader.
        uint32 brdfShader = m_hdriBRDFShader->GetID();
        m_renderDevice.SetShader(brdfShader);

        // Switch framebuffer & draw.
        m_renderDevice.SetFBO(m_hdriCaptureRenderTarget.GetID());
        m_renderDevice.SetViewport(Vector2::Zero, brdfLutSize);
        m_renderDevice.Clear(true, true, true, m_cameraSystem.GetCurrentClearColor(), 0xFF);
        m_renderDevice.Draw(m_screenQuadVAO, m_fullscreenQuadDP, 0, 6, true);
    }

    void OpenGLRenderEngine::SetHDRIData(Material* mat)
    {
        if (mat == nullptr)
        {
            LINA_WARN("Material to set HDRI data is null, returning...");
            return;
        }

        if (!m_hdriDataCaptured)
        {
            LINA_WARN("HDRI data is not captured, please capture it first then set the material's data.");
            return;
        }

        mat->SetTexture(MAT_TEXTURE2D_IRRADIANCEMAP, &m_hdriIrradianceMap, TextureBindMode::BINDTEXTURE_CUBEMAP);
        mat->SetTexture(MAT_TEXTURE2D_BRDFLUTMAP, &m_HDRILutMap, TextureBindMode::BINDTEXTURE_TEXTURE2D);
        mat->SetTexture(MAT_TEXTURE2D_PREFILTERMAP, &m_hdriPrefilterMap, TextureBindMode::BINDTEXTURE_CUBEMAP);
    }

    void OpenGLRenderEngine::RemoveHDRIData(Material* mat)
    {
        if (mat == nullptr)
        {
            LINA_WARN("Material to remove HDRI data from is null, returning...");
            return;
        }

        mat->RemoveTexture(MAT_TEXTURE2D_IRRADIANCEMAP);
        mat->RemoveTexture(MAT_TEXTURE2D_BRDFLUTMAP);
        mat->RemoveTexture(MAT_TEXTURE2D_PREFILTERMAP);
    }

    uint32 OpenGLRenderEngine::GetFinalImage()
    {
        if (m_appMode == ApplicationMode::Editor)
            return m_secondaryRTTexture.GetID();
        else
            return m_primaryRTTexture0.GetID();
    }

    uint32 OpenGLRenderEngine::GetShadowMapImage()
    {
        return m_shadowMapRTTexture.GetID();
    }

    void OpenGLRenderEngine::UpdateSystems(float delta)
    {
        // Update pipeline.
        m_renderingPipeline.UpdateSystems(delta);

        // Update uniform buffers on GPU
        UpdateUniformBuffers();
    }

    void OpenGLRenderEngine::BindShaderToViewBuffer(Shader& shader)
    {
        shader.BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
    }

    void OpenGLRenderEngine::BindShaderToDebugBuffer(Shader& shader)
    {
        shader.BindBlockToBuffer(UNIFORMBUFFER_DEBUGDATA_BINDPOINT, UNIFORMBUFFER_DEBUGDATA_NAME);
    }

    void OpenGLRenderEngine::BindShaderToLightBuffer(Shader& shader)
    {
        shader.BindBlockToBuffer(UNIFORMBUFFER_LIGHTDATA_BINDPOINT, UNIFORMBUFFER_LIGHTDATA_NAME);
    }

} // namespace Lina::Graphics
