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
#include "Common/Math/Math.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina
{

	void ColorField::Construct()
	{
		m_lgxWindow->AddListener(this);
	}

	void ColorField::Destruct()
	{
		m_lgxWindow->RemoveListener(this);
	}

	void ColorField::Tick(float delta)
	{
		Widget::SetIsHovered();
	}

	void ColorField::Draw(int32 threadIndex)
	{
		const bool hasControls = m_manager->GetControlsOwner() == this;

		const Color targetColor = *m_props.colorValue;
		const Color hovered		= targetColor + targetColor * m_props.hoverHighlightPerc;

		// Bg
		LinaVG::StyleOptions opts;
		opts.rounding				  = m_props.rounding;
		opts.outlineOptions.thickness = m_props.outlineThickness;
		opts.outlineOptions.color	  = hasControls ? m_props.colorOutlineControls.AsLVG4() : m_props.colorOutline.AsLVG4();
		opts.color					  = m_isHovered ? hovered.AsLVG4() : targetColor.AsLVG4();
		LinaVG::DrawRect(threadIndex, m_rect.pos.AsLVG(), (m_rect.pos + m_rect.size).AsLVG(), opts, 0.0f, m_drawOrder);
	}

	void ColorField::OnWindowMouse(uint32 button, LinaGX::InputAction action)
	{
		if (button != LINAGX_MOUSE_0)
			return;

		if (action == LinaGX::InputAction::Pressed || action == LinaGX::InputAction::Repeated)
		{
			if (m_isHovered)
			{
				m_isPressed = true;
				m_manager->GrabControls(this);
			}
			else
				m_manager->ReleaseControls(this);
		}

		if (action == LinaGX::InputAction::Released)
		{
			if (m_isPressed)
			{
				m_isPressed = false;

				if (m_isHovered)
				{
					if (m_props.onClicked)
						m_props.onClicked();
				}
			}
		}
	}

} // namespace Lina
