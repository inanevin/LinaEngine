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

#include "Core/GUI/Widgets/Layout/LayoutBorder.hpp"

namespace Lina
{

	void LayoutBorder::Initialize()
	{
		GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SKIP_FLOORING);

		if (m_props.orientation == DirectionOrientation::Horizontal)
		{
			GetFlags().Set(WF_USE_FIXED_SIZE_Y | WF_SIZE_ALIGN_X);
			SetFixedSizeY(Theme::GetDef().baseBorderThickness);
		}
		else
		{
			GetFlags().Set(WF_SIZE_ALIGN_Y | WF_USE_FIXED_SIZE_X);
			SetFixedSizeX(Theme::GetDef().baseBorderThickness);
		}
	}

	void LayoutBorder::PreTick()
	{
	}

	void LayoutBorder::Draw(int32 threadIndex)
	{
	}

	bool LayoutBorder::OnMouse(uint32 button, LinaGX::InputAction act)
	{
	}

	LinaGX::CursorType LayoutBorder::GetCursorOverride()
	{
	}

	void LayoutBorder::AssignSides(Widget* negative, Widget* positive)
	{
		m_negative = negative;
		m_positive = positive;

		if (m_props.orientation == DirectionOrientation::Horizontal)
		{
			m_negative->GetChildMargins() = {};
			m_positive->GetChildMargins() = {.left = GetFixedSizeX()};
		}
		else
		{
			m_negative->GetChildMargins() = {};
			m_positive->GetChildMargins() = {.top = GetFixedSizeY()};
		}
	}

} // namespace Lina
