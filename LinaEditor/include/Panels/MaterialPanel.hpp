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

Class: MaterialPanel
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
	
		virtual void Draw() override;
		virtual void Setup() override;
		FORCEINLINE void SetCurrentMaterial(LinaEngine::Graphics::Material* mat) { m_CurrentSelectedMaterial = mat; }

	private:
	
		LinaEngine::Graphics::Material* m_CurrentSelectedMaterial = nullptr;
		LinaEngine::Graphics::RenderEngine* m_RenderEngine;
	};
}

#endif
