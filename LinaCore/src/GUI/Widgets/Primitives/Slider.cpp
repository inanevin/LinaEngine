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

#include "Core/GUI/Widgets/Primitives/Slider.hpp"
#include "Core/GUI/Widgets/Primitives/Icon.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/Math/Math.hpp"

#include <LinaGX/Core/InputMappings.hpp>

namespace Lina
{
	void Slider::Construct()
	{
		m_handle								 = Allocate<Icon>();
		m_handle->GetProps().isDynamic			 = true;
		m_handle->GetProps().sdfOutlineThickness = 0.6f;
		m_handle->GetProps().sdfOutlineColor	 = Color::Black;
		AddChild(m_handle);
	}

	void Slider::Tick(float delta)
	{
		Widget::SetIsHovered();

		const float fillPercent = Math::Remap(*m_props.currentValue, m_props.minValue, m_props.maxValue, 0.0f, 1.0f);
		GetStartEnd(m_bgStart, m_bgEnd, 1.0f);
		GetStartEnd(m_fillStart, m_fillEnd, fillPercent);

		if (m_isPressed)
		{
			const Vector2 mouse		  = m_window->GetMousePosition();
			float		  targetValue = 0.0f;

			if (m_props.direction == WidgetDirection::Horizontal)
			{
				const float perc = Math::Remap(mouse.x, m_bgStart.x, m_bgEnd.x, 0.0f, 1.0f);
				targetValue		 = Math::Lerp(m_props.minValue, m_props.maxValue, perc);
			}
			else if (m_props.direction == WidgetDirection::Vertical)
			{
				const float perc = Math::Remap(mouse.y, m_bgEnd.y, m_bgStart.y, 0.0f, 1.0f);
				targetValue		 = Math::Lerp(m_props.minValue, m_props.maxValue, perc);
			}

			if (!Math::IsZero(m_props.step))
			{
				const float prev	  = *m_props.currentValue;
				const float diff	  = targetValue - prev;
				*m_props.currentValue = prev + m_props.step * Math::FloorToFloat(diff / m_props.step);
			}
			else
				*m_props.currentValue = targetValue;

			*m_props.currentValue = Math::Clamp(*m_props.currentValue, m_props.minValue, m_props.maxValue);
		}

		const Vector2 handlePos = m_props.direction == WidgetDirection::Horizontal ? Vector2(m_fillEnd.x, (m_fillEnd.y + m_fillStart.y) * 0.5f) : Vector2((m_fillStart.x + m_fillEnd.x) * 0.5f, m_fillStart.y);
		m_handle->SetPos(handlePos);

		const bool hoverColor	   = (m_handle->GetIsHovered()) && !m_isPressed;
		m_handle->GetProps().color = hoverColor ? m_props.colorHandleHovered : Math::Lerp(m_props.colorFillMin, m_props.colorFillMax, fillPercent);
	}

	void Slider::Draw(int32 threadIndex)
	{
		LinaVG::StyleOptions bg;
		bg.color					= m_props.colorBackground.AsLVG4();
		bg.rounding					= m_props.rounding;
		bg.outlineOptions.thickness = m_props.outlineThickness;
		bg.outlineOptions.color		= m_props.colorOutline.AsLVG4();
		LinaVG::DrawRect(threadIndex, m_bgStart.AsLVG(), m_bgEnd.AsLVG(), bg, 0.0f, m_drawOrder);

		const float			 fillPercent = Math::Remap(*m_props.currentValue, m_props.minValue, m_props.maxValue, 0.0f, 1.0f);
		LinaVG::StyleOptions fill;
		fill.color.start		= m_props.colorFillMin.AsLVG4();
		fill.color.end			= Math::Lerp(m_props.colorFillMin, m_props.colorFillMax, fillPercent).AsLVG4();
		fill.color.gradientType = m_props.direction == WidgetDirection::Horizontal ? LinaVG::GradientType::Horizontal : LinaVG::GradientType::Vertical;
		LinaVG::DrawRect(threadIndex, m_fillStart.AsLVG(), m_fillEnd.AsLVG(), fill, 0.0f, m_drawOrder);

		m_handle->Draw(threadIndex);
	}

	void Slider::GetStartEnd(Vector2& outStart, Vector2& outEnd, float fillPercent)
	{

		if (m_props.direction == WidgetDirection::Horizontal)
		{
			const Vector2 topLeft	  = m_rect.pos + Vector2(m_props.indent, 0);
			const Vector2 bottomRight = m_rect.pos + m_rect.size - Vector2(m_props.indent, 0);
			const float	  thickness	  = m_rect.size.y * m_props.crossAxisPercentage;
			const float	  middleY	  = (topLeft.y + bottomRight.y) / 2.0f;
			outStart				  = Vector2(topLeft.x, middleY - thickness * 0.5f);
			outEnd					  = Vector2(Math::Lerp(topLeft.x, bottomRight.x, fillPercent), middleY + thickness * 0.5f);
		}
		else if (m_props.direction == WidgetDirection::Vertical)
		{
			const Vector2 topLeft	  = m_rect.pos + Vector2(0.0f, m_props.indent);
			const Vector2 bottomRight = m_rect.pos + m_rect.size - Vector2(0.0f, m_props.indent);
			const float	  thickness	  = m_rect.size.x * m_props.crossAxisPercentage;
			const float	  middleX	  = (topLeft.x + bottomRight.x) / 2.0f;
			outStart				  = Vector2(middleX - thickness * 0.5f, Math::Lerp(bottomRight.y, topLeft.y, fillPercent));
			outEnd					  = Vector2(middleX + thickness * 0.5f, bottomRight.y);
		}
	}

	bool Slider::OnMouse(uint32 button, LinaGX::InputAction act)
	{
		if (button != LINAGX_MOUSE_0)
		{
			return Widget::OnMouse(button, act);
		}

		if (act == LinaGX::InputAction::Pressed)
		{
			if (m_handle->GetIsHovered() || m_isHovered)
			{
				m_isPressed	 = true;
				m_pressStart = m_window->GetMousePosition();
				Widget::GrabControls();
				return true;
			}
		}

		if (m_isPressed && act == LinaGX::InputAction::Released)
		{
			m_isPressed = false;
			Widget::ReleaseControls();
			return true;
		}

		return false;
	}
} // namespace Lina
