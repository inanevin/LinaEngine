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
	void FileMenu::Construct()
	{
		GetProps().direction = DirectionOrientation::Horizontal;
		GetFlags().Set(WF_SIZE_X_TOTAL_CHILDREN);
		DirectionalLayout::Construct();
	}

	void FileMenu::Initialize()
	{
		for (const auto& str : m_fileMenuProps.buttons)
		{
			Button* btn = Allocate<Button>("FileMenuButton");
			btn->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y | WF_SIZE_X_MAX_CHILDREN);
			btn->SetAlignedPosY(0.0f);
			btn->SetAlignedSizeY(1.0f);
			btn->GetChildMargins() = {.left = Theme::GetDef().baseIndent, .right = Theme::GetDef().baseIndent};

			btn->GetProps().outlineThickness  = 0.0f;
			btn->GetProps().rounding		  = 0.0f;
			btn->GetProps().colorDefaultStart = Color(0.0f, 0.0f, 0.0f, 0.0f);
			btn->GetProps().colorDefaultEnd	  = Color(0.0f, 0.0f, 0.0f, 0.0f);
			btn->GetProps().colorPressed	  = Theme::GetDef().background2;
			btn->GetProps().colorHovered	  = Theme::GetDef().background3;

			const StringID sid = TO_SID(str);

			btn->GetProps().onClicked = [sid, this, btn]() {
				Popup* popup = Allocate<Popup>();
				popup->SetPosX(btn->GetPosX());
				popup->SetPosY(btn->GetRect().GetEnd().y);
				m_manager->AddToForeground(popup);
				if (m_listener)
					m_listener->OnPopupCreated(popup, sid);
			};

			btn->GetText()->GetProps().text = str;
			AddChild(btn);
		}

		DirectionalLayout::Initialize();
	}

} // namespace Lina
