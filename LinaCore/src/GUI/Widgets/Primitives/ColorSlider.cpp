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

#include "Core/GUI/Widgets/Primitives/ColorSlider.hpp"
#include "Common/Math/Math.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Core/Graphics/CommonGraphics.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina
{
	void ColorSlider::Tick(float delta)
	{
		Widget::SetIsHovered();

		if (m_isPressed)
		{
			const Vector2 mouse		  = m_lgxWindow->GetMousePosition();
			float		  targetValue = 0.0f;

			if (m_props.direction == WidgetDirection::Horizontal)
				*m_props.value = Math::Remap(mouse.x, m_rect.pos.x, m_rect.pos.x + m_rect.size.x, 0.0f, 1.0f);
			else if (m_props.direction == WidgetDirection::Vertical)
				*m_props.value = Math::Remap(mouse.y, m_rect.pos.y + m_rect.size.y, m_rect.pos.y, 0.0f, 1.0f);

			*m_props.value = Math::Clamp(*m_props.value, 0.0f, 1.0f);
		}
	}

	void ColorSlider::Draw(int32 threadIndex)
	{
		const bool			 hasControls = m_manager->GetControlsOwner() == this;
		LinaVG::StyleOptions opts;
		opts.rounding				  = m_props.rounding;
		opts.outlineOptions.thickness = m_props.outlineThickness;
		opts.outlineOptions.color	  = hasControls ? m_props.colorOutlineControls.AsLVG4() : m_props.colorOutline.AsLVG4();

		if (m_props.isHueShift)
		{
			opts.textureHandle = GUI_TEXTURE_HUE;
		}
		else
		{
			opts.color.start		= m_props.colorBegin.AsLVG4();
			opts.color.end			= m_props.colorEnd.AsLVG4();
			opts.color.gradientType = m_props.direction == WidgetDirection::Horizontal ? LinaVG::GradientType::Horizontal : LinaVG::GradientType::Vertical;
		}

		LinaVG::DrawRect(threadIndex, m_rect.pos.AsLVG(), (m_rect.pos + m_rect.size).AsLVG(), opts, 0.0f, m_drawOrder);

		const float lineThickness = Math::FloorToFloat(m_props.direction == WidgetDirection::Horizontal ? m_rect.size.y * 0.1f : m_rect.size.x * 0.1f);

		LinaVG::StyleOptions line;
		line.color					  = m_props.colorLine.AsLVG4();
		line.outlineOptions.thickness = m_props.outlineThickness;
		line.outlineOptions.color	  = m_props.colorLineOutline.AsLVG4();

		if (m_props.direction == WidgetDirection::Horizontal)
		{
			const float lineX = Math::FloorToFloat(m_rect.pos.x + m_rect.size.x * Math::Clamp((*m_props.value), 0.0f, 1.0f));
			LinaVG::DrawRect(threadIndex, Vector2(lineX - lineThickness, m_rect.pos.y).AsLVG(), Vector2(lineX + lineThickness, m_rect.pos.y + m_rect.size.y).AsLVG(), line, 0.0f, m_drawOrder + 1);
		}
		else if (m_props.direction == WidgetDirection::Vertical)
		{
			const float lineY = Math::FloorToFloat(m_rect.pos.y + m_rect.size.y * (1.0f - Math::Clamp((*m_props.value), 0.0f, 1.0f)));
			LinaVG::DrawRect(threadIndex, Vector2(m_rect.pos.x, lineY - lineThickness).AsLVG(), Vector2(m_rect.pos.x + m_rect.size.x, lineY + lineThickness).AsLVG(), line, 0.0f, m_drawOrder + 1);
		}
	}

	bool ColorSlider::OnMouse(uint32 button, LinaGX::InputAction action)
	{
		if (button != LINAGX_MOUSE_0)
			return false;

		if (m_isHovered && (action == LinaGX::InputAction::Pressed || action == LinaGX::InputAction::Repeated))
		{
			m_isPressed = true;
			m_manager->GrabControls(this);
			return true;
		}

		if (m_isPressed && action == LinaGX::InputAction::Released)
		{
			m_isPressed = false;
			return true;
		}

		return false;
	}

} // namespace Lina
