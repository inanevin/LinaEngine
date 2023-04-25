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

#include "GUI/Nodes/Widgets/GUINodeText.hpp"
#include "GUI/Utility/GUIUtility.hpp"
#include "Graphics/Interfaces/ISwapchain.hpp"
#include "Graphics/Platform/LinaVGIncl.hpp"
#include "Graphics/Interfaces/IWindow.hpp"
#include "Math/Math.hpp"

namespace Lina::Editor
{
	GUINodeText::GUINodeText(GUIDrawerBase* drawer, int drawOrder) : GUINode(drawer, drawOrder)
	{
	}

	void GUINodeText::Draw(int threadID)
	{
		if (!m_visible)
			return;

		m_rect.size = CalculateSize();

		LinaVG::TextOptions opts;
		opts.font		 = Theme::GetFont(m_fontType, m_window->GetDPIScale());
		opts.color.start = LV4(m_startColor);
		opts.color.end	 = LV4(m_endColor);

		if (m_alignment == TextAlignment::Right)
			opts.alignment = LinaVG::TextAlignment::Right;
		else if (m_alignment == TextAlignment::Center)
			opts.alignment = LinaVG::TextAlignment::Center;

		opts.textScale = m_scale;
		LinaVG::DrawTextNormal(threadID, m_text.c_str(), LV2(m_rect.pos), opts, 0.0f, m_drawOrder);
	}

	Vector2 GUINodeText::CalculateSize()
	{
		const float windowDpi = m_window->GetDPIScale();
		if (Math::Equals(m_lastDpi, windowDpi, 0.001f))
			return m_lastCalculatedSize;

		LinaVG::TextOptions opts;
		opts.font			 = Theme::GetFont(m_fontType, windowDpi);
		opts.textScale		 = m_scale;
		m_lastCalculatedSize = FL2(LinaVG::CalculateTextSize(m_text.c_str(), opts));
		return m_lastCalculatedSize;
	}

	void GUINodeTextRichColors::Draw(int threadID)
	{
		if (!m_visible)
			return;

		m_rect.size		= CalculateSize();
		Vector2 textPos = m_rect.pos;

		if (m_alignment == TextAlignment::Center)
		{
			textPos.x -= m_lastCalculatedSize.x * 0.5f;
			textPos.y += m_lastCalculatedSize.y * 0.5f;
		}

		for (auto& d : m_textData)
		{
			LinaVG::TextOptions opts;
			opts.font	   = Theme::GetFont(m_fontType, m_window->GetDPIScale());
			opts.textScale = m_scale;
			opts.color	   = LV4(d.color);
			LinaVG::DrawTextNormal(threadID, d.text.c_str(), LV2(textPos), opts, 0.0f, m_drawOrder);
			textPos.x += d.calculatedSize.x;
		}
	}

	Vector2 GUINodeTextRichColors::CalculateSize()
	{
		const float windowDpi = m_window->GetDPIScale();
		if (Math::Equals(m_lastDpi, windowDpi, 0.001f))
			return m_lastCalculatedSize;

		m_lastCalculatedSize = Vector2::Zero;

		for (auto& d : m_textData)
		{
			LinaVG::TextOptions opts;
			opts.font	   = Theme::GetFont(m_fontType, windowDpi);
			opts.textScale = m_scale;

			d.calculatedSize = FL2(LinaVG::CalculateTextSize(d.text.c_str(), opts));
			m_lastCalculatedSize += d.calculatedSize;
		}

		return m_lastCalculatedSize;
	}

	void GUINodeTextRichColors::AddText(const String& text, const Color& col)
	{
		m_textData.push_back({text, col});
		m_lastDpi = 0.0f;
	}
} // namespace Lina::Editor