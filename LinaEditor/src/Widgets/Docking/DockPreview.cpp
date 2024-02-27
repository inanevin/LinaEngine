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

#include "Editor/Widgets/Docking/DockPreview.hpp"
#include "Editor/CommonEditor.hpp"
#include "Common/Tween/TweenManager.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/Math/Math.hpp"
#include "Core/GUI/Widgets/Primitives/Icon.hpp"

namespace Lina::Editor
{

	namespace
	{
		void SetGradientFromDirection(LinaVG::StyleOptions& outOptions, const Color& start, const Color& end, const Vector2 direction)
		{

			if (direction.x < 0.0f)
			{
				outOptions.color.start		  = start.AsLVG4();
				outOptions.color.end		  = end.AsLVG4();
				outOptions.color.gradientType = LinaVG::GradientType::Horizontal;
			}
			else if (direction.x > 0.0f)
			{
				outOptions.color.start		  = end.AsLVG4();
				outOptions.color.end		  = start.AsLVG4();
				outOptions.color.gradientType = LinaVG::GradientType::Horizontal;
			}
			else if (direction.y > 0.0f)
			{
				outOptions.color.start		  = end.AsLVG4();
				outOptions.color.end		  = start.AsLVG4();
				outOptions.color.gradientType = LinaVG::GradientType::Vertical;
			}
			else if (direction.y < 0.0f)
			{
				outOptions.color.start		  = start.AsLVG4();
				outOptions.color.end		  = end.AsLVG4();
				outOptions.color.gradientType = LinaVG::GradientType::Vertical;
			}
			else
			{
				outOptions.color.start		  = start.AsLVG4();
				outOptions.color.end		  = end.AsLVG4();
				outOptions.color.gradientType = LinaVG::GradientType::Radial;
				outOptions.color.radialSize	  = 1.0f;
			}
		};
	} // namespace

	void DockPreview::Construct()
	{
		auto createIcon = [&](const String& ic) -> Icon* {
			Icon* icon					= Allocate<Icon>();
			icon->GetProps().icon		= ic;
			icon->GetProps().textScale	= 0.7f;
			icon->GetProps().offsetPerc = ICONOFFSET_ARROW_RECTS;
			AddChild(icon);
			return icon;
		};

		m_dockRects[0].icon = createIcon(ICON_RECT_FILLED);
		m_dockRects[1].icon = createIcon(ICON_ARROW_RECT_UP);
		m_dockRects[2].icon = createIcon(ICON_ARROW_RECT_RIGHT);
		m_dockRects[3].icon = createIcon(ICON_ARROW_RECT_DOWN);
		m_dockRects[4].icon = createIcon(ICON_ARROW_RECT_LEFT);

		for (int32 i = 0; i < 5; i++)
			m_dockRects[i].direction = DockDirectionToVector(static_cast<DockDirection>(i));
	}

	void DockPreview::Initialize()
	{
		Widget::Initialize();
		TweenManager::Get()->AddTween(&m_animationAlpha, 0.0f, 1.0f, ANIM_TIME, TweenType::EaseOut);
	}

	void DockPreview::Tick(float delta)
	{
		m_smallRectSize	  = SMALL_RECT_SZ * m_lgxWindow->GetDPIScale();
		const Vector2& mp = m_lgxWindow->GetMousePosition();

		for (int32 i = 0; i < 5; i++)
		{
			auto& dr = m_dockRects[i];

			const Rect rt		  = Rect(dr.position - Vector2(dr.size, dr.size) * 0.5f, Vector2(dr.size, dr.size));
			const bool wasHovered = dr.isHovered;
			dr.isHovered		  = rt.IsPointInside(mp);

			if (dr.isHovered)
			{
				if (!wasHovered)
					dr.expand = 0.0f;

				dr.expand	= Math::Lerp(dr.expand, 1.0f, delta * 10.0f);
				dr.extraPos = dr.direction * dr.size * 0.12f * dr.expand;
			}
			else
				dr.extraPos = Vector2::Zero;

			Vector2 targetPos = m_rect.GetCenter() + dr.direction * m_smallRectSize * 1.25f;

			if (!m_props.isCentral)
				targetPos = m_rect.GetCenter() + (GetHalfSize() - Vector2(m_smallRectSize * 0.75f, m_smallRectSize * 0.75f)) * dr.direction;

			dr.position = Math::Lerp(m_rect.GetCenter(), targetPos, m_animationAlpha);
			dr.size		= Math::Lerp(0.0f, m_smallRectSize + (dr.isHovered ? dr.expand * dr.size * 0.15f : 0.0f), m_animationAlpha);
			dr.icon->SetPos(m_dockRects[i].position + dr.extraPos - m_dockRects[i].icon->GetHalfSize());
			dr.icon->SetDrawOrder(FOREGROUND_DRAW_ORDER + 1);
		}
	}

	void DockPreview::Draw(int32 threadIndex)
	{
		const int32 start = m_props.isCentral ? 0 : 1;
		for (int32 i = start; i < 5; i++)
			DrawDockRect(threadIndex, m_dockRects[i]);
	}

	void DockPreview::DrawDockRect(int32 threadIndex, const DockRect& dr)
	{
		const float	  sz	= dr.size * 0.5f;
		const Vector2 start = (dr.position + dr.extraPos) - Vector2(sz, sz);
		const Vector2 end	= (dr.position + dr.extraPos) + Vector2(sz, sz);

		LinaVG::StyleOptions opts;
		opts.rounding = Theme::GetDef().baseRounding;
		opts.color	  = dr.isHovered ? Theme::GetDef().accentPrimary2.AsLVG4() : Theme::GetDef().accentPrimary0.AsLVG4();
		// opts.color.start = Theme::GetDef().accentPrimary0.AsLVG4();
		// opts.color.end = Theme::GetDef().accentPrimary2.AsLVG4();
		//  if(dr.isHovered)
		//      opts.color = Theme::GetDef().accentPrimary2.AsLVG4();
		opts.color.start.w = opts.color.end.w = 0.25f;

		LinaVG::DrawRect(threadIndex, start.AsLVG(), end.AsLVG(), opts, 0.0f, FOREGROUND_DRAW_ORDER);
		dr.icon->Draw(threadIndex);

		if (dr.isHovered)
		{
			const float previewRectSize = (m_rect.size * dr.direction * DOCK_DEFAULT_PERCENTAGE).Magnitude();
			Vector2		halfSize		= Vector2::Zero;
			halfSize.x					= m_rect.size.x * (1.0f - Math::Abs(dr.direction.x)) + (previewRectSize * Math::Abs(dr.direction.x));
			halfSize.y					= m_rect.size.y * (1.0f - Math::Abs(dr.direction.y)) + (previewRectSize * Math::Abs(dr.direction.y));
			halfSize *= 0.5f;

			const Vector2 center = m_rect.GetCenter() + (dr.direction * GetHalfSize()) + (dr.direction * -1.0f * halfSize);

			LinaVG::StyleOptions previewRect;
			Color				 start = Theme::GetDef().accentPrimary0;
			Color				 end   = Theme::GetDef().accentPrimary0;
			end.w					   = 0.0f;
			SetGradientFromDirection(previewRect, start, end, dr.direction);

			LinaVG::DrawRect(threadIndex, (center - halfSize).AsLVG(), (center + halfSize).AsLVG(), previewRect, 0.0f, FOREGROUND_DRAW_ORDER);
		}
	}

	void DockPreview::GetHoveredDirection(DockDirection& outDirection, bool& outIsHovered)
	{
		int32 i = 0;
		for (const auto& dr : m_dockRects)
		{
			if (dr.isHovered)
			{
				outDirection = static_cast<DockDirection>(i);
				outIsHovered = true;
				return;
			}
			i++;
		}
		outIsHovered = false;
	}
} // namespace Lina::Editor
