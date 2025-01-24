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

#include "Core/GUI/Widgets/Primitives/Circle.hpp"
#include "Common/Math/Math.hpp"
#include "Common/Platform/LinaVGIncl.hpp"

namespace Lina
{
	void Circle::Draw()
	{
		const Vector2 sz  = GetEndFromMargins() - GetStartFromMargins();
		const float	  rad = m_props.useXForRadius ? (sz.x * 0.5f - m_props.thickness * 2) : (Math::Min(sz.x, sz.y) * 0.5f - m_props.thickness * 2);

		LinaVG::StyleOptions style;
		style.aaEnabled			 = m_props.useAA;
		style.thickness			 = m_props.thickness;
		style.color				 = m_props.colorBackground.AsLVG();
		style.isFilled			 = m_props.isFilled;
		style.color.gradientType = LinaVG::GradientType::Horizontal;

		const Vector2 pos = m_props.useXForRadius ? Vector2(m_rect.GetCenter().x, m_rect.GetCenter().y + sz.y * 0.25f) : m_rect.GetCenter();
		// bg
		m_lvg->DrawCircle(pos.AsLVG(), rad, style, 36, 0.0f, m_props.startAngle, m_props.endAngle, m_drawOrder);

		// fg
		if (Math::Equals(m_props.foregroundFill, 0.0f, 0.01f))
			return;
		style.color = m_props.colorForeground.AsLVG();
		m_lvg->DrawCircle(pos.AsLVG(), rad, style, 36, 0.0f, m_props.startAngle, m_props.startAngle + (m_props.endAngle - m_props.startAngle) * m_props.foregroundFill, m_drawOrder);
	}
} // namespace Lina
