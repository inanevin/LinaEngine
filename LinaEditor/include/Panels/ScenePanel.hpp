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
Class: ScenePanel

Displays the final image of the scene generated via the engine. Is used as a main viewport for editing levels.

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
		LinaEngine::Graphics::RenderEngine* m_renderEngine = nullptr;
	};
}

#endif
