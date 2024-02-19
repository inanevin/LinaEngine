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

#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Common/Math/Math.hpp"

namespace Lina
{
	void Text::Draw(int32 threadIndex)
	{
		const float dpiScale = m_window->GetDPIScale();
		if (!Math::Equals(dpiScale, m_calculatedDPIScale, 0.01f))
			GenerateTextOptions();

		if (m_lvgFont->m_isSDF)
			LinaVG::DrawTextSDF(threadIndex, m_props.text.c_str(), (m_rect.pos + Vector2(0, m_rect.size.y)).AsLVG(), m_sdfOptions, 0.0f, m_drawOrder);
		else
			LinaVG::DrawTextNormal(threadIndex, m_props.text.c_str(), (m_rect.pos + Vector2(0, m_rect.size.y)).AsLVG(), m_textOptions, 0.0f, m_drawOrder);
	}

	void Text::GenerateTextOptions()
	{
		const float dpiScale = m_window->GetDPIScale();
		m_lvgFont			 = m_props.font->GetLinaVGFont(dpiScale);
		m_calculatedDPIScale = dpiScale;

		m_textOptions.font		  = m_lvgFont;
		m_textOptions.color		  = m_props.color.AsLVG4();
		m_sdfOptions.font		  = m_lvgFont;
		m_sdfOptions.color		  = m_props.color.AsLVG4();
		m_sdfOptions.sdfThickness = 0.5f;
		m_sdfOptions.sdfSoftness  = 0.25f / static_cast<float>(m_lvgFont->m_size);

		if (m_lvgFont->m_isSDF)
			m_rect.size = LinaVG::CalculateTextSize(m_props.text.c_str(), m_sdfOptions);
		else
			m_rect.size = LinaVG::CalculateTextSize(m_props.text.c_str(), m_textOptions);
	}
} // namespace Lina
