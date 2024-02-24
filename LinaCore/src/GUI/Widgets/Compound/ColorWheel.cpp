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

#include "Core/GUI/Widgets/Compound/ColorWheel.hpp"
#include "Core/GUI/Widgets/Primitives/ColorSlider.hpp"
#include "Core/Graphics/CommonGraphics.hpp"
#include "Common/Math/Math.hpp"
#include "Common/Platform/LinaVGIncl.hpp"

namespace Lina
{
	void ColorWheel::Construct()
	{
		m_saturationSlider						 = Allocate<ColorSlider>();
		m_saturationSlider->GetProps().direction = WidgetDirection::Vertical;
		m_saturationSlider->GetProps().value	 = &m_saturation;
		m_valueSlider							 = Allocate<ColorSlider>();
		m_valueSlider->GetProps().direction		 = WidgetDirection::Vertical;
		m_valueSlider->GetProps().value			 = &m_value;
		AddChild(m_saturationSlider);
		AddChild(m_valueSlider);
	}

	void ColorWheel::Tick(float delta)
	{
		// Calc rects.
		m_topRect.pos		  = Vector2(m_rect.pos.x, m_rect.pos.y);
		m_topRect.size		  = Vector2(m_rect.size.x, m_rect.size.y * TOP_RECT_RATIO);
		m_botRect.pos		  = Vector2(m_rect.pos.x, m_rect.pos.y + m_rect.size.y * TOP_RECT_RATIO);
		m_botRect.size		  = Vector2(m_rect.size.x, m_rect.size.y * (1.0f - TOP_RECT_RATIO));
		m_colorWheelRect.pos  = Vector2(m_rect.pos.x, m_rect.pos.y);
		m_colorWheelRect.size = Vector2(m_rect.size.y * TOP_RECT_RATIO, m_rect.size.y * TOP_RECT_RATIO);
		m_svRect.pos		  = Vector2(m_rect.pos.x + m_colorWheelRect.size.x, m_rect.pos.y);
		m_svRect.size		  = Vector2(m_rect.size.x - m_colorWheelRect.size.x, m_colorWheelRect.size.y);

		// Place saturation & value sliders
	}

	void ColorWheel::Draw(int32 threadIndex)
	{
		// Color wheel.
		LinaVG::StyleOptions wheelStyle;
		wheelStyle.textureHandle = GUI_TEXTURE_COLORWHEEL;
		LinaVG::DrawCircle(threadIndex, m_colorWheelRect.GetCenter().AsLVG(), m_colorWheelRect.size.x * 0.4f, wheelStyle, 64, 0.0f, 0.0f, 360.0f, m_drawOrder);

		// Saturation & value sliders.
	}
} // namespace Lina
