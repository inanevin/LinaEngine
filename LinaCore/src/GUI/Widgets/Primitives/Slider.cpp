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
#include "Core/GUI/Widgets/WidgetUtility.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/Math/Math.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"

#include <LinaGX/Core/InputMappings.hpp>

namespace Lina
{
	void Slider::Construct()
	{
		m_handle										   = m_manager->Allocate<Icon>("Handle");
		m_handle->GetWidgetProps().hoveredIsDifferentColor = true;
		m_handle->GetWidgetProps().pressedIsDifferentColor = true;
		m_handle->GetWidgetProps().colorHovered			   = Theme::GetDef().accentPrimary2;
		m_handle->GetProps().color						   = Theme::GetDef().accentPrimary0;
		m_handle->GetWidgetProps().colorPressed			   = Theme::GetDef().accentPrimary0;
		m_handle->GetProps().icon						   = Theme::GetDef().iconSliderHandle;
		m_handle->GetProps().textScale					   = 0.5f;
		m_handle->GetFlags().Set(WF_CONTROLS_DRAW_ORDER);
		AddChild(m_handle);
	}

	void Slider::PreTick()
	{
		if (m_isPressed && !m_lgxWindow->GetInput()->GetMouseButton(LINAGX_MOUSE_0))
			m_isPressed = false;

		if (m_isPressed && m_props.valuePtr)
		{
			const Vector2 mouse		  = m_manager->GetMousePosition();
			float		  targetValue = 0.0f;

			if (m_props.direction == DirectionOrientation::Horizontal)
			{
				const float perc = Math::Remap(mouse.x, m_bgStart.x, m_bgEnd.x, 0.0f, 1.0f);
				targetValue		 = Math::Lerp(m_props.minValue, m_props.maxValue, perc);
			}
			else if (m_props.direction == DirectionOrientation::Vertical)
			{
				const float perc = Math::Remap(mouse.y, m_bgEnd.y, m_bgStart.y, 0.0f, 1.0f);
				targetValue		 = Math::Lerp(m_props.minValue, m_props.maxValue, perc);
			}

			const float prev = *m_props.valuePtr;

			if (!Math::IsZero(m_props.step))
			{
				const float prev  = *m_props.valuePtr;
				const float diff  = targetValue - prev;
				*m_props.valuePtr = prev + m_props.step * Math::FloorToFloat(diff / m_props.step);
			}
			else
				*m_props.valuePtr = targetValue;

			*m_props.valuePtr = Math::Clamp(*m_props.valuePtr, m_props.minValue, m_props.maxValue);

			if (!Math::Equals(*m_props.valuePtr, prev, 0.001f))
			{
				PropagateCBOnEdited();
			}
		}
	}

	void Slider::Tick(float delta)
	{
		if (m_props.valuePtr)
			*m_props.valuePtr = Math::Clamp(*m_props.valuePtr, m_props.minValue, m_props.maxValue);

		const float fillPercent = Math::Remap(m_props.valuePtr ? *m_props.valuePtr : m_props.minValue, m_props.minValue, m_props.maxValue, 0.0f, 1.0f);
		GetStartEnd(m_bgStart, m_bgEnd, 1.0f);
		GetStartEnd(m_fillStart, m_fillEnd, fillPercent);

		const Vector2 handlePos = m_props.direction == DirectionOrientation::Horizontal ? Vector2(m_fillEnd.x - m_handle->GetHalfSizeX(), (m_fillEnd.y + m_fillStart.y) * 0.5f - m_handle->GetHalfSizeY())
																						: Vector2((m_fillStart.x + m_fillEnd.x) * 0.5f - m_handle->GetHalfSizeX(), m_fillStart.y - m_handle->GetHalfSizeY());
		m_handle->SetPos(handlePos);
	}

	void Slider::Draw()
	{
		const bool hasControls = m_manager->IsControlsOwner(this);

		LinaVG::StyleOptions bg;
		bg.color					= m_widgetProps.colorBackground.AsLVG();
		bg.rounding					= m_widgetProps.rounding;
		bg.outlineOptions.thickness = m_widgetProps.outlineThickness;
		bg.outlineOptions.color		= hasControls ? m_widgetProps.colorOutlineControls.AsLVG() : m_widgetProps.colorOutline.AsLVG();
		m_lvg->DrawRect(m_bgStart.AsLVG(), m_bgEnd.AsLVG(), bg, 0.0f, m_drawOrder);

		if (m_props.valuePtr == nullptr)
			return;

		const float			 fillPercent = Math::Remap(*m_props.valuePtr, m_props.minValue, m_props.maxValue, 0.0f, 1.0f);
		LinaVG::StyleOptions fill;
		fill.color.start		= m_props.direction == DirectionOrientation::Horizontal ? m_props.colorFill.start.AsLVG4() : m_props.colorFill.end.AsLVG4();
		fill.color.end			= m_props.direction == DirectionOrientation::Horizontal ? m_props.colorFill.end.AsLVG4() : m_props.colorFill.start.AsLVG4();
		fill.color.gradientType = m_props.direction == DirectionOrientation::Horizontal ? LinaVG::GradientType::Horizontal : LinaVG::GradientType::Vertical;
		m_lvg->DrawRect(m_fillStart.AsLVG(), m_fillEnd.AsLVG(), fill, 0.0f, m_drawOrder);
	}

	void Slider::GetStartEnd(Vector2& outStart, Vector2& outEnd, float fillPercent)
	{
		const Vector2 topLeft	  = m_rect.pos;
		const Vector2 bottomRight = m_rect.GetEnd();

		if (m_props.direction == DirectionOrientation::Horizontal)
		{
			const float thickness = m_rect.size.y * m_props.crossAxisPercentage;
			const float middleY	  = (topLeft.y + bottomRight.y) / 2.0f;
			outStart			  = Vector2(topLeft.x, middleY - thickness * 0.5f);
			outEnd				  = Vector2(Math::Lerp(topLeft.x, bottomRight.x, fillPercent), middleY + thickness * 0.5f);
		}
		else if (m_props.direction == DirectionOrientation::Vertical)
		{
			const float thickness = m_rect.size.x * m_props.crossAxisPercentage;
			const float middleX	  = (topLeft.x + bottomRight.x) / 2.0f;
			outStart			  = Vector2(middleX - thickness * 0.5f, Math::Lerp(bottomRight.y, topLeft.y, fillPercent));
			outEnd				  = Vector2(middleX + thickness * 0.5f, bottomRight.y);
		}
	}

	bool Slider::OnMouse(uint32 button, LinaGX::InputAction act)
	{
		if (button != LINAGX_MOUSE_0)
			return false;

		if (act == LinaGX::InputAction::Pressed && (m_handle->GetIsHovered() || m_isHovered))
		{
			PropagateCBOnEditStarted();
			m_isPressed = true;
			m_manager->GrabControls(this);
			return true;
		}

		if (act == LinaGX::InputAction::Released && m_isPressed)
		{
			PropagateCBOnEditEnded();
			m_isPressed = false;
			return true;
		}

		return false;
	}

	bool Slider::OnKey(uint32 keycode, int32 scancode, LinaGX::InputAction act)
	{
		if (!m_manager->IsControlsOwner(this))
			return false;

		if (act == LinaGX::InputAction::Released)
			return false;

		auto stepValue = [&](float direction) {
			PropagateCBOnEditStarted();
			const float step = Math::Equals(m_props.step, 0.0f, 0.001f) ? (m_props.maxValue - m_props.minValue) * 0.1f : m_props.step;
			*m_props.valuePtr += step * direction;
			*m_props.valuePtr = Math::Clamp(*m_props.valuePtr, m_props.minValue, m_props.maxValue);
			PropagateCBOnEdited();
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
