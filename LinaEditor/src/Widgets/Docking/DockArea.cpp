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

#include "Editor/Widgets/Docking/DockArea.hpp"
#include "Editor/Widgets/Docking/DockPreview.hpp"
#include "Editor/Widgets/Docking/DockContainer.hpp"
#include "Editor/Widgets/Docking/DockBorder.hpp"
#include "Common/Math/Math.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina::Editor
{
	void DockArea::Construct()
	{
	}

	void DockArea::Tick(float delta)
	{
		Widget::SetIsHovered();

		SetPos(m_parent->GetSize() * m_alignRect.pos);
		SetSize(m_parent->GetSize() * m_alignRect.size);

		if (m_preview)
		{
			m_preview->SetSize(m_rect.size);
			m_preview->SetPos(m_rect.pos);
			m_preview->Tick(delta);
		}
	}

	void DockArea::Draw(int32 threadIndex)
	{
		LinaVG::StyleOptions background;
		background.color = Theme::GetDef().background0.AsLVG4();
		LinaVG::DrawRect(threadIndex, m_rect.pos.AsLVG(), m_rect.GetEnd().AsLVG(), background, 0.0f, m_drawOrder);
		Widget::Draw(threadIndex);

		if (m_preview)
			m_preview->Draw(threadIndex);
	}

	void DockArea::ShowPreview()
	{
		LINA_ASSERT(m_preview == nullptr, "");
		m_preview						= Allocate<DockPreview>("DockContainerPreview");
		m_preview->GetProps().isCentral = true;
		m_preview->Initialize();

		const float someConst = 0.3f;

		const float horizontalSize = m_rect.size.x * DOCK_DEFAULT_PERCENTAGE;
		const float verticalSize   = m_rect.size.y * DOCK_DEFAULT_PERCENTAGE;
		if (horizontalSize < DOCKED_MIN_SIZE)
		{
			m_preview->DisableDirection(Direction::Left);
			m_preview->DisableDirection(Direction::Right);
		}

		if (verticalSize < DOCKED_MIN_SIZE)
		{
			m_preview->DisableDirection(Direction::Top);
			m_preview->DisableDirection(Direction::Bottom);
		}
	}

	void DockArea::HidePreview()
	{
		LINA_ASSERT(m_preview != nullptr, "");
		Deallocate(m_preview);
		m_preview = nullptr;
	}

	bool DockArea::OnMouse(uint32 button, LinaGX::InputAction action)
	{
		if (button != LINAGX_MOUSE_0)
			return false;

		if (m_isHovered && action == LinaGX::InputAction::Pressed)
		{
			ShowPreview();
			return true;
		}
		else
		{
			if (m_preview)
			{
				bool	  isPreviewHovered = false;
				Direction hoveredDirection = Direction::Center;
				m_preview->GetHoveredDirection(hoveredDirection, isPreviewHovered);
				if (isPreviewHovered)
					AddDockArea(hoveredDirection);
				HidePreview();
				return true;
			}
		}
	}

	DockArea* DockArea::AddDockArea(Direction direction)
	{
		if (direction == Direction::Center)
		{
			// Steal the contents of the dock area and return this.
			return this;
		}

		DockArea* area = Allocate<DockArea>("DockArea");

		// Setup areas percentages according to direction...
		if (direction == Direction::Left)
		{
			const float horizontalSize = m_rect.size.x * DOCK_DEFAULT_PERCENTAGE;
			const float parentPerc	   = horizontalSize / m_parent->GetRect().size.x;

			area->m_alignRect.pos  = m_alignRect.pos;
			area->m_alignRect.size = Vector2(parentPerc, m_alignRect.size.y);
			m_alignRect.pos.x += parentPerc;
			m_alignRect.size.x -= parentPerc;
		}
		else if (direction == Direction::Right)
		{
			const float horizontalSize = m_rect.size.x * DOCK_DEFAULT_PERCENTAGE;
			const float parentPerc	   = horizontalSize / m_parent->GetRect().size.x;
			area->m_alignRect.pos	   = Vector2(m_alignRect.pos.x + m_alignRect.size.x - parentPerc, m_alignRect.pos.y);
			area->m_alignRect.size	   = Vector2(parentPerc, m_alignRect.size.y);
			m_alignRect.size.x -= parentPerc;
		}
		else if (direction == Direction::Top)
		{
			const float verticalSize = m_rect.size.y * DOCK_DEFAULT_PERCENTAGE;
			const float parentPerc	 = verticalSize / m_parent->GetRect().size.y;
			area->m_alignRect.pos	 = m_alignRect.pos;
			area->m_alignRect.size	 = Vector2(m_alignRect.size.x, parentPerc);
			m_alignRect.pos.y += parentPerc;
			m_alignRect.size.y -= parentPerc;
		}
		else if (direction == Direction::Bottom)
		{
			const float verticalSize = m_rect.size.y * DOCK_DEFAULT_PERCENTAGE;
			const float parentPerc	 = verticalSize / m_parent->GetRect().size.y;
			area->m_alignRect.pos	 = Vector2(m_alignRect.pos.x, m_alignRect.pos.y + m_alignRect.size.y - parentPerc);
			area->m_alignRect.size	 = Vector2(m_alignRect.size.x, parentPerc);
			m_alignRect.size.y -= parentPerc;
		}

		m_parent->AddChild(area);
	}

} // namespace Lina::Editor
