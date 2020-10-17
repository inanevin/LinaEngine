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

		// Setters for light counts. /* TAKE INSIDE LINA API LATER ON */
		void SetCurrentPLightCount(int count) { m_currentPointLightCount = count; }
		void SetCurrentSLightCount(int count) { m_currentSpotLightCount = count; }

		// Get hdri cubemap data.
		Texture& GetHDRICubemap() { return m_hdriCubemap; }

		// Initialize the render renderEngine.
		void Initialize(LinaEngine::ECS::ECSRegistry& ecsIn, Window& appWindow);

		// Called each frame.
		void Render();

		// Updates the layers & swaps buffers
		void TickAndSwap(float frameTime);

		// Sets the viewport offset & display size
		void SetViewportDisplay(Vector2 offset, Vector2 size);

		/// <summary> Creates a material resource with a specific shader. </summary>
		Material& CreateMaterial(int id, Shaders shader);

		/// <summary> Creates a texture resource. </summary>
		Texture& CreateTexture2D(const std::string& filePath, SamplerParameters samplerParams = SamplerParameters(), bool compress = false, bool useDefaultFormats = false);

		/// <summary> Creates an HDRI texture resource. </summary>
		Texture& CreateTextureHDRI(const std::string filePath);

		/// <summary> Creates a mesh resource. </summary>
		Mesh& CreateMesh(int id, const std::string& filePath, MeshParameters meshParams = MeshParameters());

		/// <summary> Creates a primitive resource. </summary>
		Mesh& CreatePrimitive(Primitives primitive, const std::string& path);

		/// <summary> Creates a shader resource. </summary>
		Shader& CreateShader(Shaders shader, const std::string& path, bool usesGeometryShader = false);

		// Returns a material resource.
		Material& GetMaterial(int id);

		// Returns a texture resource
		Texture& GetTexture(int id);

		// Return texture resource from the path.
		Texture& GetTexture(const std::string& path);

		// Returns a mesh resource.
		Mesh& GetMesh(int id);

		// Returns a mesh resource from the path.
		Mesh& GetMesh(const std::string& path);

		// Returns a shader resource.
		Shader& GetShader(Shaders shader);

		// Returns a vertex array resource
		Mesh& GetPrimitive(Primitives primitive);

		// Removes the targeted resource from resource map.
		void UnloadTextureResource(int id);

		// Removes the targeted resource from resource map.
		void UnloadMeshResource(int id);

		// Removes the targeted resource from resource map.
		void UnloadMaterialResource(int id);

		// Returns whether a material/texture/mesh/shader exists or not.
		bool MaterialExists(int id);
		bool TextureExists(int id);
		bool MeshExists(int id);
		bool ShaderExists(Shaders shader);

		// Sets the shader of a material to the shader specified by name. Also resets material properties based on the shader, caution!
		Material& SetMaterialShader(Material& material, Shaders shader);

		// Sets the skybox material.
		void SetSkyboxMaterial(Material& skyboxMaterial) { m_skyboxMaterial = &skyboxMaterial; }

		// Pushes a new layer into the gui stack.
		void PushLayer(Layer* layer);

		// Pushes a new overlay layer into the gui stack.
		void PushOverlay(Layer* layer);

		// Updates shader uniforms with material data.
		void UpdateShaderData(Material* mat);

		// Returns the final render texture.
		void* GetFinalImage();

		// Returns the directional shadow map texture
		void* GetShadowMapImage();

		// Initializes the setup process for loading an HDRI image to the scene
		void CaptureCalculateHDRI(Texture& hdriTexture);

		// Set & remove the HDRI data for a material
		void SetHDRIData(Material* mat);
		void RemoveHDRIData(Material* mat);

		// Draws a line bw 2 points.
		void DrawLine(Vector3 p1, Vector3 p2, Color col, float width = 1.0f);

		// Commands the render device to put the params in place.
		void SetDrawParameters(const DrawParams& params);

	private:

		// Constructs commonly used shaders within Lina Engine.
		void ConstructEngineShaders();

		// Checks if engine shaders contain errors.
		bool ValidateEngineShaders();

		// Constructs engine materials
		void ConstructEngineMaterials();

		// Constructs engine vertex arrays
		void ConstructEnginePrimitives();

		// Construct frame buffers
		void ConstructRenderTargets();

		// Sets up draw parameters.
		void SetupDrawParameters();

		// clears resource memory.
		void DumpMemory();

		// Renders the shadow depth buffer
		void DrawShadows();

		// Renders the scene.
		void Draw();

		// Default drawing
		void DrawOperationsDefault();

		// Renders skybox
		void DrawSkybox();

		// Renders scene objects.
		void DrawSceneObjects(DrawParams& drawpParams, Material* overrideMaterial = nullptr, bool drawSkybox = true);

		// Updates related uniform buffers on GPU
		void UpdateUniformBuffers();

		// Generating necessary maps for HDRI specular highlighting
		void CalculateHDRICubemap(Texture& hdriTexture, glm::mat4& captureProjection, glm::mat4 views[6]);
		void CalculateHDRIIrradiance(Matrix& captureProjection, Matrix views[6]);
		void CalculateHDRIPrefilter(Matrix& captureProjection, Matrix views[6]);
		void CalculateHDRIBRDF(Matrix& captureProjection, Matrix views[6]);


	private:

		// Device for rendering operations.
		RenderDevice m_renderDevice;

		// Context window
		Window* m_appWindow;

		// Primary render target
		RenderTarget m_primaryRenderTarget;

		// Ping-pong render targets
		RenderTarget m_pingPongRenderTarget1;
		RenderTarget m_pingPongRenderTarget2;

		// Outline render target
		RenderTarget m_outlineRenderTarget;

		// Equirectangular hdri capture render target
		RenderTarget m_hdriCaptureRenderTarget;

		// Shadows depth map
		RenderTarget m_shadowMapTarget;

		// Default Render buffer
		RenderBuffer m_primaryRenderBuffer;

		// Equirectangular hdri capture render buffer
		RenderBuffer m_hdriCaptureRenderBuffer;

		// quad mateiral.
		Material m_screenQuadFinalMaterial;

		// Screen quad blur material
		Material m_screenQuadBlurMaterial;

		// Screen quad outline material
		Material m_screenQuadOutlineMaterial;

		// Material used to draw skybox.
		Material* m_skyboxMaterial = nullptr;

		// Material for debug drawing
		Material m_debugDrawMaterial;

		// HDRI equirectangular cube material
		Material m_hdriMaterial;

		// Shadow map material
		Material m_shadowMapMaterial;

		// Dummy skybox
		Material m_defaultSkyboxMaterial;

		// Default unlit material
		Material m_defaultUnlit;

		// Primary RT Texture
		Texture m_primaryRTTexture0;
		Texture m_primaryRTTexture1;

		// Ping pong RT textures
		Texture m_pingPongRTTexture1;
		Texture m_pingPongRTTexture2;

		// Outline RT Texture
		Texture m_outlineRTTexture;

		// Cubemap texture for HDRI skybox
		Texture m_hdriCubemap;

		// Irradiance cubemap for HDRI skybox
		Texture m_hdriIrradianceMap;

		// Prefilter map for HDRI skybox
		Texture m_hdriPrefilterMap;

		// BRDF LUT texture for HDRI
		Texture m_HDRILutMap;

		// Shadow map RT texture
		Texture m_shadowMapRTTexture;

		// Default texture
		Texture m_defaultTexture;

		// Default empty cubemap texture
		Texture m_defaultCubemapTexture;

		// Default drawing parameters.
		DrawParams m_defaultDrawParams;

		// Skybox drawing parameters.
		DrawParams m_skyboxDrawParams;

		// Draw parameters for fbo texture rendering
		DrawParams m_fullscreenQuadDP;

		// Shadow map draw params
		DrawParams m_shadowMapDrawParams;

		// Buffer for global matrices
		UniformBuffer m_globalDataBuffer;

		// Buffer for lights.
		UniformBuffer m_globalLightBuffer;

		// Buffer for debugging.
		UniformBuffer m_globalDebugBuffer;

		// GUI layer queue.
		LayerStack m_guiLayerStack;

		// Debug data
		RenderingDebugData m_debugData;

		// Structure that keeps track of current buffer values
		BufferValueRecord m_bufferValueRecord;

		// ECS system for rendering camera perspective.
		LinaEngine::ECS::CameraSystem m_cameraSystem;

		// ECS Mesh Renderer system
		LinaEngine::ECS::MeshRendererSystem m_meshRendererSystem;

		// ECS Sprite rendere system
		LinaEngine::ECS::SpriteRendererSystem m_spriteRendererSystem;

		// ECS system for handling lighting
		LinaEngine::ECS::LightingSystem m_lightingSystem;

		// ECS system list for rendering operations.
		LinaEngine::ECS::ECSSystemList m_renderingPipeline;

		// Texture resources.
		std::map<int, Texture*> m_loadedTextures;

		// Model resources
		std::map<int, Mesh> m_loadedMeshes;

		// Material storage
		std::map<int, Material> m_loadedMaterials;

		// Map that stores shader ID's by name
		std::map<int, Shader> m_loadedShaders;

		// List that stores the materials to receive shadows.
		std::set<Material*> m_shadowMappedMaterials;

	private:

		// Standart Skybox vertex array object.
		uint32 m_skyboxVAO = 0;

		// Quad to draw the buffers into.
		uint32 m_screenQuadVAO = 0;

		// HDRI Cubemap vao
		uint32 m_hdriCubeVAO = 0;

		// VAO for drawing lines
		uint32 m_lineVAO = 0;

		// Light counts.
		int m_currentSpotLightCount = 0;
		int m_currentPointLightCount = 0;

		// Whether hdri data is captured & calculated or not.
		bool m_hdriDataCaptured = false;

		// HDRI Skybox resolution
		Vector2 m_hdriResolution = Vector2(512, 512);

		// Shadow map resolution
		Vector2 m_shadowMapResolution = Vector2(2048, 2048);

		// Viewport data
		Vector2 m_viewportPos;
		Vector2 m_viewportSize;

		// Frame buffer texture parameters
		SamplerParameters m_mainRTParams;
		SamplerParameters m_primaryRTParams;
		SamplerParameters m_pingPongRTParams;
		SamplerParameters m_shadowsRTParams;

		// Post scene draw callback
		std::function<void()> m_postSceneDrawCallback;

		// Flag to determine the first frame drawn.
		bool m_firstFrameDrawn = false;
	};

}


#endif