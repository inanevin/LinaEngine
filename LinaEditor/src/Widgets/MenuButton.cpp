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
Timestamp: 10/8/2020 9:02:44 PM

*/


#include "imgui.h"
#include "Widgets/MenuButton.hpp"

namespace LinaEditor
{
	MenuButtonItem::MenuButtonItem(const char* title, std::function<void()>& onClick, size_t childrenSize, MenuButtonItem** children)
	{
		m_title = title;
		m_onClick = onClick;
		m_childrenSize = childrenSize;
		m_children = children;
	}

	MenuButtonItem::~MenuButtonItem()
	{
		if (m_childrenSize != 0)
		{
			for (int i = 0; i < m_childrenSize; i++)
				delete m_children[i];
		}
	}

	void MenuButtonItem::Draw()
	{
		if (ImGui::MenuItem(m_title))
		{
			if (m_onClick)
				m_onClick();
		}
	}
}