/*
Author: Inan Evin
www.inanevin.com
https://github.com/inanevin/LinaEngine

Copyright 2020~ Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: ScenePanel
Timestamp: 6/5/2020 6:51:29 PM

*/
#pragma once

#ifndef ScenePanel_HPP
#define ScenePanel_HPP

#include "Panels/EditorPanel.hpp"
#include "Utility/EditorUtility.hpp"
#include "Utility/Math/Color.hpp"
#include "ECS/Components/TransformComponent.hpp"

namespace LinaEngine
{
	namespace Graphics
	{
		class RenderEngine;
	}
}

namespace LinaEditor
{

	class GUIlayer;

	class ScenePanel : public EditorPanel
	{
	
		
	public:

		enum DrawMode
		{
			FinalImage,
			ShadowMap
		};
		
		ScenePanel(LinaEngine::Vector2 position, LinaEngine::Vector2 size, GUILayer& guiLayer) :EditorPanel(position, size, guiLayer) {};
		virtual ~ScenePanel() {};

		virtual void Draw(float frameTime) override;
		virtual void Setup() override;

		// Set selected entity's transform component
		FORCEINLINE void SetSelectedTransform(LinaEngine::ECS::TransformComponent* tr) { m_SelectedTransform = tr; }

		// Handle hardware input
		void ProcessInput();

		// Handles gizmos
		void DrawGizmos();

		// Sets draw mode
		FORCEINLINE void SetDrawMode(DrawMode mode) { m_drawMode = mode; }

	private:

		LinaEngine::ECS::TransformComponent* m_SelectedTransform = nullptr;
		DrawMode m_drawMode = DrawMode::FinalImage;
	    LinaEngine::Graphics::RenderEngine* m_RenderEngine;
	};
}

#endif
