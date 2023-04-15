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

#include "GUI/Nodes/Widgets/GUINodeTooltip.hpp"
#include "GUI/Utility/GUIUtility.hpp"
#include "Graphics/Interfaces/ISwapchain.hpp"
#include "Graphics/Platform/LinaVGIncl.hpp"

namespace Lina::Editor
{
	GUINodeTooltip::GUINodeTooltip(Editor* editor, ISwapchain* swapchain) : GUINode(editor, swapchain, FRONT_DRAW_ORDER)
	{
	}
	void GUINodeTooltip::Draw(int threadID)
	{
		if (!m_visible)
			return;

		const float padding		  = Theme::GetProperty(ThemeProperty::GeneralItemPadding, m_swapchain->GetWindowDPIScale());
		const float textWrapWidth = 200.0f;

		LinaVG::TextOptions opts;
		opts.font	   = Theme::GetFont(FontType::AltEditor, m_swapchain->GetWindowDPIScale());
		//opts.wrapWidth = textWrapWidth;

		const Vector2 textSize = FL2(LinaVG::CalculateTextSize(m_title.c_str(), opts));
		m_rect.size			   = textSize + Vector2(padding * 2, padding * 2);

		GUIUtility::DrawPopupBackground(threadID, m_rect, 1.0f * m_swapchain->GetWindowDPIScale(), m_drawOrder);

		const Vector2 textStart = m_rect.pos + Vector2(padding, padding + textSize.y);
		LinaVG::DrawTextNormal(threadID, m_title.c_str(), LV2(textStart), opts, 0.0f, m_drawOrder);
	}
} // namespace Lina::Editor
