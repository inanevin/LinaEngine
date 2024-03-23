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

#include "Core/GUI/Widgets/WidgetUtility.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/Math/Rect.hpp"
#include "Common/Math/Math.hpp"

namespace Lina
{

	void WidgetUtility::DrawRectBackground(int32 threadIndex, const RectBackground& options, const Rect& rect, int32 drawOrder, bool positionIsCenter)
	{
		if (!options.enabled)
			return;

		LinaVG::StyleOptions opts;
		opts.color.start		 = options.startColor.AsLVG4();
		opts.color.end			 = options.endColor.AsLVG4();
		opts.rounding			 = options.rounding;
		const LinaVG::Vec2 start = positionIsCenter ? (rect.pos - rect.size * 0.5f).AsLVG() : (rect.pos).AsLVG();
		const LinaVG::Vec2 end	 = positionIsCenter ? (rect.pos + rect.size * 0.5f).AsLVG() : (rect.pos + rect.size).AsLVG();
		LinaVG::DrawRect(threadIndex, start, end, opts, 0.0f, drawOrder);
	}

	void WidgetUtility::DrawAlphaLine(int32 threadIndex, const Vector2& start, const Vector2& end, int32 drawOrder, const Color& baseColor)
	{
		Color weak = baseColor, strong = baseColor;
		strong.w = 1.0f;
		weak.w	 = 0.0f;

		LinaVG::StyleOptions style;
		style.color.start = strong.AsLVG4();
		style.color.end	  = weak.AsLVG4();
		LinaVG::DrawRect(threadIndex, start.AsLVG(), end.AsLVG(), style, 0.0f, drawOrder);
	}

	void WidgetUtility::DrawAlphaLineCentral(int32 threadIndex, const Vector2& start, const Vector2& end, int32 drawOrder, const Color& baseColor)
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

	void WidgetUtility::DrawGradLineCentral(int32 threadIndex, const Vector2& start, const Vector2& end, int32 drawOrder, const Color& centerColor, const Color& edgeColor)
	{
		Color weak = edgeColor, strong = centerColor;

		LinaVG::StyleOptions style;
		style.color.start = weak.AsLVG4();
		style.color.end	  = strong.AsLVG4();

		LinaVG::StyleOptions style2;
		style2.color.start = strong.AsLVG4();
		style2.color.end   = weak.AsLVG4();

		LinaVG::DrawRect(threadIndex, start.AsLVG(), Vector2((end.x + start.x) * 0.5f, end.y).AsLVG(), style, 0.0f, drawOrder);
		LinaVG::DrawRect(threadIndex, Vector2((end.x + start.x) * 0.5f, start.y).AsLVG(), Vector2(end.x, end.y).AsLVG(), style2, 0.0f, drawOrder);
	}

	void WidgetUtility::DrawDropShadow(int32 threadIndex, const Vector2& p1, const Vector2& p2, int32 drawOrder, const Color& baseColor, int32 radius)
	{
		const Color endColor = Color(baseColor.x, baseColor.y, baseColor.z, 0.0f);

		Vector2 startPos = p1;
		Vector2 endPos	 = p2;

		const Vector2 lineDir = (endPos - startPos).Normalized().Rotate(90.0f);

		for (int32 i = 0; i < radius; i++)
		{
			const Color			 color = Math::Lerp(baseColor, endColor, static_cast<float>(i) / static_cast<float>(radius));
			LinaVG::StyleOptions style;
			style.color = color.AsLVG4();
			LinaVG::DrawLine(threadIndex, startPos.AsLVG(), endPos.AsLVG(), style, LinaVG::LineCapDirection::None, 0.0f, drawOrder);
			startPos += lineDir;
			endPos += lineDir;
		}
	}

    void WidgetUtility::DrawDropShadowRect(int32 threadIndex, const Rect &rect, int32 drawOrder, const Color &baseColor, int32 radius)
    {
        Rect usedRect = rect;
        
        const Color endColor = Color(baseColor.x, baseColor.y, baseColor.z, 0.0f);

        for(int32 i = 0; i < radius; i++)
        {
            const Color             color = Math::Lerp(baseColor, endColor, static_cast<float>(i) / static_cast<float>(radius));
            LinaVG::StyleOptions style;
            style.color = color.AsLVG4();
            LinaVG::DrawRect(threadIndex, usedRect.pos.AsLVG(), usedRect.GetEnd().AsLVG(), style, 0.0f, drawOrder);
            
            usedRect.size += Vector2::One * 2.0f;
            usedRect.pos -= Vector2::One * 1.0f;
        }
    }

    DirectionalLayout* WidgetUtility::BuildLayoutForPopups(Widget* source)
    {
        DirectionalLayout* popup = source->Allocate<DirectionalLayout>("PopupLayout");
        popup->GetFlags().Set(WF_USE_FIXED_SIZE_X | WF_SIZE_Y_TOTAL_CHILDREN);
        popup->GetChildMargins() = {.top = Theme::GetDef().baseIndentInner, .bottom = Theme::GetDef().baseIndentInner};
        popup->GetProps().backgroundStyle = DirectionalLayout::BackgroundStyle::Default;
        popup->GetProps().direction = DirectionOrientation::Vertical;
        popup->GetProps().dropShadowBackground = true;
        popup->GetProps().backgroundAnimation = true;
        popup->GetProps().clipChildren = true;
        return popup;
    }

} // namespace Lina
