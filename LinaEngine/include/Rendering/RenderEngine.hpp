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


#include "Rendering/Sampler.hpp"
#include "Rendering/ArrayBitmap.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/Shader.hpp"
#include "Rendering/RenderTarget.hpp"
#include "Rendering/GameRenderContext.hpp"
#include "ECS/EntityComponentSystem.hpp"
#include "ECS/Systems/CameraSystem.hpp"
#include "ECS/Systems/RenderableMeshSystem.hpp"
#include "Utility/Math/Color.hpp"
#include "PackageManager/PAMInputEngine.hpp"

using namespace LinaEngine::ECS;
using namespace LinaEngine;

namespace LinaEngine::Graphics
{

	class RenderEngine
	{
	public:

		RenderEngine();

		~RenderEngine();

		// Get a void* reference to the native window. e.g GLFWwindow
		FORCEINLINE void* GetNativeWindow()
		{ 
			return m_RenderDevice->GetNativeWindow();
		}

		// Create a render context.
		FORCEINLINE bool CreateContextWindow(InputEngine<PAMInputEngine>& inputEngineIn)
		{
			return m_RenderDevice->CreateContextWindow(inputEngineIn);
		};

		// Set the target of the callbacks coming from the main window context.
		FORCEINLINE void SetMainWindowEventCallback(const std::function<void(Event&)>& callback)
		{
			m_RenderDevice->SetMainWindowEventCallback(callback);
		}

		// Initialize the render renderEngine.
		void Initialize(EntityComponentSystem* ecsIn);

		// Called each frame.
		void Tick(float delta);

		// Called when the main window is resized.
		void OnWindowResized(float width, float height);

	private:

		// Device for rendering operations.
		std::unique_ptr<PAMRenderDevice> m_RenderDevice;

		// ECS reference.
		EntityComponentSystem* m_ECS;

		// Default texture sampler
		Sampler m_DefaultSampler;

		// Default texture data.
		ArrayBitmap m_DefaultTextureBitmap;

		// Default diffuse texture
		Texture m_DefaultDiffuseTexture;

		// Default shader
		Shader m_DefaultShader;

		// Default render target
		RenderTarget m_DefaultRenderTarget;

		// Default drawing parameters.
		DrawParams m_DefaultDrawParams;

		// Default camera perspective
		Matrix m_DefaultPerspective;

		// Default Game Render Context
		GameRenderContext m_DefaultRenderContext;

		// ECS system for rendering camera perspective.
		CameraSystem m_CameraSystem;

		// ECS system for drawing meshes.
		RenderableMeshSystem m_RenderableMeshSystem;

		// ECS system list for rendering operations.
		ECSSystemList m_RenderingPipeline;

		// Default clear color.
		LinaEngine::Color m_DefaultClearColor;

		// Default field of view
		float m_DefaultFOV = 70.0f;

		// Default Z near
		float m_DefaultZNear = 0.1f;

		// Default Z far
		float m_DefaultZFar = 1000.0f;
	};

}


#endif