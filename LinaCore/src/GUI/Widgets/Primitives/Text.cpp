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
#include "Core/Resources/ResourceManager.hpp"
#include "Common/Math/Math.hpp"

namespace Lina
{
	void Text::Initialize()
	{
		CalculateTextSize();
	}

	void Text::Draw(int32 threadIndex)
	{
		const float dpiScale = m_lgxWindow->GetDPIScale();

		if (!Math::Equals(dpiScale, m_calculatedDPIScale, 0.01f))
			CalculateTextSize();

		if (m_isSDF)
		{
			LinaVG::SDFTextOptions opts;
			opts.font		 = m_lvgFont;
			opts.textScale	 = m_props.textScale;
			opts.alignment	 = m_props.alignment;
			opts.color.start = opts.color.end = GetIsDisabled() ? m_props.colorDisabled.AsLVG4() : m_props.color.AsLVG4();
			LinaVG::DrawTextSDF(threadIndex, m_props.text.c_str(), (m_rect.pos + Vector2(0.0f, m_rect.size.y)).AsLVG(), opts, 0.0f, m_drawOrder, m_props.isDynamic);
		}
		else
		{
			LinaVG::TextOptions opts;
			opts.font		 = m_lvgFont;
			opts.textScale	 = m_props.textScale;
			opts.alignment	 = m_props.alignment;
			opts.color.start = opts.color.end = GetIsDisabled() ? m_props.colorDisabled.AsLVG4() : m_props.color.AsLVG4();
			LinaVG::DrawTextNormal(threadIndex, m_props.text.c_str(), (m_rect.pos + Vector2(0.0f, m_rect.size.y)).AsLVG(), opts, 0.0f, m_drawOrder, m_props.isDynamic);
		}
	}

	void Text::CalculateTextSize()
	{
		auto*		font	 = m_resourceManager->GetResource<Font>(m_props.font);
		const float dpiScale = m_lgxWindow->GetDPIScale();
		m_lvgFont			 = font->GetLinaVGFont(dpiScale);
		m_calculatedDPIScale = dpiScale;

		m_isSDF = m_lvgFont->m_isSDF;

		if (m_isSDF)
		{
			LinaVG::SDFTextOptions opts;
			opts.font	   = m_lvgFont;
			opts.textScale = m_props.textScale;
			m_rect.size	   = LinaVG::CalculateTextSize(m_props.text.c_str(), opts);
		}
		else
		{
			LinaVG::TextOptions opts;
			opts.font	   = m_lvgFont;
			opts.textScale = m_props.textScale;
			m_rect.size	   = LinaVG::CalculateTextSize(m_props.text.c_str(), opts);
		}
	}
} // namespace Lina
