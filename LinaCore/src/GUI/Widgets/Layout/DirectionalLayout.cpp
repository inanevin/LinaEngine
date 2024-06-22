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
	}

	void DirectionalLayout::BehaviourEqualSizes(float delta)
	{
		if (m_children.empty())
			return;

		const float totalAvailableSize = (m_props.direction == DirectionOrientation::Horizontal ? m_sz.x : m_sz.y) - (static_cast<float>(m_children.size() - 1) * GetChildPadding());
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

			pos += perItemSize + GetChildPadding();
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
			// First set pos.
			if (m_props.direction == DirectionOrientation::Horizontal)
				c->SetPosX(x);
			else
				c->SetPosY(y);
			const float incrementSize = m_props.direction == DirectionOrientation::Horizontal ? c->GetSizeX() : c->GetSizeY();
			const bool	lastItem	  = idx == m_children.size() - 1;

			if (m_props.direction == DirectionOrientation::Horizontal)
				x += incrementSize + (lastItem ? 0.0f : GetChildPadding());
			else
				y += incrementSize + (lastItem ? 0.0f : GetChildPadding());

			idx++;
		}
	}

	void DirectionalLayout::Draw()
	{
		if (m_props.dropShadowBackground)
		{
			Color ds = Theme::GetDef().black;
			ds.w	 = 0.25f;
			WidgetUtility::DrawDropShadowRect(m_lvg, m_rect, m_drawOrder, ds, 6);
		}

		if (m_props.backgroundStyle == BackgroundStyle::Default)
		{
			LinaVG::StyleOptions bg;
			bg.color.start				= m_props.colorBackgroundStart.AsLVG4();
			bg.color.end				= m_props.colorBackgroundEnd.AsLVG4();
			bg.rounding					= m_props.rounding;
			bg.outlineOptions.thickness = m_props.outlineThickness;
			bg.outlineOptions.color		= m_props.colorOutline.AsLVG4();

			if (m_props.useHoverColor && m_isHovered)
				bg.color.start = bg.color.end = m_props.colorHovered.AsLVG4();

			for (int32 corner : m_props.onlyRoundTheseCorners)
				bg.onlyRoundTheseCorners.push_back(corner);

			m_lvg->DrawRect(m_rect.pos.AsLVG(), m_rect.GetEnd().AsLVG(), bg, 0.0f, m_drawOrder);
		}
		else if (m_props.backgroundStyle == BackgroundStyle::CentralGradient)
		{
			LinaVG::StyleOptions style;
			style.color.start = m_props.colorBackgroundStart.AsLVG4();
			style.color.end	  = m_props.colorBackgroundEnd.AsLVG4();

			LinaVG::StyleOptions style2;
			style2.color.start = m_props.colorBackgroundEnd.AsLVG4();
			style2.color.end   = m_props.colorBackgroundStart.AsLVG4();

			m_lvg->DrawRect(GetPos().AsLVG(), Vector2((m_rect.GetEnd().x + GetPos().x) * 0.5f, m_rect.GetEnd().y).AsLVG(), style, 0.0f, m_drawOrder);
			m_lvg->DrawRect(Vector2((m_rect.GetEnd().x + GetPos().x) * 0.5f, GetPos().y).AsLVG(), m_rect.GetEnd().AsLVG(), style2, 0.0f, m_drawOrder);
		}

		const Vector2 start = GetStartFromMargins();
		const Vector2 end	= GetEndFromMargins();

		if (m_props.clipChildren)
			m_manager->SetClip(Rect(start, end - start), {});
		Widget::Draw();
		if (m_props.clipChildren)
			m_manager->UnsetClip();
		Widget::DrawBorders();
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
		if (!m_props.receiveInput)
			return false;

		if (button == LINAGX_MOUSE_1 && m_isHovered)
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
				sz += c->GetSizeX() + GetChildPadding();
			else
				sz += c->GetSizeY() + GetChildPadding();
		}

		return sz - GetChildPadding();
	}
} // namespace Lina
