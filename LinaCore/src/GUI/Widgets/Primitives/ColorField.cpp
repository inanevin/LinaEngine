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

#include "Core/GUI/Widgets/Primitives/ColorField.hpp"
#include "Core/Graphics/CommonGraphics.hpp"
#include "Common/Math/Math.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina
{
	void ColorField::Construct()
	{
		GetWidgetProps().drawBackground		= true;
		GetWidgetProps().drawOrderIncrement = 1;

		Widget* bg = m_manager->Allocate<Widget>("CheckerBG");
		bg->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		bg->SetAlignedPos(Vector2::Zero);
		bg->SetAlignedSize(Vector2::One);
		bg->GetWidgetProps().drawBackground		 = true;
		bg->GetWidgetProps().colorBackground	 = Color(0, 0, 0, 1);
		bg->GetWidgetProps().activeTextureTiling = true;
		bg->GetWidgetProps().textureTiling		 = Vector2(3.0f, 3.0f);
		bg->GetWidgetProps().outlineThickness	 = 0.0f;
		bg->GetWidgetProps().rounding			 = 0.0f;
		bg->GetWidgetProps().drawOrderIncrement	 = -1;
		AddChild(bg);
		m_bg = bg;
	}

	void ColorField::Initialize()
	{
		Widget::Initialize();
		return;

		if (m_props.backgroundTexture != nullptr)
		{
		}
	}

	void ColorField::Tick(float delta)
	{
		if (m_props.backgroundTexture)
		{
			m_bg->GetWidgetProps().rawTexture	   = m_props.backgroundTexture;
			m_bg->GetWidgetProps().colorBackground = Color::White;

			if (m_props.value)
				GetWidgetProps().colorBackground = m_props.convertToLinear ? m_props.value->SRGB2Linear() : *m_props.value;

			if (m_props.gradValue)
			{
				GetWidgetProps().colorBackground.start = m_props.convertToLinear ? m_props.gradValue->start.SRGB2Linear() : m_props.gradValue->start;
				GetWidgetProps().colorBackground.end   = m_props.convertToLinear ? m_props.gradValue->end.SRGB2Linear() : m_props.gradValue->end;
			}
		}
		else
		{
			m_bg->GetWidgetProps().rawTexture	   = m_props.backgroundTexture;
			m_bg->GetWidgetProps().colorBackground = Color(0, 0, 0, 0);

			if (m_props.value)
				GetWidgetProps().colorBackground = m_props.convertToLinear ? m_props.value->SRGB2Linear() : *m_props.value;

			if (m_props.gradValue)
			{
				GetWidgetProps().colorBackground.start = m_props.convertToLinear ? m_props.gradValue->start.SRGB2Linear() : m_props.gradValue->start;
				GetWidgetProps().colorBackground.end   = m_props.convertToLinear ? m_props.gradValue->end.SRGB2Linear() : m_props.gradValue->end;
			}
		}
	}

	bool ColorField::OnMouse(uint32 button, LinaGX::InputAction action)
	{
		if (button != LINAGX_MOUSE_0)
			return false;

		if (m_props.disableInput)
			return false;

		if (m_isHovered && (action == LinaGX::InputAction::Pressed || action == LinaGX::InputAction::Repeated))
		{
			m_isPressed = true;
			m_manager->GrabControls(this);
			return true;
		}

		if (m_isPressed && action == LinaGX::InputAction::Released)
		{
			if (m_isHovered)
			{
				if (m_props.onClicked)
					m_props.onClicked();
			}

			m_isPressed = false;
			return true;
		}

		return false;
	}

} // namespace Lina
