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
#include "Core/GUI/Widgets/WidgetManager.hpp"

#include <LinaGX/Core/InputMappings.hpp>

namespace Lina
{
	void Slider::Construct()
	{
		m_handle									= m_manager->Allocate<Icon>("Handle");
		m_handle->GetProps().enableHoverPressColors = true;
		m_handle->GetProps().colorHovered			= Theme::GetDef().accentPrimary2;
		m_handle->GetProps().colorStart				= Theme::GetDef().accentPrimary0;
		m_handle->GetProps().colorEnd				= Theme::GetDef().accentPrimary0;
		m_handle->GetProps().colorPressed			= Theme::GetDef().accentPrimary0;
		m_handle->GetProps().icon					= Theme::GetDef().iconSliderHandle;
		m_handle->GetProps().textScale				= 0.5f;
		m_handle->GetFlags().Set(WF_CONTROLS_DRAW_ORDER);
		AddChild(m_handle);
	}

	void Slider::PreTick()
	{
		if (m_isPressed && !m_lgxWindow->GetInput()->GetMouseButton(LINAGX_MOUSE_0))
			m_isPressed = false;

		if (m_isPressed && m_props.valuePtr)
		{
			const Vector2 mouseAbs	  = m_lgxWindow->GetInput()->GetMousePositionAbs();
			const Vector2 mouse		  = Vector2(mouseAbs.x - static_cast<float>(m_lgxWindow->GetPosition().x), mouseAbs.y - static_cast<float>(m_lgxWindow->GetPosition().y));
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
				if (m_props.onValueChanged)
					m_props.onValueChanged(*m_props.valuePtr);
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
		if (!GetIsVisible())
			return;

		const bool hasControls = HasControls();

		LinaVG::StyleOptions bg;
		bg.color					= m_props.colorBackground.AsLVG4();
		bg.rounding					= m_props.rounding;
		bg.outlineOptions.thickness = m_props.outlineThickness;
		bg.outlineOptions.color		= hasControls ? m_props.colorOutlineControls.AsLVG4() : m_props.colorOutline.AsLVG4();
		m_lvg->DrawRect(m_bgStart.AsLVG(), m_bgEnd.AsLVG(), bg, 0.0f, m_drawOrder);

		if (m_props.valuePtr == nullptr)
			return;

		const float			 fillPercent = Math::Remap(*m_props.valuePtr, m_props.minValue, m_props.maxValue, 0.0f, 1.0f);
		LinaVG::StyleOptions fill;
		fill.color.start		= m_props.direction == DirectionOrientation::Horizontal ? m_props.colorFillMin.AsLVG4() : m_props.colorFillMax.AsLVG4();
		fill.color.end			= m_props.direction == DirectionOrientation::Horizontal ? m_props.colorFillMax.AsLVG4() : m_props.colorFillMin.AsLVG4();
		fill.color.gradientType = m_props.direction == DirectionOrientation::Horizontal ? LinaVG::GradientType::Horizontal : LinaVG::GradientType::Vertical;
		m_lvg->DrawRect(m_fillStart.AsLVG(), m_fillEnd.AsLVG(), fill, 0.0f, m_drawOrder);

		m_handle->Draw();
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
			m_isPressed	 = true;
			m_pressStart = m_lgxWindow->GetInput()->GetMousePositionAbs();
			GrabControls();
			;
			return true;
		}

		if (act == LinaGX::InputAction::Released && m_isPressed)
		{
			m_isPressed = false;
			return true;
		}

		return false;
	}

	bool Slider::OnKey(uint32 keycode, int32 scancode, LinaGX::InputAction act)
	{
		if (!HasControls())
			return false;

		if (act == LinaGX::InputAction::Released)
			return false;

		auto stepValue = [&](float direction) {
			const float step = Math::Equals(m_props.step, 0.0f, 0.001f) ? (m_props.maxValue - m_props.minValue) * 0.1f : m_props.step;
			*m_props.valuePtr += step * direction;
			*m_props.valuePtr = Math::Clamp(*m_props.valuePtr, m_props.minValue, m_props.maxValue);

			if (m_props.onValueChanged)
				m_props.onValueChanged(*m_props.valuePtr);
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
