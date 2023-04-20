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

namespace Lina::Editor
{
#define OFFSET_FROM_END 0.9f
#define CLOSEBUT_SPEED	25.0f

	GUINodeTab::GUINodeTab(GUIDrawerBase* drawer, GUINodeTabArea* area, int drawOrder) : m_parentArea(area), GUINode(drawer, drawOrder)
	{
	}

	void GUINodeTab::Draw(int threadID)
	{
		if (!m_visible)
			return;

		const float padding = Theme::GetProperty(ThemeProperty::GeneralItemPadding, m_window->GetDPIScale());

		// Background convex shape
		{
			Vector<LinaVG::Vec2> points;
			points.push_back(LV2(m_rect.pos));
			points.push_back(LV2(Vector2(m_rect.pos.x + m_rect.size.x * OFFSET_FROM_END, m_rect.pos.y)));
			points.push_back(LV2((m_rect.pos + m_rect.size)));
			points.push_back(LV2(Vector2(m_rect.pos.x, m_rect.pos.y + m_rect.size.y)));

			LinaVG::StyleOptions opts;
			opts.color	   = LV4((m_isFocused ? Theme::TC_Dark3 : Theme::TC_Light1));
			opts.aaEnabled = true;
			LinaVG::DrawConvex(threadID, points.data(), static_cast<int>(points.size()), opts, 0.0f, m_drawOrder);
		}

		// Text
		{
			LinaVG::TextOptions textOpts;
			textOpts.font		   = Theme::GetFont(FontType::DefaultEditor, m_window->GetDPIScale());
			const Vector2 textSize = FL2(LinaVG::CalculateTextSize(m_title.c_str(), textOpts));
			const Vector2 textPos  = Vector2(m_rect.pos.x + padding, m_rect.pos.y + m_rect.size.y * 0.5f + textSize.y * 0.5f);
			LinaVG::DrawTextNormal(threadID, m_title.c_str(), LV2(textPos), textOpts, 0.0f, m_drawOrder);
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
				DrawCloseButton(threadID, m_closeButtonAnimationAlpha);
			}
		}

		if (m_isDragging)
		{
			const Vector2 mousePosNow = m_window->GetMousePosition();
			const Vector2 deltaPress  = mousePosNow - Vector2(m_dragStartMousePos);

			if (m_isReorderEnabled)
				m_rect.pos.x += m_window->GetMouseDelta().x;

			if (m_isPanelTabs)
			{
				if (Math::Abs(deltaPress.y) > m_rect.size.y)
				{
					m_isDragging = false;
					m_parentArea->OnTabDetached(this, Vector2(m_dragStartMousePos) - m_rect.pos + Vector2(deltaPress.x, 0.0f));
				}
			}
		}
	}

	Vector2 GUINodeTab::CalculateSize()
	{
		const float windowDPI = m_window->GetDPIScale();
		if (Math::Equals(m_lastDpi, windowDPI, 0.0001f))
			return m_lastCalculatedSize;
		m_lastDpi = windowDPI;

		const float padding = Theme::GetProperty(ThemeProperty::GeneralItemPadding, m_lastDpi);
		float		totalX	= 0.0f;
		totalX += padding;

		LinaVG::TextOptions opts;
		opts.font			   = Theme::GetFont(FontType::DefaultEditor, m_lastDpi);
		const Vector2 textSize = FL2(LinaVG::CalculateTextSize(m_title.c_str(), opts));
		totalX += textSize.x + padding;

		const float closeButtonSize = textSize.y;
		totalX += closeButtonSize + padding;

		m_lastCalculatedSize = Vector2(totalX, textSize.y + padding);
		return m_lastCalculatedSize;
	}

	void GUINodeTab::OnClicked(uint32 button)
	{
		if (button == LINA_MOUSE_0)
		{
			if (m_isInCloseRect)
				m_parentArea->OnTabDismissed(this);
			else
				m_parentArea->OnTabClicked(this);
		}
	}

	void GUINodeTab::DrawCloseButton(int threadID, float t)
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
			LinaVG::DrawConvex(threadID, points.data(), static_cast<int>(points.size()), opts, 0.0f, m_drawOrder + 1);
		}

		// Close icon
		{
			Vector2 iconCenter = (p2 + targetTL) * 0.5f;
			iconCenter.x -= padding * 0.1f;
			iconCenter.y = (m_rect.pos.y + m_rect.pos.y + m_rect.size.y) * 0.5f;

			Color iconColor = t < 0.7f ? Color(1.0f, 1.0f, 1.0f, 0.2f) : Math::Lerp(Color(0, 0, 0, 0), Color::White, t * 10.0f);
			iconColor.w		= Math::Clamp(iconColor.w, 0.0f, 1.0f);

			GUIUtility::DrawIcon(threadID, m_window->GetDPIScale(), TI_CROSS, iconCenter, 0.75f, iconColor, m_drawOrder + 2, 90.0f * t, true);
		}
	}

} // namespace Lina::Editor
