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

#include "Core/GUI/Widgets/Primitives/ColorWheel.hpp"
#include "Core/GUI/Widgets/Primitives/Icon.hpp"
#include "Core/Graphics/CommonGraphics.hpp"
#include "Common/Math/Math.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina
{
	void ColorWheel::Construct()
	{
		m_icon								   = m_manager->Allocate<Icon>();
		m_icon->GetProps().icon				   = Theme::GetDef().iconColorWheelPointer;
		m_icon->GetProps().sdfOutlineColor	   = Theme::GetDef().background0;
		m_icon->GetProps().sdfThickness		   = 0.6f;
		m_icon->GetProps().sdfSoftness		   = 0.5f;
		m_icon->GetProps().sdfOutlineThickness = 0.525f;
		m_icon->GetProps().sdfOutlineSoftness  = 0.2f;
		AddChild(m_icon);
	}

	void ColorWheel::PreTick()
	{
		if (m_isPressed)
		{
			const float	   wheelRadius = m_rect.size.x * 0.5f;
			const Vector2& mouse	   = m_lgxWindow->GetMousePosition();
			const Vector2  relative	   = mouse - m_rect.GetCenter();
			const float	   x		   = Math::Clamp(Math::Remap(relative.x, -m_rect.size.x * 0.5f, m_rect.size.x * 0.5f, -1.0f, 1.0f), -1.0f, 1.0f);
			const float	   y		   = Math::Clamp(Math::Remap(relative.y, -m_rect.size.y * 0.5f, m_rect.size.y * 0.5f, -1.0f, 1.0f), -1.0f, 1.0f);
			m_pointerPos			   = Vector2(x, y);
			m_pointerPos			   = m_pointerPos.ClampMagnitude(1.0f);

			const float saturation = m_pointerPos.Magnitude();
			float		angle	   = Math::Atan2(m_pointerPos.y, m_pointerPos.x);
			if (angle < 0)
			{
				angle += 2 * MATH_PI; // Ensure the angle is positive
			}

			if (m_props.hue)
				*m_props.hue = RAD_2_DEG(angle);

			if (m_props.saturation)
				*m_props.saturation = saturation;

			if (m_props.onValueChanged)
				m_props.onValueChanged(m_props.hue ? *m_props.hue : 0.0f, m_props.saturation ? *m_props.saturation : 0.0f);
		}
		else
		{
			if (m_props.hue && m_props.saturation)
			{
				const float hueRadians = DEG2RAD(*m_props.hue);
				const float x		   = Math::Cos(hueRadians) * *m_props.saturation;
				const float y		   = Math::Sin(hueRadians) * *m_props.saturation;
				m_pointerPos		   = Vector2(x, y);
			}
		}
	}

	void ColorWheel::Tick(float delta)
	{
		m_icon->SetPos(m_rect.GetCenter() + (m_pointerPos * m_rect.size.x * 0.5f) - m_icon->GetHalfSize());
	}

	void ColorWheel::Draw()
	{
		if (!GetIsVisible())
			return;

		LinaVG::StyleOptions wheelStyle;
		wheelStyle.color		 = Math::Lerp(Color::Black, Color::White, Math::Max(m_props.darknessAlpha, 0.1f)).AsLVG4();
		wheelStyle.textureHandle = GUI_TEXTURE_COLORWHEEL;
		m_lvg->DrawRect(m_rect.pos.AsLVG(), m_rect.GetEnd().AsLVG(), wheelStyle, 0.0f, m_drawOrder);
		m_icon->Draw();
	}

	bool ColorWheel::OnMouse(uint32 button, LinaGX::InputAction act)
	{
		if (button != LINAGX_MOUSE_0)
			return false;

		if (m_isHovered && act == LinaGX::InputAction::Pressed)
		{
			m_manager->GrabControls(this);
			;
			m_isPressed = true;
			return true;
		}

		if (m_isPressed && act == LinaGX::InputAction::Released)
		{
			m_isPressed = false;
			return true;
		}

		return false;
	}
} // namespace Lina
