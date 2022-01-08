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

/*
Class: OpenGLRenderEngine

Responsible for managing the whole rendering pipeline, creating frame buffers, textures,
materials, meshes etc. Also handles most of the memory management for the rendering pipeline.

Timestamp: 4/15/2019 12:26:31 PM
*/

#pragma once

#ifndef RenderEngine_HPP
#define RenderEngine_HPP

#include "ECS/SystemList.hpp"
#include "ECS/Systems/AnimationSystem.hpp"
#include "ECS/Systems/CameraSystem.hpp"
#include "ECS/Systems/FrustumSystem.hpp"
#include "ECS/Systems/LightingSystem.hpp"
#include "ECS/Systems/ModelNodeSystem.hpp"
#include "ECS/Systems/SpriteRendererSystem.hpp"
#include "OpenGLRenderDevice.hpp"
#include "OpenGLWindow.hpp"
#include "Rendering/Mesh.hpp"
#include "Rendering/Model.hpp"
#include "Rendering/PostProcessEffect.hpp"
#include "Rendering/RenderBuffer.hpp"
#include "Rendering/RenderSettings.hpp"
#include "Rendering/RenderingCommon.hpp"
#include "Rendering/UniformBuffer.hpp"
#include "Rendering/VertexArray.hpp"

#include <functional>
#include <queue>
#include <set>

namespace Lina
{
    class Engine;

    namespace Resources
    {
        class ResourceStorage;
    }

    namespace Event
    {
        class EventSystem;
        struct EAllResourcesOfTypeLoaded;
        struct EDrawLine;
        struct EDrawBox;
        struct EDrawCircle;
        struct EDrawSphere;
        struct EDrawHemiSphere;
        struct EDrawCapsule;
        struct EWindowResized;
    } // namespace Event
} // namespace Lina

namespace Lina::Graphics
{
    class Shader;

    struct BufferValueRecord
    {
        float zNear;
        float zFar;
    };

    class OpenGLRenderEngine
    {
    public:
        static OpenGLRenderEngine* Get()
        {
            return s_renderEngine;
        }

        /// <summary>
        /// Any system added to the rendering pipeline will be updated within the render loop.
        /// </summary>
        void AddToRenderingPipeline(ECS::System& system);

        /// <summary>
        /// Any system added to the animation pipeline will be updated within the animation loop.
        /// </summary>
        void AddToAnimationPipeline(ECS::System& system);

        /// <summary>
        /// Sets the screen position and size, resizes the framebuffers accordingly.
        /// </summary>
        void SetScreenDisplay(Vector2i offset, Vector2i size);

        /// <summary>
        /// Use this to change current draw parameters. Will be replaced with in-engine parameters every frame.
        /// </summary>
        /// <param name="params"></param>
        void SetDrawParameters(const DrawParams& params);

        /// <summary>
        /// Sets uniform render settings from the current Render Settings struct.
        /// </summary>
        void UpdateRenderSettings();

        /// <summary>
        /// Allows the shader to receive data from the View Uniform Buffer, such as projection & view matrices.
        /// </summary>
        void BindShaderToViewBuffer(Shader& shader);

        /// <summary>
        /// Allows the shader to receive data from the Debug Uniform Buffer, such as depth information.
        /// </summary>
        void BindShaderToDebugBuffer(Shader& shader);

        /// <summary>
        /// Allows the shader to receive data from the Light Uniform Buffer, such as light positions & parameters.
        /// </summary>
        void BindShaderToLightBuffer(Shader& shader);

        /// <summary>
        /// Sets the current skybox material used to draw the scene.
        /// </summary>
        inline void SetSkyboxMaterial(Material* skyboxMaterial)
        {
            m_skyboxMaterial = skyboxMaterial;
        }

        /// <summary>
        /// Returns a texture pointer to the final drawed image, which is usually drawn to the screen as a full-screen quad.
        /// </summary>
        uint32 GetFinalImage();

        /// <summary>
        /// Returns a texture pointer to the current shadow map.
        /// </summary>
        uint32 GetShadowMapImage();

        /// <summary>
        /// Given an HDRI loaded texture, captures & calculates HDRI cube probes & writes it into global HRDI buffer.
        /// This buffer will be sent to the materials whose HDRI support is enabled.
        /// </summary>
        void CaptureCalculateHDRI(Texture& hdriTexture);

        /// <summary>
        /// Add the texture with the given StringID to the debug icon buffer.
        /// </summary>
        void DrawIcon(Vector3 p, StringIDType textureID, float size = 1.0f);

        /// <summary>
        /// Adds a line to the debug buffer with given parameters.
        /// </summary>
        void DrawLine(Vector3 p1, Vector3 p2, Color col, float width = 1.0f);

        /// <summary>
        /// Pass in any run-time constructed shader. The shader will be drawn to a full-screen quad & added as a
        /// post-process effect.
        /// </summary>
        PostProcessEffect& AddPostProcessEffect(Shader* shader);

        /// <summary>
        /// Renders the given model in a preview scene and returns the resulting image.
        /// </summary>
        uint32 RenderModelPreview(Model* model);

        void MaterialUpdated(Material& mat);
        void UpdateShaderData(Material* mat);

        inline UniformBuffer& GetViewBuffer()
        {
            return m_globalDataBuffer;
        }
        inline DrawParams GetMainDrawParams()
        {
            return m_defaultDrawParams;
        }
        inline OpenGLRenderDevice* GetRenderDevice()
        {
            return &m_renderDevice;
        }
        inline Texture& GetHDRICubemap()
        {
            return m_hdriCubemap;
        }
        inline uint32 GetScreenQuadVAO()
        {
            return m_screenQuadVAO;
        }
        inline Vector2i GetScreenSize()
        {
            return m_screenSize;
        }
        inline Vector2i GetScreenPos()
        {
            return m_screenPos;
        }
        inline ECS::CameraSystem* GetCameraSystem()
        {
            return &m_cameraSystem;
        }
        inline ECS::LightingSystem* GetLightingSystem()
        {
            return &m_lightingSystem;
        }
        inline ECS::ModelNodeSystem* GetModelNodeSystem()
        {
            return &m_modelNodeSystem;
        }
        inline ECS::FrustumSystem* GetFrustumSystem()
        {
            return &m_frustumSystem;
        }
        inline Texture* GetDefaultTexture()
        {
            return &m_defaultTexture;
        }
        inline Material* GetDefaultUnlitMaterial()
        {
            return m_defaultUnlit;
        }
        inline Material* GetDefaultLitMaterial()
        {
            return m_defaultLit;
        }
        inline Material* GetDefaultSkyboxMaterial()
        {
            return m_defaultSkybox;
        }
        inline Material* GetDefaultSpriteMaterial()
        {
            return m_defaultSprite;
        }
        inline Shader* GetDefaultUnlitShader()
        {
            return m_standardUnlitShader;
        }
        inline Shader* GetDefaultLitShader()
        {
            return m_standardLitShader;
        }
        inline void SetCurrentPLightCount(int count)
        {
            m_currentPointLightCount = count;
        }
        inline void SetCurrentSLightCount(int count)
        {
            m_currentSpotLightCount = count;
        }
        inline const ECS::SystemList& GetRenderingPipeline()
        {
            return m_renderingPipeline;
        }
        inline const ECS::SystemList& GetAnimationPipeline()
        {
            return m_animationPipeline;
        }

    private:
        friend class Engine;
        OpenGLRenderEngine()  = default;
        ~OpenGLRenderEngine() = default;
        void ConnectEvents();
        void Initialize(ApplicationMode appMode, RenderSettings* renderSettings);
        void Shutdown();
        void Tick(float delta);
        void Render(float interpolation);
        void UpdateSystems(float interpolation);
        void DrawSceneObjects(DrawParams& drawpParams, Material* overrideMaterial = nullptr, bool completeFlush = true);
        void DrawSkybox();
        void SetHDRIData(Material* mat);
        void RemoveHDRIData(Material* mat);
        void ProcessDebugQueue();

    private:
        void OnDrawLine(const Event::EDrawLine& event);
        void OnDrawBox(const Event::EDrawBox& event);
        void OnDrawCircle(const Event::EDrawCircle& event);
        void OnDrawSphere(const Event::EDrawSphere& event);
        void OnDrawHemiSphere(const Event::EDrawHemiSphere& event);
        void OnDrawCapsule(const Event::EDrawCapsule& event);
        void OnWindowResized(const Event::EWindowResized& event);
        void OnAllResourcesOfTypeLoaded(const Event::EAllResourcesOfTypeLoaded& ev);
        bool ValidateEngineShaders();
        void SetupEngineShaders();
        void ConstructEngineMaterials();
        void ConstructRenderTargets();
        void DumpMemory();
        void Draw();
        void DrawFinalize(bool finalizeForPreview = false);
        void UpdateUniformBuffers();
        void CalculateHDRICubemap(Texture& hdriTexture, glm::mat4& captureProjection, glm::mat4 views[6]);
        void CalculateHDRIIrradiance(Matrix& captureProjection, Matrix views[6]);
        void CalculateHDRIPrefilter(Matrix& captureProjection, Matrix views[6]);
        void CalculateHDRIBRDF(Matrix& captureProjection, Matrix views[6]);

    private:
        static OpenGLRenderEngine* s_renderEngine;
        ApplicationMode            m_appMode   = ApplicationMode::Editor;
        OpenGLWindow*              m_appWindow = nullptr;
        OpenGLRenderDevice         m_renderDevice;
        Event::EventSystem*        m_eventSystem = nullptr;

        RenderTarget m_primaryRenderTarget;
        RenderTarget m_secondaryRenderTarget;
        RenderTarget m_previewRenderTarget;
        RenderTarget m_primaryMSAATarget;
        RenderTarget m_pingPongRenderTarget1;
        RenderTarget m_pingPongRenderTarget2;
        RenderTarget m_hdriCaptureRenderTarget;
        RenderTarget m_shadowMapTarget;
        RenderTarget m_pLightShadowTargets[MAX_POINT_LIGHTS];

        RenderBuffer m_primaryBuffer;
        RenderBuffer m_primaryMSAABuffer;
        RenderBuffer m_secondaryRenderBuffer;
        RenderBuffer m_previewRenderBuffer;
        RenderBuffer m_hdriCaptureRenderBuffer;

        Texture m_primaryMSAARTTexture0;
        Texture m_primaryMSAARTTexture1;
        Texture m_primaryRTTexture0;
        Texture m_primaryRTTexture1;
        Texture m_secondaryRTTexture;
        Texture m_previewRTTexture;
        Texture m_pingPongRTTexture1;
        Texture m_pingPongRTTexture2;
        Texture m_hdriCubemap;
        Texture m_hdriIrradianceMap;
        Texture m_hdriPrefilterMap;
        Texture m_HDRILutMap;
        Texture m_shadowMapRTTexture;
        Texture m_defaultCubemapTexture;
        Texture m_pLightShadowTextures[MAX_POINT_LIGHTS];
        Texture m_defaultTexture;

        // Frame buffer texture parameters
        SamplerParameters m_primaryRTParams;
        SamplerParameters m_pingPongRTParams;
        SamplerParameters m_shadowsRTParams;

        Mesh m_quadMesh;

        Material  m_screenQuadFinalMaterial;
        Material  m_screenQuadBlurMaterial;
        Material  m_screenQuadOutlineMaterial;
        Material* m_skyboxMaterial = nullptr;
        Material  m_debugLineMaterial;
        Material  m_debugIconMaterial;
        Material  m_hdriMaterial;
        Material  m_shadowMapMaterial;
        Material  m_defaultSkyboxMaterial;
        Material  m_pLightShadowDepthMaterial;
        Material* m_defaultUnlit  = nullptr;
        Material* m_defaultLit    = nullptr;
        Material* m_defaultSkybox = nullptr;
        Material* m_defaultSprite = nullptr;

        Shader* m_hdriBRDFShader            = nullptr;
        Shader* m_hdriPrefilterShader       = nullptr;
        Shader* m_hdriEquirectangularShader = nullptr;
        Shader* m_hdriIrradianceShader      = nullptr;
        Shader* m_sqFinalShader             = nullptr;
        Shader* m_sqBlurShader              = nullptr;
        Shader* m_sqShadowMapShader         = nullptr;
        Shader* m_debugLineShader           = nullptr;
        Shader* m_debugIconShader           = nullptr;
        Shader* m_skyboxSingleColorShader   = nullptr;
        Shader* m_pointShadowsDepthShader   = nullptr;
        Shader* m_standardUnlitShader;
        Shader* m_standardLitShader;

        DrawParams m_defaultDrawParams;
        DrawParams m_skyboxDrawParams;
        DrawParams m_fullscreenQuadDP;
        DrawParams m_shadowMapDrawParams;

        UniformBuffer m_globalDataBuffer;
        UniformBuffer m_globalLightBuffer;
        UniformBuffer m_globalDebugBuffer;

        RenderingDebugData m_debugData;
        RenderSettings*    m_renderSettings;

        // Structure that keeps track of current buffer values
        BufferValueRecord m_bufferValueRecord;

        ECS::AnimationSystem        m_animationSystem;
        ECS::CameraSystem           m_cameraSystem;
        ECS::ModelNodeSystem        m_modelNodeSystem;
        ECS::SpriteRendererSystem   m_spriteRendererSystem;
        ECS::LightingSystem         m_lightingSystem;
        ECS::FrustumSystem          m_frustumSystem;
        ECS::SystemList             m_renderingPipeline;
        ECS::SystemList             m_animationPipeline;
        Resources::ResourceStorage* m_storage = nullptr;

    private:
        uint32 m_skyboxVAO     = 0;
        uint32 m_screenQuadVAO = 0;
        uint32 m_hdriCubeVAO   = 0;
        uint32 m_lineVAO       = 0;

        int  m_currentSpotLightCount  = 0;
        int  m_currentPointLightCount = 0;
        bool m_hdriDataCaptured       = false;

        Vector2i m_hdriResolution         = Vector2i(512, 512);
        Vector2i m_shadowMapResolution    = Vector2i(2048, 2048);
        Vector2i m_screenPos              = Vector2i(0, 0);
        Vector2i m_screenSize             = Vector2i(0, 0);
        Vector2i m_pLightShadowResolution = Vector2i(1024, 1024);
        bool     m_firstFrameDrawn        = false;

        std::queue<DebugLine>                m_debugLineQueue;
        std::queue<DebugIcon>                m_debugIconQueue;
        std::map<Shader*, PostProcessEffect> m_postProcessMap;
    };

} // namespace Lina::Graphics

#endif
