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
	class ScenePanel : public EditorPanel
	{
		
	public:
		
		ScenePanel(Vector2 position, Vector2 size, class GUILayer& guiLayer) :EditorPanel(position, size, guiLayer) {};
		virtual ~ScenePanel() {};

		virtual void Draw() override;
		virtual void Setup() override;

		// Set selected entity's transform component
		FORCEINLINE void SetSelectedTransform(LinaEngine::ECS::TransformComponent* tr) { m_SelectedTransform = tr; }

		// Draws a line in scene view.
		void DrawLine(Vector3 p1, Vector3 p2, Color col, float width = 1.0f);

		// Handle hardware input
		void ProcessInput();

		// Handles gizmos
		void DrawGizmos();

	private:

		LinaEngine::ECS::TransformComponent* m_SelectedTransform = nullptr;

		class LinaEngine::Graphics::RenderEngine* m_RenderEngine;
	};
}

#endif
