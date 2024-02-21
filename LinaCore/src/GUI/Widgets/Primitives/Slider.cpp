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
		const float fillPercent = Math::Remap(m_props.currentValue, m_props.minValue, m_props.maxValue, 0.0f, 1.0f);
		Vector2		fillStart = Vector2::Zero, fillEnd = Vector2::Zero;
		GetStartEnd(fillStart, fillEnd, fillPercent);

		const Vector2 handlePos = m_props.direction == WidgetDirection::Horizontal ? Vector2(fillEnd.x, (fillEnd.y + fillStart.y) * 0.5f) : Vector2((fillStart.x + fillEnd.x) * 0.5f, fillEnd.y);
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

		Vector2 bgStart = Vector2::Zero, bgEnd = Vector2::Zero;
		GetStartEnd(bgStart, bgEnd, 1.0f);
		LinaVG::DrawRect(threadIndex, bgStart.AsLVG(), bgEnd.AsLVG(), bg, 0.0f, m_drawOrder);

		const float			 fillPercent = Math::Remap(m_props.currentValue, m_props.minValue, m_props.maxValue, 0.0f, 1.0f);
		LinaVG::StyleOptions fill;
		fill.color.start		= m_props.colorFillMin.AsLVG4();
		fill.color.end			= Math::Lerp(m_props.colorFillMin, m_props.colorFillMax, fillPercent).AsLVG4();
		fill.color.gradientType = m_props.direction == WidgetDirection::Horizontal ? LinaVG::GradientType::Horizontal : LinaVG::GradientType::Vertical;

		Vector2 fillStart = Vector2::Zero, fillEnd = Vector2::Zero;
		GetStartEnd(fillStart, fillEnd, fillPercent);
		LinaVG::DrawRect(threadIndex, fillStart.AsLVG(), fillEnd.AsLVG(), fill, 0.0f, m_drawOrder);

		m_handle->Draw(threadIndex);
	}

	void Slider::GetStartEnd(Vector2& outStart, Vector2& outEnd, float fillPercent)
	{
		const Vector2 topLeft	  = m_rect.pos;
		const Vector2 bottomRight = m_rect.pos + m_rect.size;

		if (m_props.direction == WidgetDirection::Horizontal)
		{
			const float thickness = m_rect.size.y * m_props.crossAxisPercentage;
			const float middleY	  = (topLeft.y + bottomRight.y) / 2.0f;
			outStart			  = Vector2(topLeft.x, middleY - thickness * 0.5f);
			outEnd				  = Vector2(Math::Lerp(topLeft.x, bottomRight.x, fillPercent), middleY + thickness * 0.5f);
		}
		else if (m_props.direction == WidgetDirection::Vertical)
		{
			const float thickness = m_rect.size.x * m_props.crossAxisPercentage;
			const float middleX	  = (topLeft.x + bottomRight.x) / 2.0f;
			outStart			  = Vector2(middleX - thickness * 0.5f, topLeft.y);
			outEnd				  = Vector2(middleX + thickness * 0.5f, Math::Lerp(topLeft.y, bottomRight.y, fillPercent));
		}
	}

	Vector2 Slider::GetEndPos()
	{
		Vector2 endPos = m_rect.pos + Vector2(m_rect.size.x, m_rect.size.y * m_props.crossAxisPercentage);
		if (m_props.direction == WidgetDirection::Vertical)
			endPos = m_rect.pos + Vector2(m_rect.size.x * m_props.crossAxisPercentage, m_rect.size.y);
		return endPos;
	}

	bool Slider::OnMouse(uint32 button, LinaGX::InputAction act)
	{
		if (button != LINAGX_MOUSE_0)
		{
			return Widget::OnMouse(button, act);
		}

		if (m_isHovered && act == LinaGX::InputAction::Pressed)
			m_isPressed = true;

		if (act == LinaGX::InputAction::Released)
		{
			m_isPressed = false;
		}

		return true;
	}
} // namespace Lina
