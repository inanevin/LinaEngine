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

#include "GUI/Nodes/Widgets/GUINodeButton.hpp"
#include "Data/CommonData.hpp"
#include "Graphics/Platform/LinaVGIncl.hpp"
#include "GUI/Utility/GUIUtility.hpp"
#include "Core/Theme.hpp"
#include "Graphics/Interfaces/ISwapchain.hpp"
#include "Math/Math.hpp"
#include "Input/Core/InputMappings.hpp"

namespace Lina::Editor
{
	void GUINodeButton::Draw(int threadID)
	{
		if (!m_visible)
			return;

		Vector2 textSize = Vector2::Zero;

		// Determine text size
		{
			if (m_isIcon)
			{
				LinaVG::SDFTextOptions textOpts;
				textOpts.font	   = Theme::GetFont(FontType::EditorIcons, m_swapchain->GetWindowDPIScale());
				textOpts.textScale = m_textScale;
				textSize		   = FL2(LinaVG::CalculateTextSize(threadID, m_title.c_str(), textOpts));
			}
			else
			{
				LinaVG::TextOptions textOpts;
				textOpts.font	   = Theme::GetFont(m_fontType, m_swapchain->GetWindowDPIScale());
				textOpts.textScale = m_textScale;
				textSize		   = FL2(LinaVG::CalculateTextSize(threadID, m_title.c_str(), textOpts));
			}
		}

		// Adjust rect w.r.t fit type
		{
			if (m_fitType == ButtonFitType::AutoFitFromTextAndPadding)
			{
				const float padding = Theme::GetProperty(ThemeProperty::MenuButtonPadding, m_swapchain->GetWindowDPIScale());
				m_rect.size			= Vector2(textSize.x + padding * 2, textSize.y + padding * 0.75f);
			}
		}

		// Draw background rect
		{
			LinaVG::StyleOptions opts;
			opts.rounding = m_rounding;
			opts.color	  = m_isPressed ? LV4(m_pressedColor) : (m_isHovered ? LV4(m_hoveredColor) : LV4(m_defaultColor));

			if (m_isHovered && m_enableHoverOutline)
			{
				opts.outlineOptions.thickness	  = 1.0f * m_swapchain->GetWindowDPIScale();
				opts.outlineOptions.drawDirection = LinaVG::OutlineDrawDirection::Inwards;
				opts.outlineOptions.color		  = LV4(m_outlineColor);
			}

			LinaVG::DrawRect(threadID, LV2(m_rect.pos), LV2((m_rect.pos + m_rect.size)), opts, 0.0f, m_drawOrder);
		}

		// Draw actual text
		{
			if (m_isIcon)
			{
				const Vector2 textPos = Vector2(m_rect.pos.x + m_rect.size.x * 0.5f, m_rect.pos.y + m_rect.size.y * 0.5f);
				GUIUtility::DrawIcon(threadID, m_swapchain->GetWindowDPIScale(), m_title.c_str(), textPos, m_textScale, m_textColor, m_drawOrder);
			}
			else
			{
				const Vector2		textPos = Vector2(m_rect.pos.x + m_rect.size.x * 0.5f - textSize.x * 0.5f, m_rect.pos.y + m_rect.size.y * 0.5f + textSize.y * 0.5f);
				LinaVG::TextOptions textOpts;
				textOpts.color	   = LV4(m_textColor);
				textOpts.font	   = Theme::GetFont(m_fontType, m_swapchain->GetWindowDPIScale());
				textOpts.textScale = m_textScale;
				LinaVG::DrawTextNormal(threadID, m_title.c_str(), LV2(textPos), textOpts, 0.0f, m_drawOrder + 1);
			}
		}
	}

	void GUINodeButton::OnClicked(uint32 button)
	{
		if (button != LINA_MOUSE_0)
			return;

		if (m_onClicked)
			m_onClicked(this);
	}

	Vector2 GUINodeButton::CalculateSize()
	{
		if (Math::Equals(m_lastDpi, m_swapchain->GetWindowDPIScale(), 0.0001f))
			return m_lastCalculatedSize;

		m_lastDpi = m_swapchain->GetWindowDPIScale();
		LinaVG::TextOptions textOpts;
		textOpts.font		 = Theme::GetFont(m_fontType, m_lastDpi);
		m_lastCalculatedSize = FL2(LinaVG::CalculateTextSize(m_title.c_str(), textOpts));
		return m_lastCalculatedSize;
	}

} // namespace Lina::Editor
