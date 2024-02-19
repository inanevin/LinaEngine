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

#include "Core/GUI/Widgets/Primitives/Button.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/WidgetUtility.hpp"

namespace Lina
{
	void Button::Construct()
	{
		m_text = Allocate<Text>();
		AddChild(m_text);
	}

	void Button::Tick(float delta)
	{
		if (m_props.widthFit == Fit::FromChildren)
			m_rect.size.x = m_text->GetSize().x + m_props.margins.left + m_props.margins.right;

		if (m_props.heightFit == Fit::FromChildren)
			m_rect.size.y = m_text->GetSize().y + m_props.margins.top + m_props.margins.bottom;

		const Vector2& textSize = m_text->GetSize();
		m_text->SetPos(Vector2(m_rect.pos.x + m_rect.size.x * 0.5f - textSize.x * 0.5f, m_rect.pos.y + m_rect.size.y * 0.5f - textSize.y * 0.5f));
	}

	void Button::Draw(int32 threadIndex)
	{
		if (m_isPressed)
			WidgetUtility::DrawRectBackground(threadIndex, m_props.backgroundPressed, m_rect, m_drawOrder);
		else if (m_isHovered)
			WidgetUtility::DrawRectBackground(threadIndex, m_props.backgroundHovered, m_rect, m_drawOrder);
		else
			WidgetUtility::DrawRectBackground(threadIndex, m_props.background, m_rect, m_drawOrder);

		m_text->Draw(threadIndex);
	}

	void Button::SetProps(const Properties& props)
	{
		m_text->SetProps({.text = props.text, .font = props.font});
		m_props = props;
	}
} // namespace Lina
