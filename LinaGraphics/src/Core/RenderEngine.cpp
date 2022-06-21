#include "Core/RenderEngine.hpp"

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
#include "Resources/ResourceStorage.hpp"
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
#include "Resources/ResourceStorage.hpp"

namespace Lina::Graphics
{
    RenderEngine* RenderEngine::s_renderEngine = nullptr;

    constexpr size_t UNIFORMBUFFER_VIEWDATA_SIZE      = (sizeof(Matrix) * 4) + (sizeof(Vector4)) + (sizeof(float) * 2);
    constexpr int    UNIFORMBUFFER_VIEWDATA_BINDPOINT = 0;
    constexpr auto   UNIFORMBUFFER_VIEWDATA_NAME      = "ViewData";

    constexpr size_t UNIFORMBUFFER_LIGHTDATA_SIZE      = (sizeof(int) * 2) + sizeof(float) * 4;
    constexpr int    UNIFORMBUFFER_LIGHTDATA_BINDPOINT = 1;
    constexpr auto   UNIFORMBUFFER_LIGHTDATA_NAME      = "LightData";

    constexpr size_t UNIFORMBUFFER_DEBUGDATA_SIZE      = (sizeof(bool) * 1);
    constexpr int    UNIFORMBUFFER_DEBUGDATA_BINDPOINT = 2;
    constexpr auto   UNIFORMBUFFER_DEBUGDATA_NAME      = "DebugData";

    constexpr size_t UNIFORMBUFFER_APPDATA_SIZE      = (sizeof(float) * 2) + (sizeof(Vector2) * 2);
    constexpr int    UNIFORMBUFFER_APPDATA_BINDPOINT = 3;
    constexpr auto   UNIFORMBUFFER_APPDATA_NAME      = "AppData";

    void RenderEngine::ConnectEvents()
    {
        // Flip loaded images.
        ArrayBitmap::SetImageFlip(true);

        m_eventSystem = Event::EventSystem::Get();
        m_eventSystem->Connect<Event::EWindowResized, &RenderEngine::OnWindowResized>(this);
        m_eventSystem->Connect<Event::EDrawLine, &RenderEngine::OnDrawLine>(this);
        m_eventSystem->Connect<Event::EDrawBox, &RenderEngine::OnDrawBox>(this);
        m_eventSystem->Connect<Event::EDrawCircle, &RenderEngine::OnDrawCircle>(this);
        m_eventSystem->Connect<Event::EDrawSphere, &RenderEngine::OnDrawSphere>(this);
        m_eventSystem->Connect<Event::EDrawHemiSphere, &RenderEngine::OnDrawHemiSphere>(this);
        m_eventSystem->Connect<Event::EDrawCapsule, &RenderEngine::OnDrawCapsule>(this);
        m_eventSystem->Connect<Event::EAllResourcesOfTypeLoaded, &RenderEngine::OnAllResourcesOfTypeLoaded>(this);
        m_eventSystem->Connect<Event::EResourceReloaded, &RenderEngine::OnResourceReloaded>(this);
    }

    void RenderEngine::Initialize(ApplicationMode appMode, RenderSettings* renderSettings, const WindowProperties& windowProps)
    {
        LINA_TRACE("[Initialization] -> RenderEngine ({0})", typeid(*this).name());

        // Set references.
        m_appWindow      = Window::Get();
        m_appMode        = appMode;
        m_storage        = Resources::ResourceStorage::Get();
        m_renderSettings = renderSettings;

        // Initialize the render device.
        m_renderDevice.Initialize(m_appWindow->GetWidth(), m_appWindow->GetHeight(), m_defaultDrawParams, windowProps);

        // Setup draw parameters.
        m_defaultDrawParams   = DrawParameterHelper::GetDefault();
        m_skyboxDrawParams    = DrawParameterHelper::GetSkybox();
        m_fullscreenQuadDP    = DrawParameterHelper::GetFullScreenQuad();
        m_shadowMapDrawParams = DrawParameterHelper::GetShadowMap();

        // Construct the uniform buffer for global matrices.
        m_globalViewBuffer.Construct(UNIFORMBUFFER_VIEWDATA_SIZE, BufferUsage::USAGE_DYNAMIC_DRAW, NULL);
        m_globalViewBuffer.Bind(UNIFORMBUFFER_VIEWDATA_BINDPOINT);

        // Construct the uniform buffer for lights.
        m_globalLightBuffer.Construct(UNIFORMBUFFER_LIGHTDATA_SIZE, BufferUsage::USAGE_DYNAMIC_DRAW, NULL);
        m_globalLightBuffer.Bind(UNIFORMBUFFER_LIGHTDATA_BINDPOINT);

        // Construct the uniform buffer for debugging.
        m_globalDebugBuffer.Construct(UNIFORMBUFFER_DEBUGDATA_SIZE, BufferUsage::USAGE_DYNAMIC_DRAW, NULL);
        m_globalDebugBuffer.Bind(UNIFORMBUFFER_DEBUGDATA_BINDPOINT);

        // Construct the uniform buffer for app data.
        m_globalAppDataBuffer.Construct(UNIFORMBUFFER_APPDATA_SIZE, BufferUsage::USAGE_DYNAMIC_DRAW, NULL);
        m_globalAppDataBuffer.Bind(UNIFORMBUFFER_APPDATA_BINDPOINT);

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
        m_reflectionSystem.Initialize("Reflection System", m_appMode);

        // Order is important.
        AddToRenderingPipeline(m_cameraSystem);
        AddToRenderingPipeline(m_frustumSystem);
        AddToRenderingPipeline(m_reflectionSystem);
        AddToRenderingPipeline(m_modelNodeSystem);
        AddToRenderingPipeline(m_spriteRendererSystem);
        AddToRenderingPipeline(m_lightingSystem);

        // Animation pipeline
        m_animationSystem.Initialize("Animation System");
        m_animationPipeline.AddSystem(m_animationSystem);

        // Set debug values.
        m_debugData.visualizeDepth = false;

        // Shader::ClearShaderIncludes();
    }

    void RenderEngine::SetupEngineShaders()
    {
        auto* storage     = Resources::ResourceStorage::Get();
        auto& shaderCache = storage->GetCache<Shader>();

        for (auto& shaderResource : shaderCache)
        {
            Shader*           shader = storage->GetResource<Shader>(shaderResource.first);
            const String path   = shader->GetPath();

            if (path.compare("Resources/Engine/Shaders/Unlit/Unlit.linaglsl") == 0)
                m_standardUnlitShader = shader;
            else if (path.compare("Resources/Engine/Shaders/PBR/Lit.linaglsl") == 0)
                m_standardLitShader = shader;
            else if (path.compare("Resources/Engine/Shaders/HDRI/HDRIEquirectangular.linaglsl") == 0)
                m_hdriEquirectangularShader = shader;
            else if (path.compare("Resources/Engine/Shaders/HDRI/HDRIIrradiance.linaglsl") == 0)
                m_hdriIrradianceShader = shader;
            else if (path.compare("Resources/Engine/Shaders/HDRI/HDRIPrefilter.linaglsl") == 0)
                m_hdriPrefilterShader = shader;
            else if (path.compare("Resources/Engine/Shaders/HDRI/HDRIBRDF.linaglsl") == 0)
                m_hdriBRDFShader = shader;

            shader->BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
            shader->BindBlockToBuffer(UNIFORMBUFFER_LIGHTDATA_BINDPOINT, UNIFORMBUFFER_LIGHTDATA_NAME);
            shader->BindBlockToBuffer(UNIFORMBUFFER_DEBUGDATA_BINDPOINT, UNIFORMBUFFER_DEBUGDATA_NAME);
        }
    }

    bool RenderEngine::ValidateEngineShaders()
    {
        bool validated = false;

        auto* storage     = Resources::ResourceStorage::Get();
        auto& shaderCache = storage->GetCache<Shader>();

        for (auto& shaderResource : shaderCache)
        {
            Shader* shader = storage->GetResource<Shader>(shaderResource.first);

            LINA_TRACE("Validating {0}", shader->GetPath());
            bool success = m_renderDevice.ValidateShaderProgram(shader->GetID());

            if (!success)
                LINA_TRACE("Failed validation");

            validated |= success;
        }

        return !validated;
    }

    void RenderEngine::ConstructEngineMaterials()
    {
        // Keep here in-case we need to programatically re-create engine materials.
        // m_defaultLit = Material::CreateMaterial(m_storage->GetResource<Shader>("Resources/Engine/Shaders/PBR/Lit.linaglsl"), "Resources/Engine/Materials/DefaultLit.linamat");
        // m_defaultUnlit      = Material::CreateMaterial(m_storage->GetResource<Shader>("Resources/Engine/Shaders/Unlit/Unlit.linaglsl"), "Resources/Engine/Materials/DefaultUnlit.linamat");
        // m_defaultSkybox     = Material::CreateMaterial(m_storage->GetResource<Shader>("Resources/Engine/Shaders/Skybox/SkyboxAtmospheric.linaglsl"), "Resources/Engine/Materials/DefaultSkybox.linamat");
        // m_defaultSprite     = Material::CreateMaterial(m_storage->GetResource<Shader>("Resources/Engine/Shaders/2D/Sprite.linaglsl"), "Resources/Engine/Materials/DefaultSprite.linamat");
        // m_defaultSkyboxHDRI = Material::CreateMaterial(m_storage->GetResource<Shader>("Resources/Engine/Shaders/Skybox/SkyboxHDRI.linaglsl"), "Resources/Engine/Materials/DefaultSkyboxHDRI.linamat");

        m_defaultLit        = m_storage->GetResource<Material>("Resources/Engine/Materials/DefaultLit.linamat");
        m_defaultUnlit      = m_storage->GetResource<Material>("Resources/Engine/Materials/DefaultUnlit.linamat");
        m_defaultSkybox     = m_storage->GetResource<Material>("Resources/Engine/Materials/DefaultSkybox.linamat");
        m_defaultSprite     = m_storage->GetResource<Material>("Resources/Engine/Materials/DefaultSprite.linamat");
        m_defaultSkyboxHDRI = m_storage->GetResource<Material>("Resources/Engine/Materials/DefaultSkyboxHDRI.linamat");

        auto* text                                    = Resources::ResourceStorage::Get()->GetResource<Texture>("Resources/Engine/Textures/HDR/Venice.hdr");
        m_defaultSkyboxHDRI->m_environmentHDR.m_sid   = text->GetSID();
        m_defaultSkyboxHDRI->m_environmentHDR.m_value = text;
        CaptureCalculateHDRI(*text);
        m_defaultSkyboxHDRI->SetTexture(MAT_MAP_ENVIRONMENT, &m_hdriCubemap, TextureBindMode::BINDTEXTURE_CUBEMAP);

        m_gBufferLightPassMaterial.SetShader(m_storage->GetResource<Shader>("Resources/Engine/Shaders/ScreenQuads/SQLightPass.linaglsl"));
        m_screenQuadFinalMaterial.SetShader(m_storage->GetResource<Shader>("Resources/Engine/Shaders/ScreenQuads/SQFinal.linaglsl"));
        m_screenQuadBlurMaterial.SetShader(m_storage->GetResource<Shader>("Resources/Engine/Shaders/ScreenQuads/SQBlur.linaglsl"));
        m_hdriMaterial.SetShader(m_hdriEquirectangularShader);
        m_debugLineMaterial.SetShader(m_storage->GetResource<Shader>("Resources/Engine/Shaders/Debug/DebugLine.linaglsl"));
        m_debugIconMaterial.SetShader(m_storage->GetResource<Shader>("Resources/Engine/Shaders/Debug/DebugIcon.linaglsl"));
        m_shadowMapMaterial.SetShader(m_storage->GetResource<Shader>("Resources/Engine/Shaders/ScreenQuads/SQShadowMap.linaglsl"));
        m_pLightShadowDepthMaterial.SetShader(m_storage->GetResource<Shader>("Resources/Engine/Shaders/PBR/PointShadowsDepth.linaglsl"));
        UpdateRenderSettings();
    }

    void RenderEngine::ConstructRenderTargets()
    {
        SamplerParameters params;
        params.m_textureParams.m_internalPixelFormat = PixelFormat::FORMAT_RGB16F;
        params.m_textureParams.m_pixelFormat = PixelFormat::FORMAT_RGB;

        SamplerParameters cubemapParams;
        cubemapParams.m_textureParams.m_wrapR = cubemapParams.m_textureParams.m_wrapS = cubemapParams.m_textureParams.m_wrapT = SamplerWrapMode::WRAP_CLAMP_EDGE;
        cubemapParams.m_textureParams.m_magFilter                                                                             = SamplerFilter::FILTER_LINEAR;
        cubemapParams.m_textureParams.m_minFilter                                                                             = SamplerFilter::FILTER_LINEAR_MIPMAP_LINEAR;
        cubemapParams.m_textureParams.m_internalPixelFormat                                                                   = PixelFormat::FORMAT_RGB16F;
        cubemapParams.m_textureParams.m_pixelFormat                                                                           = PixelFormat::FORMAT_RGB;

        // GBuffer
        SamplerParameters gBufferParams4;
        gBufferParams4.m_textureParams.m_pixelFormat         = PixelFormat::FORMAT_RGB;
        gBufferParams4.m_textureParams.m_internalPixelFormat = PixelFormat::FORMAT_RGBA16F;
        gBufferParams4.m_textureParams.m_minFilter           = SamplerFilter::FILTER_NEAREST;
        gBufferParams4.m_textureParams.m_magFilter           = SamplerFilter::FILTER_NEAREST;

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

        // Cubemaps
        m_reflectionCubemap.ConstructRTCubemapTexture(m_hdriResolution, cubemapParams);
        m_skyboxIrradianceCubemap.ConstructRTCubemapTexture(m_skyboxIrradianceResolution, cubemapParams);

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
        m_reflectionCaptureRenderBuffer.Construct(RenderBufferStorage::STORAGE_DEPTH_COMP24, m_hdriResolution);
        m_skyboxIrradianceCaptureRenderBuffer.Construct(RenderBufferStorage::STORAGE_DEPTH_COMP24, m_skyboxIrradianceResolution);

        // Initialize primary render target.
        m_primaryRenderTarget.Construct(m_primaryRTTexture0, TextureBindMode::BINDTEXTURE_TEXTURE2D, FrameBufferAttachment::ATTACHMENT_COLOR);
        m_primaryMSAATarget.Construct(m_primaryMSAARTTexture0, TextureBindMode::BINDTEXTURE_TEXTURE2D_MULTISAMPLE, FrameBufferAttachment::ATTACHMENT_COLOR, FrameBufferAttachment::ATTACHMENT_DEPTH, m_primaryMSAABuffer.GetID(), 0);

        m_gBufferPosition.ConstructRTTexture(m_screenSize, gBufferParams4, false);
        m_gBufferNormal.ConstructRTTexture(m_screenSize, gBufferParams4, false);
        m_gBufferAlbedo.ConstructRTTexture(m_screenSize, gBufferParams4, false);
        m_gBufferEmission.ConstructRTTexture(m_screenSize, gBufferParams4, false);
        m_gBufferMetallicRoughnessAOWorkflow.ConstructRTTexture(m_screenSize, gBufferParams4, false);

        m_gBufferRenderBuffer.Construct(RenderBufferStorage::STORAGE_DEPTH, m_screenSize);
        m_gBuffer.Construct(m_gBufferPosition, TextureBindMode::BINDTEXTURE_TEXTURE2D, FrameBufferAttachment::ATTACHMENT_COLOR, FrameBufferAttachment::ATTACHMENT_DEPTH, m_gBufferRenderBuffer.GetID(), 0);
        m_renderDevice.BindTextureToRenderTarget(m_gBuffer.GetID(), m_gBufferNormal.GetID(), TextureBindMode::BINDTEXTURE_TEXTURE2D, FrameBufferAttachment::ATTACHMENT_COLOR, 1);
        m_renderDevice.BindTextureToRenderTarget(m_gBuffer.GetID(), m_gBufferAlbedo.GetID(), TextureBindMode::BINDTEXTURE_TEXTURE2D, FrameBufferAttachment::ATTACHMENT_COLOR, 2);
        m_renderDevice.BindTextureToRenderTarget(m_gBuffer.GetID(), m_gBufferEmission.GetID(), TextureBindMode::BINDTEXTURE_TEXTURE2D, FrameBufferAttachment::ATTACHMENT_COLOR, 3);
        m_renderDevice.BindTextureToRenderTarget(m_gBuffer.GetID(), m_gBufferMetallicRoughnessAOWorkflow.GetID(), TextureBindMode::BINDTEXTURE_TEXTURE2D, FrameBufferAttachment::ATTACHMENT_COLOR, 4);
        uint32 gBufferAttachments[5] = {FrameBufferAttachment::ATTACHMENT_COLOR, (FrameBufferAttachment::ATTACHMENT_COLOR + (uint32)1), (FrameBufferAttachment::ATTACHMENT_COLOR + (uint32)2),
                                        (FrameBufferAttachment::ATTACHMENT_COLOR + (uint32)3), (FrameBufferAttachment::ATTACHMENT_COLOR + (uint32)4)};
        m_renderDevice.MultipleDrawBuffersCommand(m_gBuffer.GetID(), 5, gBufferAttachments);

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
        m_reflectionCaptureRenderTarget.Construct(FrameBufferAttachment::ATTACHMENT_DEPTH, m_reflectionCaptureRenderBuffer.GetID());
        m_skyboxIrradianceCaptureRenderTarget.Construct(FrameBufferAttachment::ATTACHMENT_DEPTH, m_skyboxIrradianceCaptureRenderBuffer.GetID());

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
            m_previewRTTexture.ConstructRTTexture(m_screenSize, m_primaryRTParams, false);
            m_previewRenderBuffer.Construct(RenderBufferStorage::STORAGE_DEPTH, m_screenSize);
            m_previewRenderTarget.Construct(m_previewRTTexture, TextureBindMode::BINDTEXTURE_TEXTURE2D, FrameBufferAttachment::ATTACHMENT_COLOR, FrameBufferAttachment::ATTACHMENT_DEPTH, m_previewRenderBuffer.GetID());
        }
    }

    void RenderEngine::Shutdown()
    {
        LINA_TRACE("[Shutdown] -> RenderEngine ({0})", typeid(*this).name());

        // Dump the remaining memory.
        DumpMemory();

        // Release Vertex Array Objects
        m_skyboxVAO     = m_renderDevice.ReleaseVertexArray(m_skyboxVAO);
        m_screenQuadVAO = m_renderDevice.ReleaseVertexArray(m_screenQuadVAO);
        m_hdriCubeVAO   = m_renderDevice.ReleaseVertexArray(m_hdriCubeVAO);
        m_lineVAO       = m_renderDevice.ReleaseVertexArray(m_lineVAO);
    }

    void RenderEngine::Tick(float delta)
    {
        m_animationPipeline.UpdateSystems(delta);
    }

    void RenderEngine::Render(float interpolation)
    {
        PROFILER_FUNC("Render Engine Render");

        PROFILER_BLOCK("Event: Pre Render");
        m_eventSystem->Trigger<Event::EPreRender>(Event::EPreRender{});
        PROFILER_END_BLOCK;

       //  Draw();

        if (!m_firstFrameDrawn)
            m_firstFrameDrawn = true;

        // Reset the viewport & fbo to allow any post render drawing, like GUI.
        // TODO -- Allow in-game GUI drawing later on.
        if (m_appMode == ApplicationMode::Editor)
        {
            m_renderDevice.SetFBO(0);
            m_renderDevice.SetViewport(m_screenPos, m_screenSize);
            m_renderDevice.Clear(true, true, true, m_cameraSystem.GetCurrentClearColor(), 0xFF);
        }

        PROFILER_BLOCK("Event: Post Render");
        m_eventSystem->Trigger<Event::EPostRender>(Event::EPostRender{});
        PROFILER_END_BLOCK;
    }

    void RenderEngine::AddToRenderingPipeline(ECS::System& system)
    {
        m_renderingPipeline.AddSystem(system);
    }

    void RenderEngine::AddToAnimationPipeline(ECS::System& system)
    {
        m_animationPipeline.AddSystem(system);
    }

    void RenderEngine::SetScreenDisplay(Vector2i pos, Vector2i size)
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
        {
            m_renderDevice.ResizeRTTexture(m_secondaryRTTexture.GetID(), m_screenSize, m_primaryRTParams.m_textureParams.m_internalPixelFormat, m_primaryRTParams.m_textureParams.m_pixelFormat);
            m_renderDevice.ResizeRTTexture(m_previewRTTexture.GetID(), m_screenSize, m_primaryRTParams.m_textureParams.m_internalPixelFormat, m_primaryRTParams.m_textureParams.m_pixelFormat);
        }
    }

    void RenderEngine::OnDrawLine(const Event::EDrawLine& event)
    {
        DrawLine(event.m_from, event.m_to, event.m_color, event.m_lineWidth);
    }

    void RenderEngine::OnDrawBox(const Event::EDrawBox& event)
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

        Vector<Vector3> lines{bottomLB, bottomLF, bottomLF, bottomRF, bottomRF, bottomRB, bottomRB, bottomLB, topLB, topLF, topLF, topRF, topRF, topRB, topRB, topLB, bottomLB, topLB, bottomLF, topLF, bottomRF, topRF, bottomRB, topRB};

        for (int i = 0; i < lines.size(); i += 2)
            DrawLine(lines[i], lines[i + 1], event.m_color, event.m_lineWidth);

        lines.clear();
    }

    void RenderEngine::OnDrawCircle(const Event::EDrawCircle& event)
    {
        Vector3          previousPos = (event.m_radius * Vector3(1, 0, 0));
        const Quaternion rot         = event.m_rotation;

        Vector<std::pair<Vector3, Vector3>> lines;

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

    void RenderEngine::OnDrawSphere(const Event::EDrawSphere& event)
    {
        OnDrawCircle(Event::EDrawCircle{event.m_position, event.m_radius, event.m_color, event.m_lineWidth, false, Quaternion()});
        OnDrawCircle(Event::EDrawCircle{event.m_position, event.m_radius, event.m_color, event.m_lineWidth, false, Quaternion(Vector3(0, 0, 1), 90)});
        OnDrawCircle(Event::EDrawCircle{event.m_position, event.m_radius, event.m_color, event.m_lineWidth, false, Quaternion(Vector3(1, 0, 0), 90)});
    }

    void RenderEngine::OnDrawHemiSphere(const Event::EDrawHemiSphere& event)
    {
        Quaternion q1 = Quaternion(Vector3(1, 0, 0), event.m_top ? -90.0f : 90.0f);
        Quaternion q2 = q1 * Quaternion(Vector3(0, 0, 1), 90);
        OnDrawCircle(Event::EDrawCircle{event.m_position, event.m_radius, event.m_color, event.m_lineWidth, false, Quaternion()});
        OnDrawCircle(Event::EDrawCircle{event.m_position, event.m_radius, event.m_color, event.m_lineWidth, true, q1});
        OnDrawCircle(Event::EDrawCircle{event.m_position, event.m_radius, event.m_color, event.m_lineWidth, true, q2});
    }

    void RenderEngine::OnDrawCapsule(const Event::EDrawCapsule& event)
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

    void RenderEngine::OnWindowResized(const Event::EWindowResized& event)
    {
        SetScreenDisplay(Vector2i(0, 0), Vector2((float)event.m_windowProps.m_width, (float)event.m_windowProps.m_height));
    }

    void RenderEngine::OnAllResourcesOfTypeLoaded(const Event::EAllResourcesOfTypeLoaded& ev)
    {
        if (ev.m_tid == GetTypeID<Shader>())
        {
            SetupEngineShaders();
        }
        else if (ev.m_tid == GetTypeID<Material>())
        {
            ConstructEngineMaterials();
        }
        else if (ev.m_tid == GetTypeID<Model>())
        {
            // Set model nodes
            m_cubeNode   = m_storage->GetResource<Model>("Resources/Engine/Meshes/Primitives/Cube.fbx")->m_rootNode->GetChildren()[0];
            m_sphereNode = m_storage->GetResource<Model>("Resources/Engine/Meshes/Primitives/Sphere.fbx")->m_rootNode->GetChildren()[0];
        }
    }

    void RenderEngine::OnResourceReloaded(const Event::EResourceReloaded& ev)
    {
        if (ev.m_tid == GetTypeID<Shader>())
        {
            Shader* shader = m_storage->GetResource<Shader>(ev.m_sid);
            shader->BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
            shader->BindBlockToBuffer(UNIFORMBUFFER_LIGHTDATA_BINDPOINT, UNIFORMBUFFER_LIGHTDATA_NAME);
            shader->BindBlockToBuffer(UNIFORMBUFFER_DEBUGDATA_BINDPOINT, UNIFORMBUFFER_DEBUGDATA_NAME);
            shader->BindBlockToBuffer(UNIFORMBUFFER_APPDATA_BINDPOINT, UNIFORMBUFFER_APPDATA_NAME);
        }
    }

    void RenderEngine::DumpMemory()
    {
        while (!m_debugLineQueue.empty())
            m_debugLineQueue.pop();

        while (!m_debugIconQueue.empty())
            m_debugIconQueue.pop();
    }

    void RenderEngine::Draw()
    {
        PROFILER_BLOCK("Render: Update Systems");
        UpdateSystems(0.0f);
        PROFILER_END_BLOCK;

        PROFILER_BLOCK("Render: Lightmapping");
        // Set render targets for point light shadows & calculate all the depth textures.
        auto& tuple = m_lightingSystem.GetPointLights();
        for (int i = 0; i < tuple.size(); i++)
        {
            if (std::get<1>(tuple[i])->m_castsShadows)
            {
                Vector3 lightPos  = std::get<0>(tuple[i])->GetLocation();
                float   farPlane  = std::get<1>(tuple[i])->m_shadowFar;
                float   nearPlane = std::get<1>(tuple[i])->m_shadowNear;

                Vector<Matrix> shadowTransforms = m_lightingSystem.GetPointLightMatrices(lightPos, m_pLightShadowResolution, nearPlane, farPlane);

                // Set render target
                m_renderDevice.SetFBO(m_pLightShadowTargets[i].GetID());
                m_renderDevice.SetViewport(Vector2::Zero, m_pLightShadowResolution);

                // Clear color bit.
                m_renderDevice.Clear(false, true, false, Color::White, 0xFF);

                // Update depth shader data.
                m_pLightShadowDepthMaterial.SetVector3(UF_LIGHTPOS, lightPos);
                m_pLightShadowDepthMaterial.SetFloat(UF_LIGHTFARPLANE, farPlane);

                for (unsigned int j = 0; j < 6; j++)
                    m_pLightShadowDepthMaterial.SetMatrix4(String(UF_SHADOWMATRICES) + "[" + TO_STRING(j).c_str() + "]", shadowTransforms[j]);

                // Draw scene
                DrawSceneObjects(m_shadowMapDrawParams, &m_pLightShadowDepthMaterial, false);
            }
        }
        PROFILER_END_BLOCK;

        // m_renderDevice.SetFBO(m_primaryMSAATarget.GetID());
        m_renderDevice.SetFBO(m_gBuffer.GetID());
        m_renderDevice.SetViewport(Vector2::Zero, m_screenSize);

        if (!Event::EventSystem::Get()->IsEmpty<Event::ECustomRender>())
        {
            PROFILER_BLOCK("Event: Custom Render");
            Event::EventSystem::Get()->Trigger<Event::ECustomRender>(Event::ECustomRender{});
            PROFILER_END_BLOCK;
        }
        else
        {
            m_renderDevice.Clear(true, true, true, m_cameraSystem.GetCurrentClearColor(), 0xFF);
            Material* skyboxMat = m_skyboxMaterial == nullptr ? &m_defaultSkyboxMaterial : m_skyboxMaterial;

            // If the skybox is not an HDRI skybox, and if it contributes to indirect lighting.
            if (skyboxMat->m_skyboxIndirectContributionFactor != 0.0f && skyboxMat->m_shaderHandle.m_value->GetSpecification() != ShaderSpecification::Sky_HDRICube)
            {
                // Render the skybox into a cubemap
                PROFILER_BLOCK("Render: Capture Skybox");
                CaptureSkybox();
                PROFILER_END_BLOCK;
            }

            PROFILER_BLOCK("Render: Draw Skybox");
            DrawSkybox();
            PROFILER_END_BLOCK;

            PROFILER_BLOCK("Render: Draw Scene Objects");
            DrawSceneObjects(m_defaultDrawParams);
            PROFILER_END_BLOCK;
        }

        // Draw debugs
        ProcessDebugQueue();

        // Finalize drawing.
        DrawFinalize();
    }

    void RenderEngine::DrawFinalize(RenderTarget* overrideTarget)
    {
        // Frag color
        // m_renderDevice.BlitRenderTargets(m_primaryMSAATarget.GetID(), m_screenSize.x, m_screenSize.y, m_primaryRenderTarget.GetID(), m_screenSize.x, m_screenSize.y, BufferBit::BIT_COLOR, SamplerFilter::FILTER_NEAREST, FrameBufferAttachment::ATTACHMENT_COLOR, (uint32)0);

        // Bright color - HDR
        // m_renderDevice.BlitRenderTargets(m_primaryMSAATarget.GetID(), m_screenSize.x, m_screenSize.y, m_primaryRenderTarget.GetID(), m_screenSize.x, m_screenSize.y, BufferBit::BIT_COLOR, SamplerFilter::FILTER_NEAREST, FrameBufferAttachment::ATTACHMENT_COLOR, (uint32)1);

        // Below we process bloom post fx based on brightColor output in the shaders.
        // bool horizontal = true;
        // if (m_renderSettings->m_bloomEnabled)
        //{
        //    // Write to the pingpong buffers to apply 2 pass gaussian blur.
        //    bool         firstIteration = true;
        //    unsigned int amount         = 10;
        //    for (unsigned int i = 0; i < amount; i++)
        //    {
        //        // Select FBO
        //        m_renderDevice.SetFBO(horizontal ? m_pingPongRenderTarget1.GetID() : m_pingPongRenderTarget2.GetID());
        //
        //        // Setup material & use.
        //        m_screenQuadBlurMaterial.SetBool(MAT_ISHORIZONTAL, horizontal);
        //        if (firstIteration)
        //            m_screenQuadBlurMaterial.SetTexture(MAT_MAP_SCREEN, &m_primaryRTTexture1);
        //        else
        //        {
        //            if (horizontal)
        //                m_screenQuadBlurMaterial.SetTexture(MAT_MAP_SCREEN, &m_pingPongRTTexture2);
        //            else
        //                m_screenQuadBlurMaterial.SetTexture(MAT_MAP_SCREEN, &m_pingPongRTTexture1);
        //        }
        //
        //        // Update shader data & draw.
        //        UpdateShaderData(&m_screenQuadBlurMaterial);
        //        m_renderDevice.Draw(m_screenQuadVAO, m_fullscreenQuadDP, 0, 6, true);
        //        horizontal = !horizontal;
        //        if (firstIteration)
        //            firstIteration = false;
        //    }
        //}

        // Take the final image (without bloom), run it through a custom post processing pipeline.
        // Texture* screenMap = &m_primaryRTTexture0;
        // for (auto& pp : m_postProcessMap)
        // {
        //     pp.second.Draw(screenMap);
        //     screenMap = &pp.second.GetTexture();
        // }

        // After we've applied custom post processing, draw the final image either to the screen, or to a secondary frame buffer to display it in editor.
        if (m_appMode == ApplicationMode::Editor)
        {
            if (overrideTarget != nullptr)
                m_renderDevice.SetFBO(overrideTarget->GetID());
            else
                m_renderDevice.SetFBO(m_secondaryRenderTarget.GetID());
        }
        else
            // Back to default buffer
            m_renderDevice.SetFBO(0);

        m_renderDevice.SetViewport(m_screenPos, m_screenSize);
        m_renderDevice.Clear(true, true, true, Color::White, 0xFF);

        // Set frame buffer texture on the material.
        // m_screenQuadFinalMaterial.SetTexture(MAT_MAP_SCREEN, screenMap, TextureBindMode::BINDTEXTURE_TEXTURE2D);
        //
        // if (m_screenQuadFinalMaterial.m_bools[MAT_BLOOMENABLED])
        //     m_screenQuadFinalMaterial.SetTexture(MAT_MAP_BLOOM, horizontal ? &m_pingPongRTTexture1 : &m_pingPongRTTexture2, TextureBindMode::BINDTEXTURE_TEXTURE2D);
        //
        // Vector2 inverseMapSize = 1.0f / Vector2((float)m_primaryRTTexture0.GetSize().x, (float)(m_primaryRTTexture0.GetSize().y));
        // m_screenQuadFinalMaterial.SetVector3(MAT_INVERSESCREENMAPSIZE, Vector3(inverseMapSize.x, inverseMapSize.y, 0.0));
        //
        // UpdateShaderData(&m_screenQuadFinalMaterial);

        if (m_skyboxMaterial->m_shaderHandle.m_value->GetSpecification() == ShaderSpecification::Sky_HDRICube && !m_gBufferLightPassMaterial.m_hdriDataSet)
        {
            SetHDRIData(&m_gBufferLightPassMaterial);
        }
        else if (m_skyboxMaterial->m_shaderHandle.m_value->GetSpecification() != ShaderSpecification::Sky_HDRICube && m_gBufferLightPassMaterial.m_hdriDataSet)
            RemoveHDRIData(&m_gBufferLightPassMaterial);

        m_gBufferLightPassMaterial.SetTexture(MAT_MAP_GPOS, &m_gBufferPosition);
        m_gBufferLightPassMaterial.SetTexture(MAT_MAP_GNORMAL, &m_gBufferNormal);
        m_gBufferLightPassMaterial.SetTexture(MAT_MAP_GALBEDO, &m_gBufferAlbedo);
        m_gBufferLightPassMaterial.SetTexture(MAT_MAP_GEMISSION, &m_gBufferEmission);
        m_gBufferLightPassMaterial.SetTexture(MAT_MAP_GMETALLICROUGHNESSAOWORKFLOW, &m_gBufferMetallicRoughnessAOWorkflow);
        m_gBufferLightPassMaterial.SetTexture(MAT_MAP_REFLECTION, &m_reflectionCubemap);

        // Skybox irradiance.
        if (m_skyboxMaterial->m_skyboxIndirectContributionFactor != 0.0f && !m_gBufferLightPassMaterial.m_skyIrradianceDataSet)
        {
            m_gBufferLightPassMaterial.SetTexture(MAT_MAP_SKYBOXIRR, &m_skyboxIrradianceCubemap);
            m_gBufferLightPassMaterial.SetFloat(MAT_SKYBOXIRRFACTOR, m_skyboxMaterial->m_skyboxIndirectContributionFactor);
            m_gBufferLightPassMaterial.m_skyIrradianceDataSet = true;
        }
        else if (m_skyboxMaterial->m_skyboxIndirectContributionFactor == 0.0f && m_gBufferLightPassMaterial.m_skyIrradianceDataSet)
        {
            m_gBufferLightPassMaterial.m_skyIrradianceDataSet = false;
            m_gBufferLightPassMaterial.RemoveTexture(MAT_MAP_SKYBOXIRR);
        }

        UpdateShaderData(&m_gBufferLightPassMaterial, true);
        m_renderDevice.Draw(m_screenQuadVAO, m_fullscreenQuadDP, 0, 6, true);
    }

    uint32 RenderEngine::RenderModelPreview(Model* model, Matrix& modelMatrix, RenderTarget* overrideTarget, Material* overrideMaterial)
    {
        // Store the current skybox & switch to HDRI one
        Material* currentSkybox = m_skyboxMaterial;
        SetSkyboxMaterial(m_defaultSkyboxHDRI);

        // Update systems & prepare
        UpdateSystems(0.0f);
        m_renderDevice.SetFBO(m_primaryMSAATarget.GetID());
        m_renderDevice.SetViewport(Vector2::Zero, m_screenSize);
        m_renderDevice.Clear(true, true, true, m_cameraSystem.GetCurrentClearColor(), 0xFF);

        // Draw skybox and given model.
        // DrawSkybox();
        m_modelNodeSystem.FlushModelNode(model->m_rootNode, modelMatrix, m_defaultDrawParams, overrideMaterial);
        DrawFinalize(overrideTarget == nullptr ? &m_previewRenderTarget : overrideTarget);

        // Reset buffers back as well as the skybox.
        m_renderDevice.SetFBO(0);
        m_renderDevice.SetViewport(m_screenPos, m_screenSize);
        m_renderDevice.Clear(true, true, true, m_cameraSystem.GetCurrentClearColor(), 0xFF);
        SetSkyboxMaterial(currentSkybox);
        return m_previewRTTexture.GetID();
    }

    void RenderEngine::DrawIcon(Vector3 position, StringIDType textureID, float size)
    {
        m_debugIconQueue.push(DebugIcon{position, textureID, size});
    }

    void RenderEngine::DrawLine(Vector3 p1, Vector3 p2, Color col, float width)
    {
        m_debugLineQueue.push(DebugLine{p1, p2, col, width});
    }

    void RenderEngine::ProcessDebugQueue()
    {
        while (!m_debugLineQueue.empty())
        {
            DebugLine line = m_debugLineQueue.front();
            m_renderDevice.SetShader(m_debugLineMaterial.m_shaderHandle.m_value->GetID());
            m_renderDevice.UpdateShaderUniformColor(m_debugLineMaterial.m_shaderHandle.m_value->GetID(), MAT_COLOR, line.m_color);
            m_renderDevice.DrawLine(m_debugLineMaterial.m_shaderHandle.m_value->GetID(), Matrix::Identity(), line.m_from, line.m_to, line.m_width);
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
            m_debugIconMaterial.SetTexture(MAT_TEXTURE2D_DIFFUSE, m_storage->GetResource<Texture>(icon.m_textureID));
            UpdateShaderData(&m_debugIconMaterial);
            m_renderDevice.Draw(m_quadMesh.GetVertexArray().GetID(), m_defaultDrawParams, 1, m_quadMesh.GetVertexArray().GetIndexCount(), false);
            m_debugIconQueue.pop();
        }
    }

    void RenderEngine::SetDrawParameters(const DrawParams& params)
    {
        m_renderDevice.SetDrawParameters(params);
    }

    void RenderEngine::UpdateRenderSettings()
    {
        m_screenQuadFinalMaterial.SetBool(MAT_FXAAENABLED, m_renderSettings->m_fxaaEnabled);
        m_screenQuadFinalMaterial.SetBool(MAT_BLOOMENABLED, m_renderSettings->m_bloomEnabled);
        m_screenQuadFinalMaterial.SetBool(MAT_VIGNETTEENABLED, m_renderSettings->m_vignetteEnabled);
        m_screenQuadFinalMaterial.SetFloat(MAT_FXAAREDUCEMIN, m_renderSettings->m_fxaaReduceMin);
        m_screenQuadFinalMaterial.SetFloat(MAT_FXAAREDUCEMUL, m_renderSettings->m_fxaaReduceMul);
        m_screenQuadFinalMaterial.SetFloat(MAT_FXAASPANMAX, m_renderSettings->m_fxaaSpanMax);
        m_screenQuadFinalMaterial.SetFloat(MAT_GAMMA, m_renderSettings->m_gamma);
        m_screenQuadFinalMaterial.SetFloat(MAT_EXPOSURE, m_renderSettings->m_exposure);
        m_screenQuadFinalMaterial.SetFloat(MAT_VIGNETTEAMOUNT, m_renderSettings->m_vignetteAmount);
        m_screenQuadFinalMaterial.SetFloat(MAT_VIGNETTEPOW, m_renderSettings->m_vignettePow);
    }

    void RenderEngine::DrawSkybox()
    {
        Material* skyboxMat = m_skyboxMaterial == nullptr ? &m_defaultSkyboxMaterial : m_skyboxMaterial;
        UpdateShaderData(skyboxMat);
        m_renderDevice.Draw(m_skyboxVAO, m_skyboxDrawParams, 1, 4, true);
    }

    PostProcessEffect& RenderEngine::AddPostProcessEffect(Shader* shader)
    {
        if (m_postProcessMap.find(shader) == m_postProcessMap.end())
        {
            m_postProcessMap[shader] = PostProcessEffect();
            m_postProcessMap[shader].Construct(shader);
        }

        return m_postProcessMap[shader];
    }

    void RenderEngine::DrawSceneObjects(DrawParams& drawParams, Material* overrideMaterial, bool completeFlush)
    {
        m_modelNodeSystem.FlushOpaque(drawParams, overrideMaterial, completeFlush);
        m_modelNodeSystem.FlushTransparent(drawParams, overrideMaterial, completeFlush);
        m_spriteRendererSystem.Flush(drawParams, overrideMaterial, completeFlush);
        Event::EventSystem::Get()->Trigger<Event::EPostSceneDraw>(Event::EPostSceneDraw{});
    }

    void RenderEngine::UpdateUniformBuffers()
    {
        Vector3 cameraLocation = m_cameraSystem.GetCameraLocation();
        Vector4 viewPos        = Vector4(cameraLocation.x, cameraLocation.y, cameraLocation.z, 1.0f);

        // Update global matrix buffer
        uintptr currentGlobalDataOffset = 0;

        m_globalViewBuffer.Update(&m_cameraSystem.GetProjectionMatrix()[0][0], currentGlobalDataOffset, sizeof(Matrix));
        currentGlobalDataOffset += sizeof(Matrix);

        m_globalViewBuffer.Update(&m_cameraSystem.GetViewMatrix()[0][0], currentGlobalDataOffset, sizeof(Matrix));
        currentGlobalDataOffset += sizeof(Matrix);

        m_globalViewBuffer.Update(&m_lightingSystem.GetDirectionalLightMatrix()[0][0], currentGlobalDataOffset, sizeof(Matrix));
        currentGlobalDataOffset += sizeof(Matrix);

        Matrix VP = m_cameraSystem.GetProjectionMatrix() * m_cameraSystem.GetViewMatrix();
        m_globalViewBuffer.Update(&VP[0][0], currentGlobalDataOffset, sizeof(Matrix));
        currentGlobalDataOffset += sizeof(Matrix);

        m_globalViewBuffer.Update(&viewPos, currentGlobalDataOffset, sizeof(Vector4));
        currentGlobalDataOffset += sizeof(Vector4);

        ECS::CameraComponent* cameraComponent = m_cameraSystem.GetActiveCameraComponent();

        if (cameraComponent != nullptr)
        {
            // Update only if changed.
            if (m_bufferValueRecord.zNear != cameraComponent->m_zNear)
            {
                m_bufferValueRecord.zNear = cameraComponent->m_zNear;
                m_globalViewBuffer.Update(&cameraComponent->m_zNear, currentGlobalDataOffset, sizeof(float));
            }
            currentGlobalDataOffset += sizeof(float);

            // Update only if changed.
            if (m_bufferValueRecord.zFar != cameraComponent->m_zFar)
            {
                m_bufferValueRecord.zFar = cameraComponent->m_zFar;
                m_globalViewBuffer.Update(&cameraComponent->m_zNear, currentGlobalDataOffset, sizeof(float));
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

        // Update app data buffer
        Vector2 screenSize = Vector2((float)m_screenSize.x, (float)m_screenSize.y);

        m_globalAppDataBuffer.Update(&screenSize, 0, sizeof(Vector2));
        m_globalAppDataBuffer.Update(&m_mousePosition, sizeof(Vector2), sizeof(Vector2));
        m_globalAppDataBuffer.Update(&m_deltaTime, sizeof(Vector2) * 2, sizeof(float));
        m_globalAppDataBuffer.Update(&m_elapsedTime, sizeof(Vector2) * 2 + sizeof(float), sizeof(float));
    }

    void RenderEngine::UpdateShaderData(Material* data, bool lightPass)
    {
        uint32 shaderID = data->m_shaderHandle.m_value->GetID();
        m_renderDevice.SetShader(shaderID);

        for (auto const& d : (*data).m_floats)
            m_renderDevice.UpdateShaderUniformFloat(shaderID, d.first, d.second);

        for (auto const& d : (*data).m_bools)
            m_renderDevice.UpdateShaderUniformInt(shaderID, d.first, d.second);

        for (auto const& d : (*data).m_colors)
            m_renderDevice.UpdateShaderUniformColor(shaderID, d.first, d.second);

        for (auto const& d : (*data).m_ints)
            m_renderDevice.UpdateShaderUniformInt(shaderID, d.first, d.second);

        for (auto const& d : (*data).m_vector2s)
            m_renderDevice.UpdateShaderUniformVector2(shaderID, d.first, d.second);

        for (auto const& d : (*data).m_vector3s)
            m_renderDevice.UpdateShaderUniformVector3(shaderID, d.first, d.second);

        for (auto const& d : (*data).m_vector4s)
            m_renderDevice.UpdateShaderUniformVector4F(shaderID, d.first, d.second);

        for (auto const& d : (*data).m_matrices)
            m_renderDevice.UpdateShaderUniformMatrix(shaderID, d.first, d.second);

        // Set material's shadow textures to the FBO textures.
        if (lightPass)
        {
            auto& tuple = m_lightingSystem.GetPointLights();

            for (int i = 0; i < m_lightingSystem.GetPointLights().size(); i++)
            {
                String textureName = String(MAT_MAPS_SHADOWDEPTH) + "[" + TO_STRING(i).c_str() + "]";
                if (std::get<1>(tuple[i])->m_castsShadows)
                    data->SetTexture(textureName, &m_pLightShadowTextures[i], TextureBindMode::BINDTEXTURE_CUBEMAP);
                else
                    data->RemoveTexture(textureName);
            }
        }

        // Can be set for skyboxes too.
        m_lightingSystem.SetLightingShaderData(shaderID);

        for (auto const& d : (*data).m_sampler2Ds)
        {
            // Set whether the texture is active or not.
            bool isActive = (d.second.m_isActive && d.second.m_texture.m_value != nullptr && !d.second.m_texture.m_value->GetIsEmpty()) ? true : false;
            m_renderDevice.UpdateShaderUniformInt(shaderID, d.first + MAT_EXTENSION_ISACTIVE, isActive);

            // Set the texture to corresponding active unit.
            m_renderDevice.UpdateShaderUniformInt(shaderID, d.first + MAT_EXTENSION_TEXTURE2D, d.second.m_unit);

            // Set texture
            if (isActive)
                m_renderDevice.SetTexture(d.second.m_texture.m_value->GetID(), d.second.m_texture.m_value->GetSamplerID(), d.second.m_unit, d.second.m_bindMode, true);
            else
            {

                if (d.second.m_bindMode == TextureBindMode::BINDTEXTURE_TEXTURE2D)
                    m_renderDevice.SetTexture(m_defaultTexture.GetID(), m_defaultTexture.GetSamplerID(), d.second.m_unit, BINDTEXTURE_TEXTURE2D);
                else
                    m_renderDevice.SetTexture(m_defaultCubemapTexture.GetID(), m_defaultCubemapTexture.GetSamplerID(), d.second.m_unit, BINDTEXTURE_CUBEMAP);
            }
        }

        if (!m_firstFrameDrawn)
        {
            m_renderDevice.ValidateShaderProgram(shaderID);
        }
    }

    void RenderEngine::CaptureReflections(Texture& writeTexture, const Vector3& areaLocation, const Vector2i& resolution)
    {
        return;
        // Build projection & view matrices for capturing HDRI data.
        Matrix captureProjection = Matrix::PerspectiveRH(90.0f, 1.0f, 0.1f, 10.0f);
        Matrix captureViews[]    = {Matrix::InitLookAtRH(areaLocation, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)), Matrix::InitLookAtRH(areaLocation, glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
                                 Matrix::InitLookAtRH(areaLocation, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)), Matrix::InitLookAtRH(areaLocation, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
                                 Matrix::InitLookAtRH(areaLocation, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)), Matrix::InitLookAtRH(areaLocation, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))};

        Shader* skyboxMaterialShader = m_skyboxMaterial->GetShaderHandle().m_value;

        m_renderDevice.SetShader(skyboxMaterialShader->GetID());
        m_renderDevice.SetFBO(m_reflectionCaptureRenderTarget.GetID());
        m_renderDevice.SetViewport(Vector2::Zero, resolution);
        m_renderDevice.ResizeRenderBuffer(m_reflectionCaptureRenderTarget.GetID(), m_reflectionCaptureRenderBuffer.GetID(), resolution, RenderBufferStorage::STORAGE_DEPTH_COMP24);
        m_cameraSystem.InjectProjMatrix(captureProjection);

        // Draw the cubemap.
        for (uint32 i = 0; i < 6; ++i)
        {
            m_cameraSystem.InjectViewMatrix(captureViews[i]);
            m_renderDevice.BindTextureToRenderTarget(m_reflectionCaptureRenderTarget.GetID(), writeTexture.GetID(), TextureBindMode::BINDTEXTURE_CUBEMAP_POSITIVE_X, FrameBufferAttachment::ATTACHMENT_COLOR, 0, i, 0, false);
            m_renderDevice.Clear(true, true, true, m_cameraSystem.GetCurrentClearColor(), 0xFF);

            // Draw whole scene
            DrawSkybox();
            DrawSceneObjects(m_defaultDrawParams);

            // m_renderDevice.Draw(m_hdriCubeVAO, m_defaultDrawParams, 0, 36, true);
            // m_renderDevice.Draw(m_skyboxVAO, m_skyboxDrawParams, 1, 4, true);
        }

        // Get back to gBuffer
        m_renderDevice.SetFBO(m_gBuffer.GetID());
        m_renderDevice.SetViewport(Vector2::Zero, m_screenSize);
    }

    void RenderEngine::CaptureSkybox()
    {

        const Vector3 areaLocation = Vector3::Zero;
        // Build projection & view matrices for capturing HDRI data.
        Matrix captureProjection = Matrix::PerspectiveRH(90.0f, 1.0f, 0.1f, 10.0f);
        Matrix captureViews[]    = {Matrix::InitLookAtRH(areaLocation, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)), Matrix::InitLookAtRH(areaLocation, glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
                                 Matrix::InitLookAtRH(areaLocation, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)), Matrix::InitLookAtRH(areaLocation, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
                                 Matrix::InitLookAtRH(areaLocation, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)), Matrix::InitLookAtRH(areaLocation, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))};

        Material*    skybox               = m_skyboxMaterial == nullptr ? &m_defaultSkyboxMaterial : m_skyboxMaterial;
        Shader*      skyboxMaterialShader = skybox->GetShaderHandle().m_value;
        const Matrix currentProjection    = m_cameraSystem.GetProjectionMatrix();
        const Matrix currentView          = m_cameraSystem.GetViewMatrix();

        // Switch to new RT & set new projection matrix.
        // m_renderDevice.SetFBO(m_gBuffer.GetID());
        // m_renderDevice.SetViewport(Vector2::Zero, m_skyboxIrradianceResolution);
        m_cameraSystem.SetProjectionMatrix(captureProjection);
        m_renderDevice.SetFBO(m_gBuffer.GetID());

        // Draw the cubemap.
        for (uint32 i = 0; i < 6; ++i)
        {
            m_cameraSystem.SetViewMatrix(captureViews[i]);
            UpdateUniformBuffers();

            // Drawing from 3rd attachment, gAlbedo
            m_renderDevice.BindTextureToRenderTarget(m_gBuffer.GetID(), m_skyboxIrradianceCubemap.GetID(), TextureBindMode::BINDTEXTURE_CUBEMAP_POSITIVE_X, FrameBufferAttachment::ATTACHMENT_COLOR, 2, i, 0, false, false);
            m_renderDevice.Clear(true, true, true, m_cameraSystem.GetCurrentClearColor(), 0xFF);
            DrawSkybox();
        }

        // Get back to gBuffer
        m_renderDevice.BindTextureToRenderTarget(m_gBuffer.GetID(), m_gBufferAlbedo.GetID(), TextureBindMode::BINDTEXTURE_TEXTURE2D, FrameBufferAttachment::ATTACHMENT_COLOR, 2, 0, 0, false, false);
        m_cameraSystem.SetProjectionMatrix(currentProjection);
        m_cameraSystem.SetViewMatrix(currentView);
        m_renderDevice.GenerateTextureMipmaps(m_skyboxIrradianceCubemap.GetID(), TextureBindMode::BINDTEXTURE_CUBEMAP);
        UpdateUniformBuffers();
    }

    void RenderEngine::CaptureCalculateHDRI(Texture& hdriTexture)
    {
        // Build projection & view matrices for capturing HDRI data.
        Matrix captureProjection = Matrix::PerspectiveRH(90.0f, 1.0f, 0.1f, 10.0f);
        Matrix captureViews[]    = {Matrix::InitLookAtRH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)), Matrix::InitLookAtRH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
                                 Matrix::InitLookAtRH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)), Matrix::InitLookAtRH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
                                 Matrix::InitLookAtRH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)), Matrix::InitLookAtRH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))};

        // Calculate HDRI, Irradiance, Prefilter and BRDF
        CalculateHDRICubemap(hdriTexture, captureProjection, captureViews);
        CalculateHDRIIrradiance(captureProjection, captureViews);
        CalculateHDRIPrefilter(captureProjection, captureViews);
        CalculateHDRIBRDF(captureProjection, captureViews);
        m_renderDevice.SetFBO(0);
        m_renderDevice.SetViewport(m_screenPos, m_screenSize);

        // Set flag
        m_lastCapturedHDR = &hdriTexture;
    }

    void RenderEngine::CalculateHDRICubemap(Texture& hdriTexture, glm::mat4& captureProjection, glm::mat4 views[6])
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
        m_renderDevice.UpdateShaderUniformInt(equirectangularShader, MAT_MAP_EQUIRECTANGULAR + String(MAT_EXTENSION_TEXTURE2D), 0);
        m_renderDevice.UpdateShaderUniformInt(equirectangularShader, MAT_MAP_EQUIRECTANGULAR + String(MAT_EXTENSION_ISACTIVE), 1);
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

    void RenderEngine::CalculateHDRIIrradiance(Matrix& captureProjection, Matrix views[6])
    {
        // Generate sampler.
        SamplerParameters irradianceParams;
        irradianceParams.m_textureParams.m_wrapR = irradianceParams.m_textureParams.m_wrapS = irradianceParams.m_textureParams.m_wrapT = SamplerWrapMode::WRAP_CLAMP_EDGE;
        irradianceParams.m_textureParams.m_magFilter                                                                                   = SamplerFilter::FILTER_LINEAR;
        irradianceParams.m_textureParams.m_minFilter                                                                                   = SamplerFilter::FILTER_LINEAR_MIPMAP_LINEAR;
        irradianceParams.m_textureParams.m_internalPixelFormat                                                                         = PixelFormat::FORMAT_RGB16F;
        irradianceParams.m_textureParams.m_pixelFormat                                                                                 = PixelFormat::FORMAT_RGB;

        // Set resolution
        Vector2 irradianceMapResolution = Vector2(32, 32);

        // Build irradiance texture & scale render buffer according to the resolution.
        m_hdriIrradianceMap.ConstructRTCubemapTexture(irradianceMapResolution, irradianceParams);
        m_renderDevice.SetFBO(m_hdriCaptureRenderTarget.GetID());
        m_renderDevice.ResizeRenderBuffer(m_hdriCaptureRenderTarget.GetID(), m_hdriCaptureRenderBuffer.GetID(), irradianceMapResolution, RenderBufferStorage::STORAGE_DEPTH_COMP24);

        // Build & setup shader info.
        uint32 irradianceShader = m_hdriIrradianceShader->GetID();
        m_renderDevice.SetShader(irradianceShader);
        m_renderDevice.UpdateShaderUniformInt(irradianceShader, MAT_MAP_ENVIRONMENT + String(MAT_EXTENSION_TEXTURE2D), 0);
        m_renderDevice.UpdateShaderUniformInt(irradianceShader, MAT_MAP_ENVIRONMENT + String(MAT_EXTENSION_ISACTIVE), 1);
        m_renderDevice.UpdateShaderUniformMatrix(irradianceShader, UF_MATRIX_PROJECTION, captureProjection);
        m_renderDevice.SetTexture(m_hdriCubemap.GetID(), m_hdriCubemap.GetSamplerID(), 0, TextureBindMode::BINDTEXTURE_CUBEMAP);
        m_renderDevice.SetViewport(Vector2::Zero, irradianceMapResolution);

        // Draw cubemap.
        for (uint32 i = 0; i < 6; ++i)
        {
            m_renderDevice.UpdateShaderUniformMatrix(irradianceShader, UF_MATRIX_VIEW, views[i]);
            m_renderDevice.BindTextureToRenderTarget(m_hdriCaptureRenderTarget.GetID(), m_hdriIrradianceMap.GetID(), TextureBindMode::BINDTEXTURE_CUBEMAP_POSITIVE_X, FrameBufferAttachment::ATTACHMENT_COLOR, 0, i, 0, false, false);
            m_renderDevice.Clear(true, true, true, m_cameraSystem.GetCurrentClearColor(), 0xFF);
            m_renderDevice.Draw(m_hdriCubeVAO, m_defaultDrawParams, 0, 36, true);
        }
    }

    void RenderEngine::CalculateHDRIPrefilter(Matrix& captureProjection, Matrix views[6])
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
        m_renderDevice.UpdateShaderUniformInt(prefilterShader, MAT_MAP_ENVIRONMENT + String(MAT_EXTENSION_TEXTURE2D), 0);
        m_renderDevice.UpdateShaderUniformInt(prefilterShader, MAT_MAP_ENVIRONMENT + String(MAT_EXTENSION_ISACTIVE), 1);
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
            m_renderDevice.ResizeRenderBuffer(m_hdriCaptureRenderTarget.GetID(), m_hdriCaptureRenderBuffer.GetID(), Vector2i((int)mipWidth, (int)mipHeight), RenderBufferStorage::STORAGE_DEPTH_COMP24);
            m_renderDevice.SetViewport(Vector2i(0, 0), Vector2i((int)mipWidth, (int)mipHeight));

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

    void RenderEngine::CalculateHDRIBRDF(Matrix& captureProjection, Matrix views[6])
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
        m_hdriLutMap.ConstructHDRI(samplerParams, brdfLutSize, NULL);

        // Scale render buffer according to the resolution & bind lut map to frame buffer.
        m_renderDevice.ResizeRenderBuffer(m_hdriCaptureRenderTarget.GetID(), m_hdriCaptureRenderBuffer.GetID(), brdfLutSize, RenderBufferStorage::STORAGE_DEPTH_COMP24);
        m_renderDevice.BindTextureToRenderTarget(m_hdriCaptureRenderTarget.GetID(), m_hdriLutMap.GetID(), TextureBindMode::BINDTEXTURE_TEXTURE2D, FrameBufferAttachment::ATTACHMENT_COLOR, 0, 0, 0, true, false);

        // Setup shader.
        uint32 brdfShader = m_hdriBRDFShader->GetID();
        m_renderDevice.SetShader(brdfShader);

        // Switch framebuffer & draw.
        m_renderDevice.SetFBO(m_hdriCaptureRenderTarget.GetID());
        m_renderDevice.SetViewport(Vector2::Zero, brdfLutSize);
        m_renderDevice.Clear(true, true, true, m_cameraSystem.GetCurrentClearColor(), 0xFF);
        m_renderDevice.Draw(m_screenQuadVAO, m_fullscreenQuadDP, 0, 6, true);
    }

    void RenderEngine::SetHDRIData(Material* mat)
    {
        if (mat == nullptr)
        {
            LINA_WARN("Material to set HDRI data is null, returning...");
            return;
        }

        if (m_lastCapturedHDR == nullptr)
        {
            LINA_WARN("HDRI data is not captured, please capture it first then set the material's data.");
            return;
        }

        mat->m_hdriDataSet = true;
        mat->SetTexture(MAT_TEXTURE2D_IRRADIANCEMAP, &m_hdriIrradianceMap, TextureBindMode::BINDTEXTURE_CUBEMAP);
        mat->SetTexture(MAT_TEXTURE2D_BRDFLUTMAP, &m_hdriLutMap, TextureBindMode::BINDTEXTURE_TEXTURE2D);
        mat->SetTexture(MAT_TEXTURE2D_PREFILTERMAP, &m_hdriPrefilterMap, TextureBindMode::BINDTEXTURE_CUBEMAP);
    }

    void RenderEngine::RemoveHDRIData(Material* mat)
    {
        if (mat == nullptr)
        {
            LINA_WARN("Material to remove HDRI data from is null, returning...");
            return;
        }

        mat->RemoveTexture(MAT_TEXTURE2D_IRRADIANCEMAP);
        mat->RemoveTexture(MAT_TEXTURE2D_BRDFLUTMAP);
        mat->RemoveTexture(MAT_TEXTURE2D_PREFILTERMAP);
        mat->m_hdriDataSet = false;
    }

    uint32 RenderEngine::GetFinalImage()
    {
        if (m_appMode == ApplicationMode::Editor)
            return m_secondaryRTTexture.GetID();
        else
            return m_primaryRTTexture0.GetID();
    }

    uint32 RenderEngine::GetShadowMapImage()
    {
        return m_shadowMapRTTexture.GetID();
    }

    void RenderEngine::UpdateSystems(float delta)
    {
        // Update pipeline.
        m_renderingPipeline.UpdateSystems(delta);

        // Update uniform buffers on GPU
        UpdateUniformBuffers();
    }

} // namespace Lina::Graphics
