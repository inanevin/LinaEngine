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

#include "EventSystem/Events.hpp"
#include "Panels/EditorPanel.hpp"
#include "ECS/ECS.hpp"

namespace Lina
{
	namespace World
	{
		class Level;
	}
}

namespace Lina::Editor
{
	class ScenePanel;
	class PropertiesPanel;

	class ECSPanel : public EditorPanel
	{
		
	public:
		
		ECSPanel() {};
		virtual ~ECSPanel() {};

		virtual void Initialize() override;
		virtual void Draw() override;
		void Refresh();
		void DrawEntityNode(int id,  Lina::ECS::Entity entity);
		void OnLevelInstall(Event::ELevelInstalled event);

	private:

		Lina::ECS::Entity m_selectedEntity;
	};
}

#endif
