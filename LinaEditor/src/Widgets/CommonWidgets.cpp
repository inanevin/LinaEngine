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

#include "Editor/Widgets/CommonWidgets.hpp"
#include "Core/GUI/Widgets/Primitives/Icon.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Editor/Widgets/Compound/WindowButtons.hpp"
#include "Editor/CommonEditor.hpp"
#include "Common/Platform/LinaVGIncl.hpp"

namespace Lina::Editor
{
	DirectionalLayout* CommonWidgets::BuildWindowBar(const String& title, bool hasWindowButtons, bool hasIcon, Widget* allocator)
	{
		DirectionalLayout* layout = allocator->Allocate<DirectionalLayout>();
		layout->SetChildPadding(Theme::GetDef().baseIndent);

		if (hasIcon)
		{
			Icon* icon			  = allocator->Allocate<Icon>();
			icon->GetProps().icon = ICON_LINA_LOGO;
			icon->GetFlags().Set(WF_POS_ALIGN_Y);
			icon->SetAlignedPosY(0.5f);
			icon->SetPosAlignmentSourceY(PosAlignmentSource::Center);
			layout->AddChild(icon);
		}

		Text* text			  = allocator->Allocate<Text>();
		text->GetProps().text = title;
		text->GetFlags().Set(WF_POS_ALIGN_Y);
		text->SetAlignedPosY(0.5f);
		text->SetPosAlignmentSourceY(PosAlignmentSource::Center);
		layout->AddChild(text);

		if (hasWindowButtons)
		{
			WindowButtons* wb = allocator->Allocate<WindowButtons>();
			wb->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y | WF_USE_FIXED_SIZE_X);
			wb->SetAlignedPosY(0.0f);
			wb->SetAlignedSizeY(1.0f);
			wb->SetPosAlignmentSourceX(PosAlignmentSource::End);
			wb->SetFixedSizeX(Theme::GetDef().baseItemHeight * 6.0f);
			layout->AddChild(wb);
		}

		return layout;
	}

	void CommonWidgets::DrawGradLine(int32 threadIndex, const Vector2& start, const Vector2& end, int32 drawOrder, const Color& baseColor)
	{
		Color weak = baseColor, strong = baseColor;
		strong.w = 1.0f;
		weak.w	 = 0.0f;

		LinaVG::StyleOptions style;
		style.color.start = strong.AsLVG4();
		style.color.end	  = weak.AsLVG4();
		LinaVG::DrawRect(threadIndex, start.AsLVG(), end.AsLVG(), style, 0.0f, drawOrder);
	}

	void CommonWidgets::DrawGradLineCentral(int32 threadIndex, const Vector2& start, const Vector2& end, int32 drawOrder, const Color& baseColor)
	{
		Color weak = baseColor, strong = baseColor;
		strong.w = 1.0f;
		weak.w	 = 0.0f;

		LinaVG::StyleOptions style;
		style.color.start = weak.AsLVG4();
		style.color.end	  = strong.AsLVG4();

		LinaVG::StyleOptions style2;
		style2.color.start = strong.AsLVG4();
		style2.color.end   = weak.AsLVG4();

		LinaVG::DrawRect(threadIndex, start.AsLVG(), Vector2((end.x + start.x) * 0.5f, end.y).AsLVG(), style, 0.0f, drawOrder);
		LinaVG::DrawRect(threadIndex, Vector2((end.x + start.x) * 0.5f, start.y).AsLVG(), Vector2(end.x, end.y).AsLVG(), style2, 0.0f, drawOrder);
	}

} // namespace Lina::Editor
