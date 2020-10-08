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

Class: MenuButton
Timestamp: 10/8/2020 9:02:33 PM

*/
#pragma once

#ifndef MenuButton_HPP
#define MenuButton_HPP

// Headers here.
#include "Utility/Math/Color.hpp"
#include <functional>

namespace LinaEditor
{
	class MenuElement
	{
	public:

		MenuElement(const char* title) : m_title(title) {};
		~MenuElement() {}

		// Draw this element.
		virtual void Draw() {};

	protected:

		const char* m_title;

	};

	class MenuItem : public MenuElement
	{
	public:

		MenuItem(const char* title, std::function<void()> onClick) : MenuElement(title), m_onClick(onClick) {};
		~MenuItem() {};

		// Draw this menu button item.
		virtual void Draw() override;

	private:

		std::function<void()> m_onClick;
	};

	class MenuButton : public MenuElement
	{
		
	public:
		
		MenuButton(const char* title, const char* popupID, std::vector<MenuElement*>& children, const LinaEngine::Color& bgColor = LinaEngine::Color(0,0,0,0), bool useSameLine = false);
		~MenuButton();

		// Draw this item.
		virtual void Draw() override;
	
	private:
	
		const char* m_popupID;
		bool m_popupOpen = false;
		bool m_useSameLine = false;
		std::vector<MenuElement*> m_children;
		LinaEngine::Color m_bgColor;
	};
}

#endif
