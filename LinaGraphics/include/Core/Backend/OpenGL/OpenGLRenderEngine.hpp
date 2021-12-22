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

#include "EventSystem/Events.hpp"
#include "Core/Common.hpp"
#include "Rendering/RenderingCommon.hpp"
#include "ECS/ECS.hpp"
#include "ECS/Systems/CameraSystem.hpp"
#include "ECS/Systems/LightingSystem.hpp"
#include "ECS/Systems/AnimationSystem.hpp"
#include "ECS/Systems/MeshRendererSystem.hpp"
#include "ECS/Systems/SpriteRendererSystem.hpp"
#include "Rendering/ModelLoader.hpp"
#include "Rendering/VertexArray.hpp"
#include "Rendering/RenderBuffer.hpp"
#include "Rendering/Model.hpp"
#include "Rendering/UniformBuffer.hpp"
#include "Rendering/RenderContext.hpp"
#include "Core/Backend/OpenGL/OpenGLWindow.hpp"
#include "Math/Color.hpp"
#include "Rendering/RenderSettings.hpp"
#include "Rendering/PostProcessEffect.hpp"
#include <functional>
#include <set>
#include <queue>

namespace Lina
{
	class Engine;

	namespace Event
	{
		class EventSystem;
	}
}

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

		static OpenGLRenderEngine* Get() { return s_renderEngine; }

		void AddToRenderingPipeline(Lina::ECS::BaseECSSystem& system);
		void SetScreenDisplay(Vector2 offset, Vector2 size);
		void SetSkyboxMaterial(Material* skyboxMaterial) { m_skyboxMaterial = skyboxMaterial; }
		void MaterialUpdated(Material& mat);
		void UpdateShaderData(Material* mat);
		void SetDrawParameters(const DrawParams& params);
		void UpdateRenderSettings();
		void* GetFinalImage();
		void* GetShadowMapImage();
		void UpdateSystems(float interpolation);
		void BindShaderToViewBuffer(Shader& shader);
		void BindShaderToDebugBuffer(Shader& shader);
		void BindShaderToLightBuffer(Shader& shader);

		// Initializes the setup process for loading an HDRI image to the scene
		void CaptureCalculateHDRI(Texture& hdriTexture);
		void SetHDRIData(Material* mat);
		void RemoveHDRIData(Material* mat);

		void DrawIcon(Vector3 p, StringIDType textureID, float size = 1.0f);
		void DrawLine(Vector3 p1, Vector3 p2, Color col, float width = 1.0f);
		void ProcessDebugQueue();

		Vector2 GetScreenSize() { return m_screenSize; }
		Vector2 GetScreenPos() { return m_screenPos; }
		ECS::CameraSystem* GetCameraSystem() { return &m_cameraSystem; }
		ECS::LightingSystem* GetLightingSystem() { return &m_lightingSystem; }
		ECS::MeshRendererSystem* GetMeshRendererSystem() { return &m_meshRendererSystem; }
		Texture& GetHDRICubemap() { return m_hdriCubemap; }
		static Texture& GetDefaultTexture() { return s_defaultTexture; }
		static Material& GetDefaultUnlitMaterial() { return s_defaultUnlit; }
		static Shader& GetDefaultShader() { return *s_standardUnlitShader; }
		RenderSettings& GetRenderSettings() { return m_renderSettings; }
		OpenGLRenderDevice* GetRenderDevice() { return &m_renderDevice; }

		DrawParams GetMainDrawParams() { return m_defaultDrawParams; }
		void SetCurrentPLightCount(int count) { m_currentPointLightCount = count; }
		void SetCurrentSLightCount(int count) { m_currentSpotLightCount = count; }
		void DrawSceneObjects(DrawParams& drawpParams, Material* overrideMaterial = nullptr, bool completeFlush = true);
		void DrawSkybox();
		uint32 GetScreenQuadVAO() { return m_screenQuadVAO; }
		PostProcessEffect& AddPostProcessEffect(Shader& shader);
		UniformBuffer& GetViewBuffer() { return m_globalDataBuffer; }


	private:
		friend class Lina::Engine;
		OpenGLRenderEngine() {};
		~OpenGLRenderEngine() {};
		void ConnectEvents();
		void Initialize(ApplicationMode appMode);
		void Shutdown();
		void Tick(float delta);
		void Render(float interpolation);

	private:

		void OnLoadResourceFromFile(Event::ELoadResourceFromFile event);
		void OnLoadResourceFromMemory(Event::ELoadResourceFromMemory event);
		void OnDrawLine(Event::EDrawLine event);
		void OnDrawBox(Event::EDrawBox event);
		void OnDrawCircle(Event::EDrawCircle event);
		void OnDrawSphere(Event::EDrawSphere event);
		void OnDrawHemiSphere(Event::EDrawHemiSphere event);
		void OnDrawCapsule(Event::EDrawCapsule event);
		void OnWindowResized(Event::EWindowResized event);
		bool ValidateEngineShaders();
		void ConstructShader(const std::string& path, unsigned char* data, size_t dataSize);
		void ConstructEngineMaterials();
		void ConstructRenderTargets();
		void DumpMemory();
		void Draw();
		void DrawFinalize();
		void UpdateUniformBuffers();
		
		// Generating necessary maps for HDRI specular highlighting
		void CalculateHDRICubemap(Texture& hdriTexture, glm::mat4& captureProjection, glm::mat4 views[6]);
		void CalculateHDRIIrradiance(Matrix& captureProjection, Matrix views[6]);
		void CalculateHDRIPrefilter(Matrix& captureProjection, Matrix views[6]);
		void CalculateHDRIBRDF(Matrix& captureProjection, Matrix views[6]);
	private:

		static OpenGLRenderEngine* s_renderEngine;
		ApplicationMode m_appMode;
		Event::EventSystem* m_eventSystem;
		OpenGLRenderDevice m_renderDevice;
		OpenGLWindow* m_appWindow;

		RenderTarget m_primaryRenderTarget;
		RenderTarget m_pingPongRenderTarget1;
		RenderTarget m_pingPongRenderTarget2;
		RenderTarget m_hdriCaptureRenderTarget;
		RenderTarget m_shadowMapTarget;
		RenderTarget m_pLightShadowTargets[MAX_POINT_LIGHTS];

		RenderTarget m_secondaryRenderTarget;
		RenderBuffer m_secondaryRenderBuffer;
		Texture m_secondaryRTTexture;

		RenderBuffer m_primaryBuffer;
		RenderBuffer m_hdriCaptureRenderBuffer;

		// Frame buffer texture parameters
		SamplerParameters m_primaryRTParams;
		SamplerParameters m_pingPongRTParams;
		SamplerParameters m_shadowsRTParams;

		Mesh m_quadMesh;

		Material m_screenQuadFinalMaterial;
		Material m_screenQuadBlurMaterial;
		Material m_screenQuadOutlineMaterial;
		Material* m_skyboxMaterial = nullptr;
		Material m_debugLineMaterial;
		Material m_debugIconMaterial;
		Material m_hdriMaterial;
		Material m_shadowMapMaterial;
		Material m_defaultSkyboxMaterial;
		Material m_pLightShadowDepthMaterial;
		static Material s_defaultUnlit;

		Shader* m_hdriBRDFShader = nullptr;
		Shader* m_hdriPrefilterShader = nullptr;
		Shader* m_hdriEquirectangularShader = nullptr;
		Shader* m_hdriIrradianceShader = nullptr;
		Shader* m_sqFinalShader = nullptr;
		Shader* m_sqBlurShader = nullptr;
		Shader* m_sqShadowMapShader = nullptr;
		Shader* m_debugLineShader = nullptr;
		Shader* m_debugIconShader = nullptr;
		Shader* m_skyboxSingleColorShader = nullptr;
		Shader* m_pointShadowsDepthShader = nullptr;
		static Shader* s_standardUnlitShader;

		Texture m_primaryMSAARTTexture0;
		Texture m_primaryMSAARTTexture1;
		RenderTarget m_primaryMSAATarget;
		RenderBuffer m_primaryMSAABuffer;

		Texture m_primaryRTTexture0;
		Texture m_primaryRTTexture1;
		Texture m_pingPongRTTexture1;
		Texture m_pingPongRTTexture2;
		Texture m_hdriCubemap;
		Texture m_hdriIrradianceMap;
		Texture m_hdriPrefilterMap;
		Texture m_HDRILutMap;
		Texture m_shadowMapRTTexture;
		static Texture s_defaultTexture;
		Texture m_defaultCubemapTexture;
		Texture m_pLightShadowTextures[MAX_POINT_LIGHTS];

		DrawParams m_defaultDrawParams;
		DrawParams m_skyboxDrawParams;
		DrawParams m_fullscreenQuadDP;
		DrawParams m_shadowMapDrawParams;

		UniformBuffer m_globalDataBuffer;
		UniformBuffer m_globalLightBuffer;
		UniformBuffer m_globalDebugBuffer;

		RenderingDebugData m_debugData;
		RenderSettings m_renderSettings;

		// Structure that keeps track of current buffer values
		BufferValueRecord m_bufferValueRecord;

		Lina::ECS::AnimationSystem m_animationSystem;
		Lina::ECS::CameraSystem m_cameraSystem;
		Lina::ECS::MeshRendererSystem m_meshRendererSystem;
		Lina::ECS::SpriteRendererSystem m_spriteRendererSystem;
		Lina::ECS::LightingSystem m_lightingSystem;
		Lina::ECS::ECSSystemList m_renderingPipeline;
		Lina::ECS::ECSSystemList m_animationPipeline;

	private:

		uint32 m_skyboxVAO = 0;
		uint32 m_screenQuadVAO = 0;
		uint32 m_hdriCubeVAO = 0;
		uint32 m_lineVAO = 0;

		int m_currentSpotLightCount = 0;
		int m_currentPointLightCount = 0;
		bool m_hdriDataCaptured = false;

		Vector2 m_hdriResolution = Vector2(512, 512);
		Vector2 m_shadowMapResolution = Vector2(2048, 2048);
		Vector2 m_screenPos = Vector2::Zero;
		Vector2 m_screenSize = Vector2::Zero;
		Vector2 m_pLightShadowResolution = Vector2(1024, 1024);

		bool m_firstFrameDrawn = false;

		std::queue<DebugLine> m_debugLineQueue;
		std::queue<DebugIcon> m_debugIconQueue;
		std::map<Shader*, PostProcessEffect> m_postProcessMap;

		DISALLOW_COPY_ASSIGN_MOVE(OpenGLRenderEngine)
	};

}


#endif