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

Class: HeaderPanel
Timestamp: 10/8/2020 1:39:03 PM

*/
#pragma once

#ifndef HeaderPanel_HPP
#define HeaderPanel_HPP

// Headers here.
#include "Widgets/MenuButton.hpp"
#include "Panels/EditorPanel.hpp"

namespace LinaEngine
{
	namespace Graphics
	{
		class RenderEngine;
		class Window;
	}
}

namespace LinaEditor
{
	class GUILayer;
	class MenuButton;

	class HeaderPanel : public EditorPanel
	{

	public:

		HeaderPanel(LinaEngine::Vector2 position, LinaEngine::Vector2 size, GUILayer& guiLayer, const std::string& title) : EditorPanel(position, size, guiLayer) { m_title = title; };
		virtual ~HeaderPanel();

		virtual void Setup() override;
		virtual void Draw() override;


	private:

		std::string m_title;
		std::vector<MenuButton*> m_menuBarButtons;
		LinaEngine::Graphics::RenderEngine* m_renderEngine = nullptr;
		LinaEngine::Graphics::Window* m_appWindow = nullptr;

	};
}

#endif
