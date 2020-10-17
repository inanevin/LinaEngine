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
Class: ECSPanel

Draws all the alive entities in the current loaded level.

Timestamp: 5/23/2020 4:15:06 PM
*/
#pragma once

#ifndef ECSPanel_HPP
#define ECSPanel_HPP

#include "Panels/EditorPanel.hpp"
#include "ECS/ECS.hpp"

namespace LinaEditor
{
	class ScenePanel;
	class PropertiesPanel;
	class GUILayer;

	class ECSPanel : public EditorPanel
	{
		
	public:
		
		ECSPanel(LinaEngine::Vector2 position, LinaEngine::Vector2 size,GUILayer& guiLayer) : EditorPanel(position,size, guiLayer) {};
		virtual ~ECSPanel() {};
	
		virtual void Setup() override;
		virtual void Draw(float frameTime) override;

		void Refresh();

	private:

		ScenePanel* m_scenePanel = nullptr;
		PropertiesPanel* m_propertiesPanel = nullptr;
		LinaEngine::ECS::ECSRegistry* m_ecs = nullptr;
		LinaEngine::ECS::ECSEntity m_selectedEntity;
		std::vector<LinaEngine::ECS::ECSEntity> m_entityList;
	};
}

#endif
