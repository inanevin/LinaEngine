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

#include "Core/GUI/Widgets/Primitives/Selectable.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/Math/Math.hpp"
#include "Common/Data/CommonData.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina
{

	void Selectable::PreTick()
	{
		if (m_wasSelected && GetControlsOwner() != this)
		{
			m_wasSelected = false;
			if (m_props.onSelectionChanged)
				m_props.onSelectionChanged(false);
		}
	}

	void Selectable::Tick(float dt)
	{
		const bool hasControls = GetControlsOwner() == this;
		if (hasControls)
		{
			m_usedStart = Math::Lerp(m_usedStart, m_props.colorSelectedStart, dt * COLOR_SPEED);
			m_usedEnd	= Math::Lerp(m_usedEnd, m_props.colorSelectedEnd, dt * COLOR_SPEED);
		}
		else
		{
			m_usedStart = Math::Lerp(m_usedStart, m_props.colorStart, dt * COLOR_SPEED);
			m_usedEnd	= Math::Lerp(m_usedEnd, m_props.colorEnd, dt * COLOR_SPEED);
		}
	}

	void Selectable::Draw(int32 threadIndex)
	{
		if (!GetIsVisible())
			return;

		const bool hasControls = GetControlsOwner() == this;

		LinaVG::StyleOptions opts;
		opts.color.start			  = m_usedStart.AsLVG4();
		opts.color.end				  = m_usedEnd.AsLVG4();
		opts.rounding				  = m_props.rounding;
		opts.outlineOptions.thickness = m_props.outlineThickness;
		opts.outlineOptions.color	  = m_props.colorOutline.AsLVG4();
		opts.color.gradientType		  = LinaVG::GradientType::Vertical;
		LinaVG::DrawRect(threadIndex, m_rect.pos.AsLVG(), m_rect.GetEnd().AsLVG(), opts, 0.0f, m_drawOrder);

		Widget::Draw(threadIndex);

		Widget::DrawTooltip(threadIndex);
	}

	bool Selectable::OnKey(uint32 keycode, int32 scancode, LinaGX::InputAction act)
	{
		if (act == LinaGX::InputAction::Released)
			return false;

		if (GetControlsOwner() != this)
			return false;

		if (keycode == LINAGX_KEY_RETURN)
		{
			if (m_props.onInteracted)
				m_props.onInteracted();

			return true;
		}

		if (m_props.moveNextArrowHorizontal)
		{
			if (keycode == LINAGX_KEY_RIGHT)
			{
				FindGetControlsManager()->MoveControlsToNext();
				return true;
			}

			if (keycode == LINAGX_KEY_LEFT)
			{
				FindGetControlsManager()->MoveControlsToPrev();
				return true;
			}
		}
		else if (m_props.moveNextArrowVertical)
		{
			if (keycode == LINAGX_KEY_DOWN)
			{
				FindGetControlsManager()->MoveControlsToNext();
				return true;
			}

			if (keycode == LINAGX_KEY_UP)
			{
				FindGetControlsManager()->MoveControlsToPrev();
				return true;
			}
		}

		return false;
	}

	bool Selectable::OnMouse(uint32 button, LinaGX::InputAction act)
	{
		if (button != LINAGX_MOUSE_0)
			return false;

		if (m_isHovered && act == LinaGX::InputAction::Repeated)
		{
			if (m_props.onInteracted)
				m_props.onInteracted();
			return true;
		}

		if (GetControlsOwner() == this)
			return false;

		if (m_isHovered && (act == LinaGX::InputAction::Pressed || act == LinaGX::InputAction::Repeated))
		{
			m_wasSelected = true;
			if (m_props.onSelectionChanged)
				m_props.onSelectionChanged(true);

			GrabControls(this);

			return true;
		}

		return false;
	}

} // namespace Lina
