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
#include "Core/GUI/Widgets/WidgetUtility.hpp"
#include "Common/Math/Math.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include <LinaGX/Core/InputMappings.hpp>

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
		AssignSides(m_negative, m_positive);
	}

	void LayoutBorder::PreTick()
	{
		Widget::PreTick();

		if (m_isPressed)
		{
			const Vector2& mousePosition   = m_lgxWindow->GetMousePosition();
			const Vector2  perc			   = mousePosition / m_parent->GetSize();
			const float	   borderSizePercX = Theme::GetDef().baseBorderThickness / m_parent->GetSizeX();
			const float	   borderSizePercY = Theme::GetDef().baseBorderThickness / m_parent->GetSizeY();

			if (m_props.orientation == DirectionOrientation::Horizontal)
			{
				const float desiredYPos = mousePosition.y - m_pressDiff;
				const float deltaYPos	= desiredYPos - GetPosY();
				const float deltaPerc	= deltaYPos / m_parent->GetSizeY();
				const float percAmt		= Math::Abs(deltaPerc);

				if (deltaPerc > 0.0f)
				{
					if (WidgetUtility::CheckIfCanShrinkWidgets({m_positive}, percAmt, false, m_props.minSize))
					{
						m_positive->AddAlignedPosY(percAmt);
						m_positive->AddAlignedSizeY(-percAmt);
						m_negative->AddAlignedSizeY(percAmt);
						m_alignedPos.y = (desiredYPos - m_parent->GetPosY()) / m_parent->GetSizeY();
					}
				}
				else
				{
					if (WidgetUtility::CheckIfCanShrinkWidgets({m_negative}, percAmt, false, m_props.minSize))
					{
						m_negative->AddAlignedSizeY(-percAmt);
						m_positive->AddAlignedPosY(-percAmt);
						m_positive->AddAlignedSizeY(percAmt);
						m_alignedPos.y = (desiredYPos - m_parent->GetPosY()) / m_parent->GetSizeY();
					}
				}
			}
			else
			{
				const float desiredXPos = mousePosition.x - m_pressDiff;
				const float deltaXPos	= desiredXPos - GetPosX();
				const float deltaPerc	= deltaXPos / m_parent->GetSizeX();
				const float percAmt		= Math::Abs(deltaPerc);

				if (Math::Equals(percAmt, 0.0f, 0.0001f))
					return;

				if (deltaPerc > 0.0f)
				{
					if (WidgetUtility::CheckIfCanShrinkWidgets({m_positive}, percAmt, true, m_props.minSize))
					{
						m_positive->AddAlignedPosX(percAmt);
						m_positive->AddAlignedSizeX(-percAmt);
						m_negative->AddAlignedSizeX(percAmt);
						m_alignedPos.x = (desiredXPos - m_parent->GetPosX()) / m_parent->GetSizeX();
					}
				}
				else
				{
					if (WidgetUtility::CheckIfCanShrinkWidgets({m_negative}, percAmt, true, m_props.minSize))
					{
						m_negative->AddAlignedSizeX(-percAmt);
						m_positive->AddAlignedPosX(-percAmt);
						m_positive->AddAlignedSizeX(percAmt);
						m_alignedPos.x = (desiredXPos - m_parent->GetPosX()) / m_parent->GetSizeX();
					}
				}
			}
		}
	}

	void LayoutBorder::Draw(int32 threadIndex)
	{
		LinaVG::StyleOptions opts;
		opts.color = Theme::GetDef().background0.AsLVG4();
		LinaVG::DrawRect(threadIndex, m_rect.pos.AsLVG(), m_rect.GetEnd().AsLVG(), opts, 0.0f, m_drawOrder);
	}

	bool LayoutBorder::OnMouse(uint32 button, LinaGX::InputAction act)
	{
		if (m_isHovered && button == LINAGX_MOUSE_0 && act == LinaGX::InputAction::Pressed)
		{
			m_pressDiff = m_props.orientation == DirectionOrientation::Horizontal ? (m_lgxWindow->GetMousePosition().y - GetPosY()) : (m_lgxWindow->GetMousePosition().x - GetPosX());
			m_isPressed = true;
			return true;
		}

		if (m_isPressed && button == LINAGX_MOUSE_0 && act == LinaGX::InputAction::Released)
		{
			m_isPressed = false;
			return true;
		}

		return false;
	}

	LinaGX::CursorType LayoutBorder::GetCursorOverride()
	{
		if (m_isHovered)
			return m_props.orientation == DirectionOrientation::Horizontal ? LinaGX::CursorType::SizeVertical : LinaGX::CursorType::SizeHorizontal;

		return LinaGX::CursorType::Default;
	}

	void LayoutBorder::AssignSides(Widget* negative, Widget* positive)
	{
		if (negative == nullptr || positive == nullptr)
			return;

		m_negative = negative;
		m_positive = positive;

		if (m_props.orientation == DirectionOrientation::Horizontal)
		{
			if (m_positive->GetChildMargins().left < GetFixedSizeX())
				m_positive->GetChildMargins() = {.left = GetFixedSizeX()};
		}
		else
		{
			if (m_positive->GetChildMargins().top < GetFixedSizeY())
				m_positive->GetChildMargins() = {.top = GetFixedSizeY()};
		}
	}

} // namespace Lina
