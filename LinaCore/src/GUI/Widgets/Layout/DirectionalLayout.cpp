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

#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Common/Data/CommonData.hpp"
#include "Common/Math/Math.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Core/GUI/Widgets/WidgetUtility.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina
{
	void DirectionalLayout::Destruct()
	{
		if (m_props.onDestructed)
			m_props.onDestructed();
	}

	void DirectionalLayout::Initialize()
	{
		Widget::Initialize();
	}

	void DirectionalLayout::PreTick()
	{
		if (GetIsHovered() && !m_lastHoverStatus)
		{
			if (m_props.onHoverBegin)
				m_props.onHoverBegin();
		}
		else if (!GetIsHovered() && m_lastHoverStatus)
		{
			if (m_props.onHoverEnd)
				m_props.onHoverEnd();
		}
	}

	void DirectionalLayout::Tick(float delta)
	{
		m_lastHoverStatus = GetIsHovered();

		m_start	 = GetStartFromMargins();
		m_end	 = GetEndFromMargins();
		m_sz	 = m_end - m_start;
		m_center = (m_start + m_end) * 0.5f;

		if (m_props.direction == DirectionOrientation::Horizontal)
			m_start.x -= m_scrollerOffset;
		else
			m_start.y -= m_scrollerOffset;

		if (m_props.mode == Mode::Default)
			BehaviourDefault(delta);
		else if (m_props.mode == Mode::EqualPositions)
			BehaviourEqualPositions(delta);
		else if (m_props.mode == Mode::EqualSizes)
			BehaviourEqualSizes(delta);
		else if (m_props.mode == Mode::Bordered)
			BehaviourBorders(delta);
	}

	void DirectionalLayout::BehaviourBorders(float delta)
	{
		if (m_children.empty())
			return;

		m_borderRects.resize(m_children.size() - 1);

		const uint32 sz = static_cast<uint32>(m_children.size());

		if (m_pressedBorder != -1)
		{
			Widget*		negative = m_children[m_pressedBorder];
			Widget*		positive = m_children[m_pressedBorder + 1];
			const Rect& r		 = m_borderRects[m_pressedBorder];

			const Vector2& mousePosition   = m_lgxWindow->GetMousePosition();
			const Vector2  perc			   = mousePosition / GetSize();
			const float	   borderSizePercX = m_props.borderThickness / m_sz.x;
			const float	   borderSizePercY = m_props.borderThickness / m_sz.y;

			if (m_props.direction == DirectionOrientation::Horizontal)
			{
				const float desiredXPos = mousePosition.x - m_borderPressDiff;
				const float deltaXPos	= desiredXPos - r.GetCenter().x;
				const float deltaPerc	= deltaXPos / m_sz.x;
				const float percAmt		= Math::Abs(deltaPerc);

				if (Math::Equals(percAmt, 0.0f, 0.0001f))
					return;

				if (deltaPerc > 0.0f)
				{
					if (WidgetUtility::CheckIfCanShrinkWidgets({positive}, percAmt, true, m_props.borderMinSize * m_sz.x))
					{
						positive->AddAlignedPosX(percAmt);
						positive->AddAlignedSizeX(-percAmt);
						negative->AddAlignedSizeX(percAmt);
					}
				}
				else
				{
					if (WidgetUtility::CheckIfCanShrinkWidgets({negative}, percAmt, true, m_props.borderMinSize * m_sz.x))
					{
						negative->AddAlignedSizeX(-percAmt);
						positive->AddAlignedPosX(-percAmt);
						positive->AddAlignedSizeX(percAmt);
					}
				}
			}
			else
			{
				const float desiredYPos = mousePosition.y - m_borderPressDiff;
				const float deltaYPos	= desiredYPos - r.GetCenter().y;
				const float deltaPerc	= deltaYPos / m_sz.y;
				const float percAmt		= Math::Abs(deltaPerc);

				if (deltaPerc > 0.0f)
				{
					if (WidgetUtility::CheckIfCanShrinkWidgets({positive}, percAmt, false, m_props.borderMinSize * m_sz.y))
					{
						positive->AddAlignedPosY(percAmt);
						positive->AddAlignedSizeY(-percAmt);
						negative->AddAlignedSizeY(percAmt);
					}
				}
				else
				{
					if (WidgetUtility::CheckIfCanShrinkWidgets({negative}, percAmt, false, m_props.borderMinSize * m_sz.y))
					{
						negative->AddAlignedSizeY(-percAmt);
						positive->AddAlignedPosY(-percAmt);
						positive->AddAlignedSizeY(percAmt);
					}
				}
			}
		}

		for (uint32 i = 0; i < sz; i++)
		{
			Widget* c = m_children.at(i);

			if (m_props.direction == DirectionOrientation::Horizontal)
			{
				c->SetPosX(m_start.x + GetSizeX() * c->GetAlignedPosX());
				c->SetSizeX(GetSizeX() * c->GetAlignedSizeX());

				if (i < sz - 1)
				{
					Rect& rect = m_borderRects[i];
					rect.pos   = Vector2(m_start.x + (GetSizeX() * c->GetAlignedPosX() + GetSizeX() * c->GetAlignedSizeX()) - m_props.borderThickness * 0.5f, m_start.y);
					rect.size  = Vector2(m_props.borderThickness, m_end.y - m_start.y);
				}
			}
			else
			{
				c->SetPosY(m_start.y + GetSizeY() * c->GetAlignedPosY());
				c->SetSizeY(GetSizeY() * c->GetAlignedSizeY());

				if (i < sz - 1)
				{
					Rect& rect = m_borderRects[i];
					rect.pos   = Vector2(m_start.x, m_start.y + (GetSizeY() * c->GetAlignedPosY() + GetSizeY() * c->GetAlignedSizeY()) - m_props.borderThickness * 0.5f);
					rect.size  = Vector2(m_end.x - m_start.x, m_props.borderThickness);
				}
			}
		}
	}
	void DirectionalLayout::BehaviourEqualSizes(float delta)
	{
		if (m_children.empty())
			return;

		const float totalAvailableSize = (m_props.direction == DirectionOrientation::Horizontal ? m_sz.x : m_sz.y) - (static_cast<float>(m_children.size() - 1) * GetWidgetProps().childPadding);
		const float perItemSize		   = totalAvailableSize / static_cast<float>(m_children.size());

		float pos = m_props.direction == DirectionOrientation::Horizontal ? m_start.x : m_start.y;
		for (auto* c : m_children)
		{
			if (m_props.direction == DirectionOrientation::Horizontal)
			{
				c->SetSizeX(perItemSize);
				c->SetPosX(pos);
			}
			else
			{
				c->SetSizeY(perItemSize);
				c->SetPosY(pos);
			}

			pos += perItemSize + GetWidgetProps().childPadding;
		}
	}

	void DirectionalLayout::BehaviourEqualPositions(float delta)
	{
		if (m_children.empty())
			return;

		float totalSizeMainAxis = 0.0f;

		for (auto* c : m_children)
		{
			const Vector2& sz = c->GetSize();

			if (m_props.direction == DirectionOrientation::Horizontal)
				totalSizeMainAxis += sz.x;
			else
				totalSizeMainAxis += sz.y;
		}

		const float remainingSize = m_props.direction == DirectionOrientation::Horizontal ? (m_sz.x - totalSizeMainAxis) : (m_sz.y - totalSizeMainAxis);
		const float pad			  = remainingSize / static_cast<float>(m_children.size() + 1);

		float x = m_start.x;
		float y = m_start.y;

		if (m_props.direction == DirectionOrientation::Horizontal)
			x += pad;
		else
			y += pad;

		for (auto* c : m_children)
		{
			if (m_props.direction == DirectionOrientation::Horizontal)
			{
				c->SetPosX(x);
				x += pad + c->GetSize().x;
			}
			else
			{
				c->SetPosY(y);
				y += pad + c->GetSize().y;
			}
		}
	}

	void DirectionalLayout::BehaviourDefault(float delta)
	{
		float x = m_start.x;
		float y = m_start.y;

		size_t idx = 0;
		for (auto* c : m_children)
		{
			if (c->GetFlags().IsSet(WF_HIDE))
				continue;

			// First set pos.
			if (m_props.direction == DirectionOrientation::Horizontal)
				c->SetPosX(x);
			else
				c->SetPosY(y);
			const float incrementSize = m_props.direction == DirectionOrientation::Horizontal ? c->GetSizeX() : c->GetSizeY();
			const bool	lastItem	  = idx == m_children.size() - 1;

			if (m_props.direction == DirectionOrientation::Horizontal)
				x += incrementSize + (lastItem ? 0.0f : GetWidgetProps().childPadding);
			else
				y += incrementSize + (lastItem ? 0.0f : GetWidgetProps().childPadding);

			idx++;
		}
	}

	void DirectionalLayout::Draw()
	{
		const Vector2 start = GetStartFromMargins();
		const Vector2 end	= GetEndFromMargins();
		if (end.x - start.x < 1.0f || end.y - start.y < 1.0f)
			return;
		Widget::Draw();

		if (m_props.mode != Mode::Bordered || m_children.empty())
			return;

		LinaVG::StyleOptions opts;

		const uint32 childSz = static_cast<uint32>(m_children.size());

		for (uint32 i = 0; i < childSz - 1; i++)
		{
			const Rect& r = m_borderRects[i];

			if (m_pressedBorder != -1)
				opts.color = m_pressedBorder == i ? m_props.colorBorderHovered.AsLVG4() : m_props.colorBorder.AsLVG4();
			else
				opts.color = r.IsPointInside(m_lgxWindow->GetMousePosition()) ? m_props.colorBorderHovered.AsLVG4() : m_props.colorBorder.AsLVG4();

			m_lvg->DrawRect(r.pos.AsLVG(), r.GetEnd().AsLVG(), opts, 0.0f, m_drawOrder);
		}
	}

	void DirectionalLayout::DebugDraw(int32 drawOrder)
	{
		LinaVG::StyleOptions opts;
		opts.color	   = Color::Red.AsLVG4();
		opts.isFilled  = false;
		opts.thickness = 2.0f;
		m_lvg->DrawRect(m_start.AsLVG(), m_end.AsLVG(), opts, 0.0f, drawOrder);
	}

	bool DirectionalLayout::OnMouse(uint32 button, LinaGX::InputAction act)
	{
		if (m_props.mode == Mode::Bordered && button == LINAGX_MOUSE_0)
		{
			if (act == LinaGX::InputAction::Pressed)
			{
				int32 i = 0;
				for (const Rect& rect : m_borderRects)
				{
					if (rect.IsPointInside(m_lgxWindow->GetMousePosition()))
					{
						m_pressedBorder	  = i;
						m_borderPressDiff = m_props.direction == DirectionOrientation::Vertical ? (m_lgxWindow->GetMousePosition().y - rect.GetCenter().y) : (m_lgxWindow->GetMousePosition().x - rect.GetCenter().x);

						return true;
					}
					i++;
				}
			}
			else
			{
				if (m_pressedBorder != -1)
				{
					m_pressedBorder = -1;
					return true;
				}
			}
		}
		if (!m_props.receiveInput)
			return false;

		if (button == LINAGX_MOUSE_0 && m_isHovered && act == LinaGX::InputAction::Repeated)
		{
			if (m_props.onDoubleClicked)
				m_props.onDoubleClicked();

			return true;
		}

		if (button == LINAGX_MOUSE_1 && m_isHovered && act == LinaGX::InputAction::Pressed)
		{
			if (m_props.onRightClicked)
				m_props.onRightClicked();

			return true;
		}

		if (button != LINAGX_MOUSE_0)
			return false;

		if ((act == LinaGX::InputAction::Pressed || act == LinaGX::InputAction::Repeated) && m_isHovered)
		{
			m_isPressed = true;

			if (m_props.onPressed)
				m_props.onPressed();

			return true;
		}

		if (act == LinaGX::InputAction::Released)
		{
			if (m_isPressed && m_isHovered)
			{
				if (m_props.onClicked)
					m_props.onClicked();

				m_isPressed = false;
				return true;
			}

			if (m_isPressed)
			{
				m_isPressed = false;
				return true;
			}
		}

		return false;
	}

	float DirectionalLayout::CalculateChildrenSize()
	{
		float sz = 0.0f;
		for (auto* c : m_children)
		{
			if (m_props.direction == DirectionOrientation::Horizontal)
				sz += c->GetSizeX() + GetWidgetProps().childPadding;
			else
				sz += c->GetSizeY() + GetWidgetProps().childPadding;
		}

		return sz - GetWidgetProps().childPadding;
	}

	void DirectionalLayout::Properties::SaveToStream(OStream& stream) const
	{
		stream << mode << direction << colorBorder << colorBorderHovered;
		stream << borderThickness;
		stream << borderMinSize;
	}

	void DirectionalLayout::Properties::LoadFromStream(IStream& stream)
	{
		stream >> mode >> direction >> colorBorder >> colorBorderHovered;
		stream >> borderThickness;
		stream >> borderMinSize;
	}

	LinaGX::CursorType DirectionalLayout::GetCursorOverride()
	{
		if (m_props.mode == Mode::Bordered)
		{
			for (const Rect& rect : m_borderRects)
			{
				if (rect.IsPointInside(m_lgxWindow->GetMousePosition()))
					return m_props.direction == DirectionOrientation::Horizontal ? LinaGX::CursorType::SizeHorizontal : LinaGX::CursorType::SizeVertical;
			}
		}
		return LinaGX::CursorType::Default;
	}
} // namespace Lina
