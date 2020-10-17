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
Class: MenuButton

Menu button elements are used for creating a custom menu barç

Timestamp: 10/8/2020 9:02:33 PM
*/

#pragma once

#ifndef MenuButton_HPP
#define MenuButton_HPP


#include "Utility/Math/Color.hpp"
#include "Core/Common.hpp"
#include <functional>

namespace LinaEditor
{
	class MenuElement
	{
	public:

		MenuElement(const char* icon, const char* title) : m_icon(icon), m_title(title) {};
		~MenuElement() {}

		// Draw this element.
		virtual void Draw() {};

	protected:

		const char* m_icon;
		const char* m_title;

	private:

		friend class MenuButton;
		friend class MenuItem;
		MenuButton* m_parent;

	};

	class MenuItem : public MenuElement
	{
	public:

		MenuItem(const char* icon, const char* title, std::function<void()> onClick) : MenuElement(icon, title), m_onClick(onClick) {};
		~MenuItem() {};

		// Draw this menu button item.
		virtual void Draw() override;

	private:

		friend class MenuButton;

		bool m_isHovered = false;
		LinaEngine::Color m_color = LinaEngine::Color(0, 0, 0, 0);
		std::function<void()> m_onClick;
	};

	class MenuButton : public MenuElement
	{
		
	public:
		
		MenuButton(const char* title, const char* popupID, std::vector<MenuElement*>& children, const LinaEngine::Color& bgColor = LinaEngine::Color(0,0,0,0), bool useSameLine = false);
		~MenuButton();

		// Draw this item.
		virtual void Draw() override;
	
		// Closes children popup
		void ClosePopup();

		// Set on click explicitly.
		FORCEINLINE void SetOnClick(std::function<void()> onClick) { m_onClick = onClick; }

		static bool s_anyButtonFocused;

	private:
	
		const char* m_popupID;
		bool m_popupOpen = false;
		bool m_useSameLine = false;
		std::function<void()> m_onClick;
		std::vector<MenuElement*> m_children;
		LinaEngine::Color m_bgColor;
	};
}

#endif
