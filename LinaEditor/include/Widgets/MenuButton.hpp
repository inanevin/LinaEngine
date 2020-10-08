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
	class MenuButton;

	class MenuButtonItem
	{
	private:

		friend class MenuButton;

		MenuButtonItem(const char* title, std::function<void()>& onClick, size_t childrenSize = 0, MenuButtonItem** children = nullptr);
		~MenuButtonItem();

		// Draw this menu button item.
		void Draw();

	private:

		const char* m_title = nullptr;
		MenuButtonItem** m_children = nullptr;
		std::function<void()> m_onClick;
		size_t m_childrenSize = 0;
	};

	class MenuButton
	{
		
	public:
		
		MenuButton(const char* title, size_t childrenSize, MenuButtonItem** children, const LinaEngine::Color& bgColor = LinaEngine::Color(0,0,0,0), const LinaEngine::Color& activeColor = LinaEngine::Color(0,0,0,0));
		~MenuButton();

		// Draw this item.
		void Draw();
	
	private:
	
		const char* m_title;
		const char* m_popupID;
		MenuButtonItem** m_children;
		size_t m_childrenSize;
		LinaEngine::Color m_bgColor;
		LinaEngine::Color m_activeColor;
	};
}

#endif
