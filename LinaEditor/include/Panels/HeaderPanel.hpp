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
Class: HeaderPanel

Draws the top header and its components like title, lina icon, window buttons, menubar etc.

Timestamp: 10/8/2020 1:39:03 PM
*/

#pragma once

#ifndef HeaderPanel_HPP
#define HeaderPanel_HPP

#include "Widgets/MenuButton.hpp"
#include "Panels/EditorPanel.hpp"

namespace Lina
{
	namespace Graphics
	{
		class RenderEngine;
		class Window;
	}
}

namespace Lina::Editor
{
	class MenuButton;
	enum class MenuBarItems;

	class HeaderPanel : public EditorPanel
	{

	public:

		HeaderPanel() {};
		virtual ~HeaderPanel();

		virtual void Setup() override;
		virtual void Draw() override;

	private:

		void DispatchMenuBarClickedAction(const MenuBarItems& item);

	private:

		std::string m_title = "";
		std::vector<MenuButton*> m_menuBarButtons;
		std::vector<MenuButton*> m_toolButtons;

	};
}

#endif
