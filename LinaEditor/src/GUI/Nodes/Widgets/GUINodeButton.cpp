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
#include "Platform/LinaVGIncl.hpp"
#include "GUI/Utility/GUIUtility.hpp"
#include "Core/Theme.hpp"
#include "Math/Math.hpp"
#include "LinaGX/Core/InputMappings.hpp"

namespace Lina::Editor
{
	void GUINodeButton::Draw(int threadID)
	{
		if (!GetIsVisible())
			return;

		Vector2 textSize = GetStoreSize("TextSize"_hs, m_title, m_isIcon ? FontType::EditorIcons : FontType::DefaultEditor, m_textScale);

		// Adjust rect
		{
			const float padding = Theme::GetProperty(ThemeProperty::MenuButtonPadding, m_window->GetDPIScale());

			SetWidgetHeight(ThemeProperty::WidgetHeightTall);
			if (!m_widthOverridenOutside)
				m_rect.size.x = textSize.x + padding * 2;
		}

		// Draw background rect
		{
			LinaVG::StyleOptions opts;
			opts.rounding = m_rounding;
			opts.color	  = m_isPressed ? m_pressedColor.AsLVG4() : (m_isHovered ? m_hoveredColor.AsLVG4() : m_defaultColor.AsLVG4());

			if (m_isHovered && m_enableHoverOutline)
			{
				opts.outlineOptions.thickness	  = 1.0f * m_window->GetDPIScale();
				opts.outlineOptions.drawDirection = LinaVG::OutlineDrawDirection::Inwards;
				opts.outlineOptions.color		  = m_outlineColor.AsLVG4();
			}

			LinaVG::DrawRect(threadID, m_rect.pos.AsLVG2(), (m_rect.pos + m_rect.size).AsLVG2(), opts, 0.0f, m_drawOrder);
		}

		// Draw actual text
		{
			if (m_isIcon)
			{
				const Vector2 textPos = Vector2(m_rect.pos.x + m_rect.size.x * 0.5f, m_rect.pos.y + m_rect.size.y * 0.5f);
				GUIUtility::DrawIcon(threadID, m_window->GetDPIScale(), m_title.c_str(), textPos, m_textScale, m_textColor, m_drawOrder);
			}
			else
			{
				const Vector2		textPos = Vector2(m_rect.pos.x + m_rect.size.x * 0.5f - textSize.x * 0.5f, m_rect.pos.y + m_rect.size.y * 0.5f + textSize.y * 0.5f);
				LinaVG::TextOptions textOpts;
				textOpts.color	   = m_textColor.AsLVG4();
				textOpts.font	   = Theme::GetFont(FontType::DefaultEditor, m_window->GetDPIScale());
				textOpts.textScale = m_textScale;
				LinaVG::DrawTextNormal(threadID, m_title.c_str(), textPos.AsLVG2(), textOpts, 0.0f, m_drawOrder + 1);
			}
		}
	}

	void GUINodeButton::OnPressEnd(uint32 button)
	{
		if (button != LINAGX_MOUSE_0)
			return;

		if (m_onClicked)
			m_onClicked(this);
	}
} // namespace Lina::Editor
