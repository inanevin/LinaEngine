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

#include "Editor/Widgets/Testbed.hpp"
#include "Editor/CommonEditor.hpp"
#include "Common/System/System.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/Primitives/Button.hpp"
#include "Core/GUI/Widgets/Primitives/Checkbox.hpp"
#include "Core/GUI/Theme.hpp"
#include "Core/GUI/Widgets/WidgetUtility.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/Resource/Font.hpp"

namespace Lina::Editor
{

	void Testbed::Construct()
	{
		auto* resMan = m_system->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);

		// // Icon
		// {
		// 	Text* text = Allocate<Text>();
		// 	text->SetProps({
		// 		.text  = ICON_LINA_LOGO,
		// 		.font  = ICON_FONT_SID,
		// 	});
		// 	text->SetPos(Vector2(10, 10));
		// 	AddChild(text);
		// }
		//
		// // Testbed Text
		// {
		// 	Text* text = Allocate<Text>();
		// 	text->SetProps({
		// 		.text  = "Testbed",
		// 	});
		// 	text->SetPos(Vector2(30, 10));
		// 	AddChild(text);
		// }
		//
		// // Button
		// {
		//
		// 	Button* button = Allocate<Button>();
		// 	button->SetProps({
		// 		.widthFit		   = Fit::Fixed,
		// 		.heightFit		   = Fit::Fixed,
		// 		.margins		   = {},
		// 		.text			   = "Button",
		// 	});
		// 	button->SetSize(Vector2(100, 30));
		// 	button->SetPos(Vector2(10, 30));
		// 	AddChild(button);
		// }

		// Checkbox
		{
			Checkbox* check = Allocate<Checkbox>();
			check->SetProps({
				.margins   = TBLR::Eq(Theme::GetDef().baseIndent),
				.font	   = ICON_FONT_SID,
				.checkIcon = ICON_CHECK,
			});
			check->SetPos(Vector2(10, 70));
			AddChild(check);
		}
	}

	void Testbed::Tick(float delta)
	{
		m_rect = m_parent->GetRect();
		Widget::Tick(delta);
	}

	void Testbed::Draw(int32 threadIndex)
	{
		RectBackground bgSettings = {
			.enabled	= true,
			.startColor = Theme::GetDef().background1,
			.endColor	= Theme::GetDef().background1,
		};

		WidgetUtility::DrawRectBackground(threadIndex, bgSettings, m_rect, m_drawOrder);
		Widget::Draw(threadIndex);
	}

} // namespace Lina::Editor
