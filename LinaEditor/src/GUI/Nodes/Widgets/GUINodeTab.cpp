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

#include "GUI/Nodes/Widgets/GUINodeTab.hpp"
#include "GUI/Utility/GUIUtility.hpp"
#include "Graphics/Interfaces/ISwapchain.hpp"
#include "Graphics/Platform/LinaVGIncl.hpp"
#include "Core/Editor.hpp"
#include "GUI/Nodes/Widgets/GUINodeButton.hpp"
#include "GUI/Nodes/Widgets/GUINodeTabArea.hpp"
#include "Math/Math.hpp"
#include "Core/SystemInfo.hpp"
#include "Input/Core/InputMappings.hpp"
#include "Graphics/Interfaces/IWindow.hpp"
#include "Core/SystemInfo.hpp"

namespace Lina::Editor
{
#define OFFSET_FROM_END	  0.9f
#define CLOSEBUT_SPEED	  25.0f
#define ANIM_ALPHA_SPEED  15.0f
#define ANIM_TEXT_SCALE	  2.0f
#define TARGET_TEXT_SCALE 1.1f
	GUINodeTab::GUINodeTab(GUIDrawerBase* drawer, GUINodeTabArea* area, int drawOrder) : m_parentArea(area), GUINode(drawer, drawOrder)
	{
	}

	void GUINodeTab::Draw(int threadID)
	{
		if (!GetIsVisible())
			return;

		const float padding	  = Theme::GetProperty(ThemeProperty::GeneralItemPadding, m_window->GetDPIScale());
		const int	drawOrder = m_isFocused ? m_drawOrder + 3 : m_drawOrder;
		const float delta	  = SystemInfo::GetDeltaTimeF();

		// Animations alpha
		{
			m_indicatorAnimationAlpha = Math::Lerp(m_indicatorAnimationAlpha, m_isFocused ? 1.0f : 0.0f, ANIM_ALPHA_SPEED * delta);

			if (m_flashAnimState == 1)
			{
				if (m_textScale < TARGET_TEXT_SCALE)
					m_textScale += ANIM_TEXT_SCALE * delta;
				else
					m_flashAnimState = 2;
			}
			else if (m_flashAnimState == 2)
			{
				if (m_textScale > 1.0f)
					m_textScale -= ANIM_TEXT_SCALE * delta;
				else
				{
					m_flashAnimState = 0;
					m_textScale		 = 1.0f;
				}
			}
		}

		// Background convex shape
		{
			Vector<LinaVG::Vec2> points;
			points.push_back(LV2(m_rect.pos));
			points.push_back(LV2(Vector2(m_rect.pos.x + m_rect.size.x * OFFSET_FROM_END, m_rect.pos.y)));
			points.push_back(LV2((m_rect.pos + m_rect.size)));
			points.push_back(LV2(Vector2(m_rect.pos.x, m_rect.pos.y + m_rect.size.y)));

			LinaVG::StyleOptions opts;
			opts.color		   = LV4(Theme::TC_Dark3);
			opts.color.start.w = opts.color.end.w = m_isFocused ? 1.0f : 0.3f;
			opts.aaEnabled						  = true;
			LinaVG::DrawConvex(threadID, points.data(), static_cast<int>(points.size()), opts, 0.0f, drawOrder);

			// Draw side and bottom line if focused.
			{
				if (!Math::Equals(m_indicatorAnimationAlpha, 0.0f, 0.05f))
				{
					LinaVG::StyleOptions lineStyle;
					lineStyle.color.start = LV4(Theme::TC_CyanAccent);
					lineStyle.color.end	  = LV4(Theme::TC_PurpleAccent);
					lineStyle.thickness	  = 2.0f * m_window->GetDPIScale();

					Vector2 lineBegin = Vector2(m_rect.pos.x + padding * 0.5f, m_rect.pos.y + padding * 0.25f);
					Vector2 lineEnd	  = Vector2(m_rect.pos.x + padding * 0.5f, m_rect.pos.y + +m_rect.size.y - padding * 0.25f);

					const Vector2 usedLineBegin = Math::Lerp(lineEnd, lineBegin, m_indicatorAnimationAlpha);

					LinaVG::DrawLine(threadID, LV2(usedLineBegin), LV2(lineEnd), lineStyle, LinaVG::LineCapDirection::None, 0.0f, drawOrder);

					lineBegin		= Vector2(m_rect.pos.x, m_rect.pos.y + m_rect.size.y - 1.0f);
					lineEnd			= Vector2(m_rect.pos.x + m_rect.size.x, m_rect.pos.y + m_rect.size.y - 1.0f);
					lineStyle.color = LV4(Theme::TC_Dark2);
					// LinaVG::DrawLine(threadID, LV2(lineBegin), LV2(lineEnd), lineStyle, LinaVG::LineCapDirection::None, 0.0f, drawOrder);
				}
			}
		}

		// Text
		{
			LinaVG::TextOptions textOpts;
			textOpts.font		   = Theme::GetFont(FontType::DefaultEditor, m_window->GetDPIScale());
			textOpts.color.start.w = textOpts.color.end.w = m_isFocused ? 1.0f : 0.3f;
			textOpts.textScale							  = m_textScale;

			const Vector2 textSize = FL2(LinaVG::CalculateTextSize(m_title.c_str(), textOpts));
			const Vector2 textPos  = Vector2(m_rect.pos.x + padding, m_rect.pos.y + m_rect.size.y * 0.5f + textSize.y * 0.5f);
			LinaVG::DrawTextNormal(threadID, m_title.c_str(), LV2(textPos), textOpts, 0.0f, drawOrder, true);
		}

		// Close button
		{
			if (m_closeButtonEnabled)
			{
				if (m_isHovered)
				{
					const Rect closeRect		= Rect(Vector2(m_rect.pos.x + m_rect.size.x * OFFSET_FROM_END - padding * 1.0f, m_rect.pos.y), Vector2(m_rect.size.x * (1.0f - OFFSET_FROM_END) + padding * 1.5f, m_rect.size.y));
					m_isInCloseRect				= GUIUtility::IsInRect(m_window->GetMousePosition(), closeRect);
					m_closeButtonAnimationAlpha = Math::Lerp(m_closeButtonAnimationAlpha, m_isInCloseRect ? 1.0f : 0.0f, SystemInfo::GetDeltaTime() * CLOSEBUT_SPEED);
				}
				else
					m_isInCloseRect = false;

				m_closeButtonAnimationAlpha = Math::Lerp(m_closeButtonAnimationAlpha, m_isInCloseRect ? 1.0f : 0.0f, SystemInfo::GetDeltaTime() * CLOSEBUT_SPEED);
				DrawCloseButton(threadID, m_closeButtonAnimationAlpha, drawOrder);
			}
		}

		if (m_isPressed)
		{
			const Vector2 mousePosNow = m_window->GetMousePosition();
			const Vector2 deltaPress  = mousePosNow - Vector2(m_pressStartMousePos);

			if (m_isReorderEnabled)
			{
				m_rect.pos.x += m_window->GetMouseDelta().x;
				m_rect.pos.x = Math::Clamp(m_rect.pos.x, m_minRect.pos.x, m_maxRect.pos.x);
			}

			if (m_canDetach)
			{
				if (Math::Abs(deltaPress.y) > m_rect.size.y)
				{
					m_isPressed = false;
					m_parentArea->OnTabDetached(this, Vector2(m_pressStartMousePos) - m_rect.pos + Vector2(deltaPress.x, 0.0f));
				}
			}
		}
	}

	void GUINodeTab::OnPressEnd(uint32 button)
	{
		if (button == LINA_MOUSE_0)
		{
			if (m_isInCloseRect)
				m_parentArea->OnTabDismissed(this);
			else
				m_parentArea->OnTabClicked(this);
		}
	}

	void GUINodeTab::OnPressBegin(uint32 button)
	{
		if (button != LINA_MOUSE_0)
			return;

		m_parentArea->OnTabClicked(this);
	}

	void GUINodeTab::AnimateFlash()
	{
		m_textScale		 = 1.0f;
		m_flashAnimState = 1;
	}

	void GUINodeTab::DrawCloseButton(int threadID, float t, int baseDrawOrder)
	{
		// Uuf, setup
		const float	  padding	 = Theme::GetProperty(ThemeProperty::GeneralItemPadding, m_window->GetDPIScale());
		const float	  offset	 = padding * 1.5f;
		const float	  yOffset	 = 0.0f;
		const Vector2 originalTL = Vector2(m_rect.pos.x + m_rect.size.x * OFFSET_FROM_END, m_rect.pos.y + yOffset);
		const Vector2 originalBL = Vector2(m_rect.pos.x + m_rect.size.x, m_rect.pos.y + m_rect.size.y);
		const Vector2 targetTL	 = Vector2(originalTL.x - offset, m_rect.pos.y + yOffset);
		const Vector2 targetBL	 = Vector2(originalBL.x - offset, m_rect.pos.y + m_rect.size.y - yOffset);
		const Vector2 p0		 = Math::Lerp(originalTL, targetTL, t);
		const Vector2 p3		 = Math::Lerp(originalBL, targetBL, t);
		const Vector2 p1		 = Vector2(m_rect.pos.x + m_rect.size.x * OFFSET_FROM_END, m_rect.pos.y + yOffset);
		const Vector2 p2		 = m_rect.pos + m_rect.size - Vector2(0, yOffset);

		Vector<LinaVG::Vec2> points;
		points.push_back(LV2(p0));
		points.push_back(LV2(p1));
		points.push_back(LV2(p2));
		points.push_back(LV2(p3));

		// Don't bother drawing if too small.
		if (t > 0.05f)
		{
			LinaVG::StyleOptions opts;
			Color				 bg = Theme::TC_RedAccent;
			bg.w					= Math::Lerp(0.0f, m_isPressed ? 0.5f : 1.0f, t);
			opts.color				= LV4(bg);
			opts.aaEnabled			= true;
			opts.aaMultiplier		= 1.0f;
			LinaVG::DrawConvex(threadID, points.data(), static_cast<int>(points.size()), opts, 0.0f, baseDrawOrder + 1);
		}

		// Close icon
		{
			Vector2 iconCenter = (p2 + targetTL) * 0.5f;
			iconCenter.x -= padding * 0.1f;
			iconCenter.y = (m_rect.pos.y + m_rect.pos.y + m_rect.size.y) * 0.5f;

			Color iconColor = t < 0.7f ? Color(1.0f, 1.0f, 1.0f, 0.2f) : Math::Lerp(Color(0, 0, 0, 0), Color::White, t * 10.0f);
			iconColor.w		= Math::Clamp(iconColor.w, 0.0f, 1.0f);

			GUIUtility::DrawIcon(threadID, m_window->GetDPIScale(), TI_CROSS, iconCenter, 0.75f, iconColor, baseDrawOrder + 2, 90.0f * t, true);
		}
	}

} // namespace Lina::Editor
