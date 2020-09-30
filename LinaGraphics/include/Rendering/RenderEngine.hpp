/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: RenderEngine
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
#include "Rendering/ModelLoader.hpp"
#include "Rendering/VertexArray.hpp"
#include "Rendering/RenderBuffer.hpp"
#include "Mesh.hpp"
#include "UniformBuffer.hpp"
#include "Window.hpp"
#include "RenderContext.hpp"
#include "Utility/Math/Color.hpp"
#include <functional>
#include <set>
#include "Core/LayerStack.hpp"

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
		FORCEINLINE void SetPostSceneDrawCallback(std::function<void()>& cb)
		{
			m_postSceneDrawCallback = cb;
		}

		// Creates an GLFW window.
		FORCEINLINE bool CreateContextWindow()
		{
			return m_MainWindow.Initialize();
		}

		// Returns GLFW window instance.
		FORCEINLINE void* GetNativeWindow()
		{
			return m_MainWindow.GetNativeWindow();
		}

		// Swaps window buffer.
		FORCEINLINE void TickWindow()
		{
			m_MainWindow.Tick();
		}

		// Get main window.
		FORCEINLINE Window& GetMainWindow() { return m_MainWindow; }

		// Returns the window width & height
		FORCEINLINE Vector2 GetWindowSize()
		{
			return Vector2(m_MainWindow.GetWidth(), m_MainWindow.GetHeight());
		};

		// Returns the window center coordinates.
		FORCEINLINE Vector2 GetWindowCenter()
		{
			return Vector2(m_MainWindow.GetWidth() / 2.0f, m_MainWindow.GetHeight() / 2.0f);
		}

		FORCEINLINE ECS::CameraSystem* GetCameraSystem() { return &m_CameraSystem; }

		// Setters for light counts. /* TAKE INSIDE LINA API LATER ON */
		FORCEINLINE void SetCurrentPLightCount(int count) { m_CurrentPointLightCount = count; }
		FORCEINLINE void SetCurrentSLightCount(int count) { m_CurrentSpotLightCount = count; }

		// Get hdri cubemap data.
		FORCEINLINE Texture& GetHDRICubemap() { return m_HDRICubemap; }

		// Initialize the render renderEngine.
		void Initialize(LinaEngine::ECS::ECSRegistry& ecsIn);

		// Called before the first run.
		void PostInitialFrame();

		// Called each frame.
		void Render();

		// Called when the main window is resized.
		void OnWindowResized(float width, float height);

		/// <summary> Creates a material resource with a specific shader. </summary>
		Material& CreateMaterial(int id, Shaders shader);

		/// <summary> Creates a texture resource. </summary>
		Texture& CreateTexture2D(int id, const std::string& filePath, SamplerParameters samplerParams = SamplerParameters(), bool compress = false, bool useDefaultFormats = false);

		/// <summary> Creates an HDRI texture resource. </summary>
		Texture& CreateTextureHDRI(int id, const std::string filePath);

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

		// Returns a mesh resource.
		Mesh& GetMesh(int id);

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
		FORCEINLINE void SetSkyboxMaterial(Material& skyboxMaterial) { m_SkyboxMaterial = &skyboxMaterial; }

		// Pushes a new layer into the gui stack.
		void PushLayer(Layer* layer);

		// Pushes a new overlay layer into the gui stack.
		void PushOverlay(Layer* layer);

		// Updates shader uniforms with material data.
		void UpdateShaderData(Material* mat);

		// Returns the final render texture.
		void* GetFinalImage();

		// Initializes the setup process for loading an HDRI image to the scene
		void CaptureCalculateHDRI(Texture& hdriTexture);

		// Set & remove the HDRI data for a material
		void SetHDRIData(Material* mat);
		void RemoveHDRIData(Material* mat);

		// Draws a line bw 2 points.
		void DrawLine(Vector3 p1, Vector3 p2, Color col, float width = 1.0f);

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

		// Renders the scene.
		void Draw();

		// Default drawing
		void DrawOperationsDefault(float delta);

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
		RenderDevice m_RenderDevice;

		// Context window
		Window m_MainWindow;

		// Primary render target
		RenderTarget m_PrimaryRenderTarget;

		// Ping-pong render targets
		RenderTarget m_PingPongRenderTarget1;
		RenderTarget m_PingPongRenderTarget2;

		// Outline render target
		RenderTarget m_OutlineRenderTarget;

		// Equirectangular hdri capture render target
		RenderTarget m_HDRICaptureRenderTarget;

		// Default Render buffer
		RenderBuffer m_PrimaryRenderBuffer;

		// Equirectangular hdri capture render buffer
		RenderBuffer m_HDRICaptureRenderBuffer;

		// quad mateiral.
		Material m_ScreenQuadFinalMaterial;

		// Screen quad blur material
		Material m_ScreenQuadBlurMaterial;

		// Screen quad outline material
		Material m_ScreenQuadOutlineMaterial;

		// Material used to draw skybox.
		Material* m_SkyboxMaterial = nullptr;

		// Material for debug drawing
		Material m_debugDrawMaterial;

		// HDRI equirectangular cube material
		Material m_HDRIMaterial;

		// Primary RT Texture
		Texture m_PrimaryRTTexture0;
		Texture m_PrimaryRTTexture1;
		Texture m_PrimaryRTTexture2;

		// Ping pong RT textures
		Texture m_PingPongRTTexture1;
		Texture m_PingPongRTTexture2;

		// Outline RT Texture
		Texture m_OutlineRTTexture;

		// Cubemap texture for HDRI skybox
		Texture m_HDRICubemap;

		// Irradiance cubemap for HDRI skybox
		Texture m_HDRIIrradianceMap;

		// Prefilter map for HDRI skybox
		Texture m_HDRIPrefilterMap;

		// BRDF LUT texture for HDRI
		Texture m_HDRILutMap;

		// Default texture
		Texture m_DefaultTexture;

		// Default empty cubemap texture
		Texture m_DefaultCubemapTexture;

		// Default drawing parameters.
		DrawParams m_DefaultDrawParams;

		// Skybox drawing parameters.
		DrawParams m_SkyboxDrawParams;

		// Draw parameters for fbo texture rendering
		DrawParams m_FullscreenQuadDP;

		// Buffer for global matrices
		UniformBuffer m_GlobalDataBuffer;

		// Buffer for lights.
		UniformBuffer m_GlobalLightBuffer;

		// Buffer for debugging.
		UniformBuffer m_GlobalDebugBuffer;

		// Model loader.
		ModelLoader m_ModelLoader;

		// GUI layer queue.
		LayerStack m_GUILayerStack;

		// Debug data
		RenderingDebugData m_DebugData;

		// Structure that keeps track of current buffer values
		BufferValueRecord m_BufferValueRecord;

		// ECS system for rendering camera perspective.
		LinaEngine::ECS::CameraSystem m_CameraSystem;

		// ECS Mesh Renderer system
		LinaEngine::ECS::MeshRendererSystem m_MeshRendererSystem;

		// ECS system for handling lighting
		LinaEngine::ECS::LightingSystem m_LightingSystem;

		// ECS system list for rendering operations.
		LinaEngine::ECS::ECSSystemList m_RenderingPipeline;

		// Texture resources.
		std::map<int, Texture> m_LoadedTextures;

		// Model resources
		std::map<int, Mesh> m_LoadedMeshes;

		// Material storage
		std::map<int, Material> m_LoadedMaterials;

		// Map that stores shader ID's by name
		std::map<int, Shader> m_LoadedShaders;

		// List that stores the materials to receive shadows.
		std::set<Material*> m_ShadowMappedMaterials;

	private:

		// Standart Skybox vertex array object.
		uint32 m_SkyboxVAO;

		// Quad to draw the buffers into.
		uint32 m_ScreenQuadVAO;

		// HDRI Cubemap vao
		uint32 m_HDRICubeVAO;

		// VAO for drawing lines
		uint32 m_lineVAO;

		// Light counts.
		int m_CurrentSpotLightCount;
		int m_CurrentPointLightCount;

		// Whether hdri data is captured & calculated or not.
		bool m_HDRIDataCaptured;

		// HDRI Skybox resolution
		Vector2 m_HDRIResolution = Vector2(512, 512);

		// Frame buffer texture parameters
		SamplerParameters mainRTParams;
		SamplerParameters primaryRTParams;
		SamplerParameters pingPongRTParams;

		// Post scene draw callback
		std::function<void()> m_postSceneDrawCallback;

		DISALLOW_COPY_ASSIGN_NEW(RenderEngine);
	};

}


#endif