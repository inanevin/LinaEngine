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

#include "Common/GUI/Widgets/Text/Text.hpp"

namespace Lina
{
	void Text::SizePass()
	{
		CalcTextOptions();

		if (contents.font->m_isSDF)
			base.size = LinaVG::CalculateTextSize(m_threadIndex, contents.text.c_str(), m_sdfOptions);
		else
			base.size = LinaVG::CalculateTextSize(m_threadIndex, contents.text.c_str(), m_textOptions);
	}

	void Text::Draw()
	{
		if (contents.font->m_isSDF)
			LinaVG::DrawTextSDF(m_threadIndex, contents.text.c_str(), (base.pos + Vector2(0, base.size.y)).AsLVG(), m_sdfOptions);
		else
			LinaVG::DrawTextNormal(m_threadIndex, contents.text.c_str(), (base.pos + Vector2(0, base.size.y)).AsLVG(), m_textOptions);
	}

	void Text::CalcTextOptions()
	{
		m_textOptions.font		  = contents.font;
		m_sdfOptions.font		  = contents.font;
		m_sdfOptions.sdfThickness = 0.5f;
		m_sdfOptions.sdfSoftness  = 0.25f / static_cast<float>(contents.font->m_size);
	}
} // namespace Lina
