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
#include "Mesh.hpp"
#include "UniformBuffer.hpp"
#include "Window.hpp"
#include "RenderContext.hpp"
#include "Utility/Math/Color.hpp"
#include <functional>
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

	class RenderEngine
	{
	public:

		RenderEngine();

		~RenderEngine();


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
		FORCEINLINE Vector2F GetWindowSize()
		{
			return Vector2F(m_MainWindow.GetWidth(), m_MainWindow.GetHeight());
		};

		// Returns the window center coordinates.
		FORCEINLINE Vector2F GetWindowCenter()
		{
			return Vector2F(m_MainWindow.GetWidth() / 2.0f, m_MainWindow.GetHeight() / 2.0f);
		}

		// Initialize the render renderEngine.
		void Initialize(LinaEngine::ECS::ECSRegistry& ecsIn);

		// Called each frame.
		void Tick(float delta);

		// Called when the main window is resized.
		void OnWindowResized(float width, float height);

		/// <summary> Creates a material resource with a specific shader. </summary>
		/// <param name= "refPointer"> Send a pointer as a reference and it will point to the newly created/existing material. </param>
		void CreateMaterial(const std::string & materialName, const std::string & shaderName, Material** refPointer = nullptr);

		/// <summary> Creates a texture resource. </summary>
		/// <param name= "refPointer"> Send a pointer as a reference and it will point to the newly created/existing texture. </param>
		void CreateTexture(const std::string & textureName, const std::string & filePath, PixelFormat pixelFormat = PixelFormat::FORMAT_RGB, bool generateMipmaps = true, bool compress = false,SamplerData samplerData = SamplerData(), Texture * *refPointer = nullptr);

		/// <summary> Creates a cubemap texture resource. </summary>
		/// <param name= "refPointer"> Send a pointer as a reference and it will point to the newly created/existing cube map texture. </param>
		void CreateTexture(const std::string & textureName, const std::string filePath[6], PixelFormat pixelFormat = PixelFormat::FORMAT_RGB, bool generateMipmaps = true, bool compress = false, SamplerData samplerData = SamplerData(), Texture** refPointer = nullptr);

		/// <summary> Creates a mesh resource. </summary>
		/// <param name= "refPointer"> Send a pointer as a reference and it will point to the newly created/existing mesh. </param>
		void CreateMesh(const std::string & meshName, const std::string & filePath, Mesh** refPointer = nullptr);

		/// <summary> Creates a shader resource. </summary>
		/// <param name= "refPointer"> Send a pointer as a reference and it will point to the newly created/existing shader. </param>
		void CreateShader(const std::string & shaderName, const std::string & shaderText, Shader** refPointer = nullptr);

		// Returns a material resource.
		Material & GetMaterial(const std::string & materialName);

		// Returns a texture resource
		Texture & GetTexture(const std::string & textureName);

		// Returns a mesh resource.
		Mesh & GetMesh(const std::string & meshName);

		// Returns a shader resource.
		Shader & GetShader(const std::string & shaderName);

		// Removes the targeted resource from resource map.
		void UnloadTextureResource(const std::string & textureName);

		// Removes the targeted resource from resource map.
		void UnloadMeshResource(const std::string & meshName);

		// Removes the targeted resource from resource map.
		void UnloadMaterialResource(const std::string & materialName);

		// Returns whether a material/texture/mesh/shader exists or not.
		bool MaterialExists(const std::string & materialName);
		bool TextureExists(const std::string & textureName);
		bool MeshExists(const std::string & meshName);
		bool ShaderExists(const std::string & shaderName);

		// Sets the shader of a material to the shader specified by name. Also resets material properties based on the shader, caution!
		void SetMaterialShader(Material & material, const std::string & shaderName);

		// Sets the skybox material.
		FORCEINLINE void SetSkyboxMaterial(Material & skyboxMaterial) { m_SkyboxMaterial = &skyboxMaterial; }
		void SetSkyboxMaterial(const std::string & materialName);

		// Pushes a new layer into the gui stack.
		void PushLayer(Layer* layer);

		// Pushes a new overlay layer into the gui stack.
		void PushOverlay(Layer* layer);


	private:

		// Constructs commonly used shaders within Lina Engine.
		void ConstructEngineShaders();

		// clears resource memory.
		void DumpMemory();

		// Renders skybox
		void DrawSkybox();

		// Updates related uniform buffers on GPU
		void UpdateUniformBuffers();

	private:

		// Device for rendering operations.
		RenderDevice m_RenderDevice;

		// Context window
		Window m_MainWindow;

		// Default render target
		RenderTarget m_RenderTarget;

		// Default drawing parameters.
		DrawParams m_DefaultDrawParams;

		// Skybox drawing parameters.
		DrawParams m_SkyboxDrawParams;

		// Sprite drawing parameters.
		DrawParams m_SpriteDrawParams;

		// Default Game Render Context
		RenderContext m_DefaultRenderContext;

		// ECS system for rendering camera perspective.
		LinaEngine::ECS::CameraSystem m_CameraSystem;

		// ECS Mesh Renderer system
		LinaEngine::ECS::MeshRendererSystem m_MeshRendererSystem;

		// ECS system for handling lighting
		LinaEngine::ECS::LightingSystem m_LightingSystem;

		// ECS system list for rendering operations.
		LinaEngine::ECS::ECSSystemList m_RenderingPipeline;

		// Texture resources.
		std::map<std::string, Texture> m_LoadedTextures;

		// Model resources
		std::map<std::string, Mesh> m_LoadedMeshes;

		// Material storage
		std::map<std::string, Material> m_LoadedMaterials;

		// Map that stores shader ID's by name
		std::map<std::string, Shader> m_LoadedShaders;

		// Buffer for global matrices
		UniformBuffer m_GlobalDataBuffer;

		// Buffer for lights.
		UniformBuffer m_GlobalLightBuffer;

		// Material used to draw skybox.
		Material* m_SkyboxMaterial = nullptr;

		// Dummy material, mesh and texture to return if none is found while trying to get one of them.
		Material m_DummyMaterial;
		Mesh m_DummyMesh;
		Texture m_DummyTexture;

	private:

		// Standart Skybox vertex array object.
		uint32 m_SkyboxVAO;

		// Standart sprite vertex array object.
		uint32 m_SpriteVAO;

		// Model loader.
		ModelLoader m_ModelLoader;

		// GUI layer queue.
		LayerStack m_GUILayerStack;

		DISALLOW_COPY_ASSIGN_NEW(RenderEngine);
	};

}


#endif