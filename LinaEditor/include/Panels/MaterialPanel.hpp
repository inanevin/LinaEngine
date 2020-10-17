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
Class: MaterialPanel

Responsible for editing materials.

Timestamp: 6/4/2020 8:31:22 PM
*/
#pragma once

#ifndef MaterialPanel_HPP
#define MaterialPanel_HPP

#include "Panels/EditorPanel.hpp"
#include "Utility/EditorUtility.hpp"
#include "Rendering/Material.hpp"

namespace LinaEngine
{
	namespace Graphics
	{
		class RenderEngine;
		class Material;
	}
}

namespace LinaEditor
{
	class GUIlayer;

	class MaterialPanel : public EditorPanel
	{
		
	public:
		
		MaterialPanel(LinaEngine::Vector2 position, LinaEngine::Vector2 size, GUILayer& guiLayer) : EditorPanel(position, size, guiLayer) {};
		virtual ~MaterialPanel() {};
	
		virtual void Draw(float frameTime) override;
		virtual void Setup() override;
		FORCEINLINE void SetCurrentMaterial(LinaEngine::Graphics::Material* mat) { m_CurrentSelectedMaterial = mat; }

	private:
	
		LinaEngine::Graphics::Material* m_CurrentSelectedMaterial = nullptr;
		LinaEngine::Graphics::RenderEngine* m_RenderEngine;
	};
}

#endif
