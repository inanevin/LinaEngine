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
Class: RenderEngine

Responsible for managing the whole rendering pipeline, creating frame buffers, textures,
materials, meshes etc. Also handles most of the memory management for the rendering pipeline.

Timestamp: 4/15/2019 12:26:31 PM
*/

#pragma once

#ifndef RenderEngine_HPP
#define RenderEngine_HPP

#include "Core/Common.hpp"
#include "RenderingCommon.hpp"
#include "ECS/ECSSystem.hpp"
#include "ECS/Systems/CameraSystem.hpp"
#include "ECS/Systems/LightingSystem.hpp"
#include "ECS/Systems/MeshRendererSystem.hpp"
#include "ECS/Systems/SpriteRendererSystem.hpp"
#include "Rendering/ModelLoader.hpp"
#include "Rendering/VertexArray.hpp"
#include "Rendering/RenderBuffer.hpp"
#include "Mesh.hpp"
#include "UniformBuffer.hpp"
#include "Window.hpp"
#include "RenderContext.hpp"
#include "Utility/Math/Color.hpp"
#include "Core/LayerStack.hpp"
#include <functional>
#include <set>

namespace LinaEngine
{
	namespace Input
	{
		class InputEngine;
	}

	class Event;
}

namespace LinaEngine::Graphics
{
	class Shader;

	struct BufferValueRecord
	{
		float zNear;
		float zFar;
	};

	class RenderEngine
	{
	public:

		RenderEngine();

		~RenderEngine();

		// Setter for post scene draw callback.
		void SetPostSceneDrawCallback(std::function<void()>& cb)
		{
			m_postSceneDrawCallback = cb;
		}

		Vector2 GetViewportSize()
		{
			return m_viewportSize;
		}

		ECS::CameraSystem* GetCameraSystem() { return &m_cameraSystem; }
		Texture& GetHDRICubemap() { return m_hdriCubemap; }

		void SetCurrentPLightCount(int count) { m_currentPointLightCount = count; }
		void SetCurrentSLightCount(int count) { m_currentSpotLightCount = count; }
		void Initialize(LinaEngine::ECS::ECSRegistry& ecsIn, Window& appWindow);
		void Render();
		void RenderLayers();
		void Swap();

		// Sets the viewport offset & display size
		void SetViewportDisplay(Vector2 offset, Vector2 size);

		Material& CreateMaterial(Shaders shader, const std::string& path = "");
		Material& LoadMaterialFromFile(const std::string& path = "");
		void MaterialUpdated(Material& mat);
		Texture& CreateTexture2D(const std::string& filePath, SamplerParameters samplerParams = SamplerParameters(), bool compress = false, bool useDefaultFormats = false, const std::string& paramsPath = "");
		Texture& CreateTextureHDRI(const std::string filePath);
		Mesh& CreateMesh (const std::string& filePath, MeshParameters meshParams = MeshParameters(), int id = -1, const std::string& paramsPath = "");
		Shader& CreateShader(Shaders shader, const std::string& path, bool usesGeometryShader = false);
		Material& GetMaterial(int id);
		Material& GetMaterial(const std::string& path);
		Texture& GetTexture(int id);
		Texture& GetTexture(const std::string& path);
		Mesh& GetMesh(int id);
		Mesh& GetMesh(const std::string& path);
		Shader& GetShader(Shaders shader);
		Mesh& GetPrimitive(Primitives primitive);
		void UnloadTextureResource(int id);
		void UnloadMeshResource(int id);
		void UnloadMaterialResource(int id);
		bool MaterialExists(int id);
		bool MaterialExists(const std::string& path);
		bool TextureExists(int id);
		bool TextureExists(const std::string& path);
		bool MeshExists(int id);
		bool MeshExists(const std::string& path);
		bool ShaderExists(Shaders shader);

		// Sets the shader of a material to the shader specified by name. Also resets material properties based on the shader, caution!
		Material& SetMaterialShader(Material& material, Shaders shader);
		void SetMaterialContainers(Material& material);
		void SetSkyboxMaterial(Material& skyboxMaterial) { m_skyboxMaterial = &skyboxMaterial; }
		void PushLayer(Layer& layer);
		void PushOverlay(Layer& layer);

		// Updates shader uniforms with material data.
		void UpdateShaderData(Material* mat);

		// Returns the final render texture.
		void* GetFinalImage();

		// Returns the directional shadow map texture
		void* GetShadowMapImage();

		// Initializes the setup process for loading an HDRI image to the scene
		void CaptureCalculateHDRI(Texture& hdriTexture);
		void SetHDRIData(Material* mat);
		void RemoveHDRIData(Material* mat);
		void DrawLine(Vector3 p1, Vector3 p2, Color col, float width = 1.0f);

		// Commands the render device to put the params in place.
		void SetDrawParameters(const DrawParams& params);

		std::map<int, Mesh>& GetLoadedMeshes() { return m_loadedMeshes; }
		std::map<int, Material>& GetLoadedMaterials()  { return m_loadedMaterials; }
	private:

		void ConstructEngineShaders();
		bool ValidateEngineShaders();
		void ConstructEngineMaterials();
		void ConstructEnginePrimitives();
		void ConstructRenderTargets();
		void SetupDrawParameters();
		void DumpMemory();
		void DrawShadows();
		void Draw();
		void DrawOperationsDefault();
		void DrawSkybox();
		void DrawSceneObjects(DrawParams& drawpParams, Material* overrideMaterial = nullptr, bool drawSkybox = true);
		void UpdateUniformBuffers();

		// Generating necessary maps for HDRI specular highlighting
		void CalculateHDRICubemap(Texture& hdriTexture, glm::mat4& captureProjection, glm::mat4 views[6]);
		void CalculateHDRIIrradiance(Matrix& captureProjection, Matrix views[6]);
		void CalculateHDRIPrefilter(Matrix& captureProjection, Matrix views[6]);
		void CalculateHDRIBRDF(Matrix& captureProjection, Matrix views[6]);


	private:

		RenderDevice m_renderDevice;
		Window* m_appWindow;

		RenderTarget m_primaryRenderTarget;
		RenderTarget m_pingPongRenderTarget1;
		RenderTarget m_pingPongRenderTarget2;
		RenderTarget m_outlineRenderTarget;
		RenderTarget m_hdriCaptureRenderTarget;
		RenderTarget m_shadowMapTarget;

		RenderBuffer m_primaryRenderBuffer;
		RenderBuffer m_hdriCaptureRenderBuffer;

		// Frame buffer texture parameters
		SamplerParameters m_mainRTParams;
		SamplerParameters m_primaryRTParams;
		SamplerParameters m_pingPongRTParams;
		SamplerParameters m_shadowsRTParams;

		Material m_screenQuadFinalMaterial;
		Material m_screenQuadBlurMaterial;
		Material m_screenQuadOutlineMaterial;
		Material* m_skyboxMaterial = nullptr;
		Material m_debugDrawMaterial;
		Material m_hdriMaterial;
		Material m_shadowMapMaterial;
		Material m_defaultSkyboxMaterial;
		Material m_defaultUnlit;

		Texture m_primaryRTTexture0;
		Texture m_primaryRTTexture1;
		Texture m_pingPongRTTexture1;
		Texture m_pingPongRTTexture2;
		Texture m_outlineRTTexture;
		Texture m_hdriCubemap;
		Texture m_hdriIrradianceMap;
		Texture m_hdriPrefilterMap;
		Texture m_HDRILutMap;
		Texture m_shadowMapRTTexture;
		Texture m_defaultTexture;
		Texture m_defaultCubemapTexture;

		DrawParams m_defaultDrawParams;
		DrawParams m_skyboxDrawParams;
		DrawParams m_fullscreenQuadDP;
		DrawParams m_shadowMapDrawParams;

		UniformBuffer m_globalDataBuffer;
		UniformBuffer m_globalLightBuffer;
		UniformBuffer m_globalDebugBuffer;

		LayerStack m_guiLayerStack;
		RenderingDebugData m_debugData;

		// Structure that keeps track of current buffer values
		BufferValueRecord m_bufferValueRecord;

		LinaEngine::ECS::CameraSystem m_cameraSystem;
		LinaEngine::ECS::MeshRendererSystem m_meshRendererSystem;
		LinaEngine::ECS::SpriteRendererSystem m_spriteRendererSystem;
		LinaEngine::ECS::LightingSystem m_lightingSystem;
		LinaEngine::ECS::ECSSystemList m_renderingPipeline;

		std::map<int, Texture*> m_loadedTextures;
		std::map<int, Mesh> m_loadedMeshes;
		std::map<int, Material> m_loadedMaterials;
		std::map<int, Shader> m_loadedShaders;
		std::set<Material*> m_shadowMappedMaterials;
		std::set<Material*> m_hdriMaterials;

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
		Vector2 m_viewportPos = Vector2::Zero;
		Vector2 m_viewportSize = Vector2::Zero;

		std::function<void()> m_postSceneDrawCallback;
		bool m_firstFrameDrawn = false;


		DISALLOW_COPY_ASSIGN_MOVE(RenderEngine)
	};

}


#endif