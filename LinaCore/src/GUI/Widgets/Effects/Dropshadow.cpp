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

#include "Core/GUI/Widgets/Effects/Dropshadow.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/Math/Math.hpp"

namespace Lina
{
	void Dropshadow::Draw()
	{
		const float			 startAlpha = m_props.color.w;
		LinaVG::StyleOptions opts;
		opts.thickness = m_props.thickness;
		opts.isFilled  = true;
		opts.rounding  = m_props.rounding;
		opts.color	   = m_props.color.AsLVG4();
		Vector2 min	   = m_rect.pos;
		Vector2 max	   = m_rect.GetEnd();

		for (int corner : m_props.onlyRoundCorners)
			opts.onlyRoundTheseCorners.push_back(corner);

		if (m_props.direction == Direction::Center)
		{
			for (int32 i = 0; i < m_props.steps; i++)
			{
				m_lvg->DrawRect(min.AsLVG(), max.AsLVG(), opts, 0.0f, m_drawOrder);
				min -= Vector2(m_props.thickness);
				max += Vector2(m_props.thickness);
				opts.color.start.w = opts.color.end.w = Math::Lerp(startAlpha, 0.0f, static_cast<float>(i) / static_cast<float>(m_props.steps));
			}
		}
		else
		{
			const Vector2 dir = DirectionToVector(m_props.direction);

			for (int32 i = 0; i < m_props.steps; i++)
			{
				m_lvg->DrawRect(min.AsLVG(), max.AsLVG(), opts, 0.0f, m_drawOrder);
				max += m_props.thickness * dir;
				opts.color.start.w = opts.color.end.w = Math::Lerp(startAlpha, 0.0f, static_cast<float>(i) / static_cast<float>(m_props.steps));
			}
		}
	}
} // namespace Lina
