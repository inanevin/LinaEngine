/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

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

#include "Core/GUI/Widgets/Compound/FileMenu.hpp"
#include "Core/GUI/Widgets/Compound/Popup.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"

namespace Lina
{
	void FileMenu::Initialize()
	{
		for (const auto& str : m_props.buttons)
		{
			Button*		   btn = Allocate<Button>("FileMenuButton");
			const StringID sid = TO_SID(str);

			btn->GetProps()			  = m_props.buttonProps;
			btn->GetProps().onClicked = [sid, this, btn]() {
				Popup* popup = Allocate<Popup>();
				popup->SetPosX(btn->GetPosX());
				popup->SetPosY(btn->GetRect().GetEnd().y);
				m_manager->AddToForeground(popup);
				if (m_listener)
					m_listener->OnPopupCreated(popup, sid);
			};

			btn->GetText()->GetProps().text = str;
			btn->Initialize();
			AddChild(btn);
			m_buttons.push_back(btn);
		}
	}

	void FileMenu::Tick(float delta)
	{
		Widget::SetIsHovered();

		const float padding = Theme::GetDef().baseIndentInner;

		float x = m_rect.pos.x;
		for (Button* btn : m_buttons)
		{
			btn->SetSizeX(padding * 2 + btn->GetText()->GetSizeX());
			btn->SetSizeY(m_rect.size.y);
			btn->SetPosX(x);
			btn->SetPosY(m_rect.pos.y);
			btn->Tick(delta);
			x += btn->GetSizeX();
		}
	}

	void FileMenu::Draw(int32 threadIndex)
	{
		Widget::Draw(threadIndex);
	}
} // namespace Lina
