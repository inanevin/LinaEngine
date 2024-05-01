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
		if (m_isPressed && m_props.value)
		{
			const Vector2 mouse		  = m_lgxWindow->GetMousePosition();
			float		  targetValue = 0.0f;

			if (m_props.direction == DirectionOrientation::Horizontal)
				targetValue = Math::Remap(mouse.x, m_rect.pos.x, m_rect.pos.x + m_rect.size.x, m_props.minValue, m_props.maxValue);
			else if (m_props.direction == DirectionOrientation::Vertical)
				targetValue = Math::Remap(mouse.y, m_rect.pos.y + m_rect.size.y, m_rect.pos.y, m_props.minValue, m_props.maxValue);

			if (!Math::IsZero(m_props.step))
			{
				const float prev = *m_props.value;
				const float diff = targetValue - prev;
				*m_props.value	 = prev + m_props.step * Math::FloorToFloat(diff / m_props.step);
			}
			else
				*m_props.value = targetValue;

			*m_props.value = Math::Clamp(*m_props.value, m_props.minValue, m_props.maxValue);

			if (m_props.onValueChanged)
				m_props.onValueChanged(*m_props.value);
		}
	}

	void ColorSlider::Draw(int32 threadIndex)
	{
		int32 drawOrder = m_drawOrder;

		const bool			 hasControls = m_manager->GetControlsOwner() == this;
		LinaVG::StyleOptions opts;
		opts.rounding				  = m_props.rounding;
		opts.outlineOptions.thickness = m_props.outlineThickness;
		opts.outlineOptions.color	  = hasControls ? m_props.colorOutlineControls.AsLVG4() : m_props.colorOutline.AsLVG4();
		opts.color					  = m_props.colorBackground.AsLVG4();
		LinaVG::DrawRect(threadIndex, m_rect.pos.AsLVG(), m_rect.GetEnd().AsLVG(), opts, 0.0f, drawOrder);
		drawOrder++;

		const Vector2 bump = Vector2(m_props.outlineThickness, m_props.outlineThickness);
		if (m_props.drawCheckeredBackground)
		{
			LinaVG::StyleOptions checkered;
			checkered.color			  = Color::White.AsLVG4();
			checkered.textureHandle	  = DEFAULT_TEXTURE_CHECKERED;
			checkered.textureUVTiling = Vector2(m_rect.size.x / 256.0f, m_rect.size.y / 256.0f).AsLVG();
			LinaVG::DrawRect(threadIndex, (m_rect.pos + bump).AsLVG(), (m_rect.GetEnd() - bump).AsLVG(), checkered, 0.0f, drawOrder);
			drawOrder++;
		}

		LinaVG::StyleOptions colorOpts;
		if (m_props.isHueShift)
			colorOpts.textureHandle = m_props.direction == DirectionOrientation::Horizontal ? GUI_TEXTURE_HUE_HORIZONTAL : GUI_TEXTURE_HUE_VERTICAL;
		else
		{
			colorOpts.color.start		 = m_props.colorBegin.AsLVG4();
			colorOpts.color.end			 = m_props.colorEnd.AsLVG4();
			colorOpts.color.gradientType = m_props.direction == DirectionOrientation::Horizontal ? LinaVG::GradientType::Horizontal : LinaVG::GradientType::Vertical;
		}
		LinaVG::DrawRect(threadIndex, (m_rect.pos + bump).AsLVG(), (m_rect.GetEnd() - bump).AsLVG(), colorOpts, 0.0f, drawOrder);
		drawOrder++;

		if (m_props.value == nullptr)
			return;

		const float lineThickness = Math::FloorToFloat(m_props.direction == DirectionOrientation::Horizontal ? m_rect.size.y * 0.1f : m_rect.size.x * 0.1f);

		LinaVG::StyleOptions line;
		line.color					  = m_props.colorLine.AsLVG4();
		line.outlineOptions.thickness = m_props.outlineThickness;
		line.outlineOptions.color	  = m_props.colorLineOutline.AsLVG4();

		if (m_props.direction == DirectionOrientation::Horizontal)
		{
			const float lineX = Math::FloorToFloat(m_rect.pos.x + m_rect.size.x * Math::Clamp((*m_props.value), m_props.minValue, m_props.maxValue) / m_props.maxValue);
			LinaVG::DrawRect(threadIndex, Vector2(lineX - lineThickness, m_rect.pos.y).AsLVG(), Vector2(lineX + lineThickness, m_rect.pos.y + m_rect.size.y).AsLVG(), line, 0.0f, drawOrder);
		}
		else if (m_props.direction == DirectionOrientation::Vertical)
		{
			const float lineY = Math::FloorToFloat(m_rect.pos.y + m_rect.size.y * (m_props.maxValue - Math::Clamp((*m_props.value), m_props.minValue, m_props.maxValue)) / m_props.maxValue);
			LinaVG::DrawRect(threadIndex, Vector2(m_rect.pos.x, lineY - lineThickness).AsLVG(), Vector2(m_rect.pos.x + m_rect.size.x, lineY + lineThickness).AsLVG(), line, 0.0f, drawOrder);
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

	bool ColorSlider::OnKey(uint32 keycode, int32 scancode, LinaGX::InputAction act)
	{
		if (m_manager->GetControlsOwner() != this)
			return false;

		if (act == LinaGX::InputAction::Released)
			return false;

		auto stepValue = [&](float direction) {
			const float step = Math::Equals(m_props.step, 0.0f, 0.001f) ? (m_props.maxValue - m_props.minValue) * 0.1f : m_props.step;
			*m_props.value += step * direction;
			*m_props.value = Math::Clamp(*m_props.value, m_props.minValue, m_props.maxValue);

			if (m_props.onValueChanged)
				m_props.onValueChanged(*m_props.value);
		};

		if (m_props.direction == DirectionOrientation::Horizontal && keycode == LINAGX_KEY_LEFT)
		{
			stepValue(-1.0f);
			return true;
		}

		if (m_props.direction == DirectionOrientation::Horizontal && keycode == LINAGX_KEY_RIGHT)
		{
			stepValue(1.0f);
			return true;
		}

		if (m_props.direction == DirectionOrientation::Vertical && keycode == LINAGX_KEY_UP)
		{
			stepValue(1.0f);
			return true;
		}

		if (m_props.direction == DirectionOrientation::Vertical && keycode == LINAGX_KEY_DOWN)
		{
			stepValue(-1.0f);
			return true;
		}

		return false;
	}

} // namespace Lina
