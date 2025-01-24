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
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina
{
	void ColorSlider::Initialize()
	{
		if (m_props.backgroundTexture != nullptr)
		{
			Widget* bg = m_manager->Allocate<Widget>("TextureBG");
			bg->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
			bg->SetAlignedPos(Vector2::Zero);
			bg->SetAlignedSize(Vector2::One);
			bg->GetWidgetProps().drawBackground		 = true;
			bg->GetWidgetProps().colorBackground	 = Color::White;
			bg->GetWidgetProps().outlineThickness	 = 0.0f;
			bg->GetWidgetProps().rounding			 = 0.0f;
			bg->GetWidgetProps().textureTiling		 = Vector2(3.0f, 3.0f);
			bg->GetWidgetProps().activeTextureTiling = true;
			bg->GetWidgetProps().rawTexture			 = m_props.backgroundTexture;
			AddChild(bg);
		}
	}

	void ColorSlider::PreTick()
	{
		if (m_isPressed && m_props.value)
		{
			const Vector2 mouse		  = m_manager->GetMousePosition();
			float		  targetValue = 0.0f;

			if (m_widgetProps.colorBackgroundDirection == DirectionOrientation::Horizontal)
				targetValue = Math::Remap(mouse.x, m_rect.pos.x, m_rect.pos.x + m_rect.size.x, m_props.minValue, m_props.maxValue);
			else if (m_widgetProps.colorBackgroundDirection == DirectionOrientation::Vertical)
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
			PropagateCBOnEdited();
		}
	}

	void ColorSlider::Draw()
	{
		/*
		if (m_props.backgroundTexture || GetWidgetProps().lvgUserData != nullptr)
		{
			if (!m_children.empty())
				m_children[0]->DrawBackground();
			m_drawOrder++;
			Widget::DrawBackground();
			Widget::DrawBorders();
			Widget::DrawTooltip();
		}
		else
		{
			Widget::Draw();
		}
			*/

		if (!m_props.value)
			return;

		const float lineThickness = Math::FloorToFloat(m_widgetProps.colorBackgroundDirection == DirectionOrientation::Horizontal ? m_rect.size.y * 0.1f : m_rect.size.x * 0.1f);

		LinaVG::StyleOptions line;
		line.color					  = m_props.colorLine.AsLVG4();
		line.outlineOptions.thickness = m_widgetProps.outlineThickness;
		line.outlineOptions.color	  = m_props.colorLineOutline.AsLVG4();

		if (m_widgetProps.colorBackgroundDirection == DirectionOrientation::Horizontal)
		{
			const float lineX = Math::FloorToFloat(m_rect.pos.x + m_rect.size.x * Math::Clamp((*m_props.value), m_props.minValue, m_props.maxValue) / m_props.maxValue);
			m_lvg->DrawRect(Vector2(lineX - lineThickness, m_rect.pos.y).AsLVG(), Vector2(lineX + lineThickness, m_rect.pos.y + m_rect.size.y).AsLVG(), line, 0.0f, m_drawOrder + 1);
		}
		else if (m_widgetProps.colorBackgroundDirection == DirectionOrientation::Vertical)
		{
			const float lineY = Math::FloorToFloat(m_rect.pos.y + m_rect.size.y * (m_props.maxValue - Math::Clamp((*m_props.value), m_props.minValue, m_props.maxValue)) / m_props.maxValue);
			m_lvg->DrawRect(Vector2(m_rect.pos.x, lineY - lineThickness).AsLVG(), Vector2(m_rect.pos.x + m_rect.size.x, lineY + lineThickness).AsLVG(), line, 0.0f, m_drawOrder + 1);
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
			PropagateCBOnEditStarted();
			return true;
		}

		if (m_isPressed && action == LinaGX::InputAction::Released)
		{
			PropagateCBOnEditEnded();
			m_isPressed = false;
			return true;
		}

		return false;
	}

	bool ColorSlider::OnKey(uint32 keycode, int32 scancode, LinaGX::InputAction act)
	{
		if (!m_manager->IsControlsOwner(this))
			return false;

		if (act == LinaGX::InputAction::Released)
			return false;

		auto stepValue = [&](float direction) {
			PropagateCBOnEditStarted();
			const float step = Math::Equals(m_props.step, 0.0f, 0.001f) ? (m_props.maxValue - m_props.minValue) * 0.1f : m_props.step;
			*m_props.value += step * direction;
			*m_props.value = Math::Clamp(*m_props.value, m_props.minValue, m_props.maxValue);
			PropagateCBOnEdited();
		};

		if (m_widgetProps.colorBackgroundDirection == DirectionOrientation::Horizontal && keycode == LINAGX_KEY_LEFT)
		{
			stepValue(-1.0f);
			return true;
		}

		if (m_widgetProps.colorBackgroundDirection == DirectionOrientation::Horizontal && keycode == LINAGX_KEY_RIGHT)
		{
			stepValue(1.0f);
			return true;
		}

		if (m_widgetProps.colorBackgroundDirection == DirectionOrientation::Vertical && keycode == LINAGX_KEY_UP)
		{
			stepValue(1.0f);
			return true;
		}

		if (m_widgetProps.colorBackgroundDirection == DirectionOrientation::Vertical && keycode == LINAGX_KEY_DOWN)
		{
			stepValue(-1.0f);
			return true;
		}

		return false;
	}

} // namespace Lina
