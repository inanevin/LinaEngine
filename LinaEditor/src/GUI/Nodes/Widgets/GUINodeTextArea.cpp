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

#include "GUI/Nodes/Widgets/GUINodeTextArea.hpp"
#include "GUI/Utility/GUIUtility.hpp"
#include "Graphics/Interfaces/ISwapchain.hpp"
#include "Graphics/Platform/LinaVGIncl.hpp"
#include "Graphics/Interfaces/IWindow.hpp"
#include "GUI/Utility/GUIUtility.hpp"
#include "Core/Theme.hpp"
#include "Math/Math.hpp"

namespace Lina::Editor
{
	void GUINodeTextArea::Draw(int threadID)
	{
		if (!GetIsVisible())
			return;

		const float	  padding	   = Theme::GetProperty(ThemeProperty::GeneralItemPadding, m_window->GetDPIScale());
		const float	  widgetHeight = Theme::GetProperty(ThemeProperty::WidgetHeightTall, m_window->GetDPIScale());
		const Vector2 lastTextSize = Vector2::Zero;

		m_rect.size.y = widgetHeight;

		GUINode::ConsumeAvailableWidth();

		GUIUtility::DrawWidgetBackground(threadID, m_rect, m_window->GetDPIScale(), m_drawOrder + 1);

		LinaVG::TextOptions opts;
		opts.font			  = Theme::GetFont(FontType::DefaultEditor, m_window->GetDPIScale());
		const Vector2 textPos = Vector2(m_rect.pos.x + padding, m_rect.pos.y + m_rect.size.y * 0.5f + lastTextSize.y * 0.5f);
		LinaVG::DrawTextNormal(threadID, m_title.c_str(), LV2(textPos), opts, 0.0f, m_drawOrder + 1, true);
	}

} // namespace Lina::Editor
