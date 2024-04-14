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

#include "Core/GUI/Widgets/Compound/FoldingSelectable.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/Primitives/Icon.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/Math/Math.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina
{

	void FoldingSelectable::Construct()
	{
		DirectionalLayout* layout = m_manager->Allocate<DirectionalLayout>("Layout");
		layout->GetFlags().Set(WF_POS_ALIGN_Y);
		layout->SetAlignedPosY(0.0f);
		AddChild(layout);
		m_layout = layout;
	}

	void FoldingSelectable::Initialize()
	{
		ChangeFold(m_folded);
		Widget::Initialize();
	}

	void FoldingSelectable::CalculateSize(float dt)
	{
		float childrenTotalHeight = 0.0f;
		for (auto* c : m_children)
		{
			if (c == m_layout)
				continue;

			c->CalculateSize(dt);
			childrenTotalHeight += c->GetSizeY();
		}

		float targetY = 0.0f;

		if (m_folded)
			targetY = m_props.height;
		else
			targetY = m_props.height + childrenTotalHeight;

		SetSizeY(Math::Lerp(GetSizeY(), targetY, SIZE_SPEED * dt));
	}

	void FoldingSelectable::PreTick()
	{
		// Requires main thread due to possible allocation/deallocation below
		const bool hasControls = m_manager->GetControlsOwner() == this;

		if (hasControls && !m_selected)
			ChangeSelected(true);

		// if (!hasControls && m_selected && FindGetControlsManager()->IsWidgetInHierarchy(GetControlsOwner()))
		// 	ChangeSelected(false);
	}

	void FoldingSelectable::Tick(float dt)
	{
		if (GetIsDisabled())
			return;

		SetIsHovered();

		const bool hasControls = m_manager->GetControlsOwner() == this;

		if (!m_selected)
		{
			m_usedColorStart = Math::Lerp(m_usedColorStart, Color(0.0f, 0.0f, 0.0f, 0.0f), dt * COLOR_SPEED);
			m_usedColorEnd	 = Math::Lerp(m_usedColorEnd, Color(0.0f, 0.0f, 0.0f, 0.0f), dt * COLOR_SPEED);
		}
		else
		{
			m_usedColorStart = Math::Lerp(m_usedColorStart, hasControls ? m_props.colorSelectedStart : m_props.colorSelectedInactiveStart, dt * COLOR_SPEED);
			m_usedColorEnd	 = Math::Lerp(m_usedColorEnd, hasControls ? m_props.colorSelectedEnd : m_props.colorSelectedInactiveEnd, dt * COLOR_SPEED);
		}

		m_layout->SetPosX(GetPosX());
		m_layout->SetSizeX(GetPosX() + GetSizeX() - m_layout->GetPosX());
		m_layout->SetSizeY(m_props.height);

		float childY = GetPosY() + m_props.height;
		for (auto* c : m_children)
		{
			if (c == m_layout)
				continue;

			c->SetPosX(GetPosX());
			c->SetPosY(childY);
			// c->GetChildMargins().left = GetChildMargins().left + GetChildPadding();
			c->Tick(dt);
			childY += c->GetSizeY();
		}
	}

	void FoldingSelectable::Draw(int32 threadIndex)
	{
		if (!GetIsVisible())
			return;

		const bool hasControls = m_manager->GetControlsOwner() == this;

		LinaVG::StyleOptions opts;
		opts.color.start = m_usedColorStart.AsLVG4();
		opts.color.end	 = m_usedColorEnd.AsLVG4();

		LinaVG::DrawRect(threadIndex, m_rect.pos.AsLVG(), (m_rect.pos + Vector2(m_rect.size.x, m_props.height)).AsLVG(), opts, 0.0f, m_drawOrder);

		m_layout->Draw(threadIndex);

		for (auto* c : m_children)
		{
			if (c == m_layout)
				continue;

			if (!m_folded)
				c->Draw(threadIndex);
		}
	}

	bool FoldingSelectable::OnMouse(uint32 button, LinaGX::InputAction act)
	{
		if (button != LINAGX_MOUSE_0)
			return false;

		if (m_isHovered && act == LinaGX::InputAction::Pressed)
		{
			m_manager->GrabControls(this);
			ChangeSelected(true);
			return true;
		}

		if (m_isHovered && act == LinaGX::InputAction::Repeated)
		{
			ChangeFold(!m_folded);
			return true;
		}

		return false;
	}

	bool FoldingSelectable::OnKey(uint32 keycode, int32 scancode, LinaGX::InputAction act)
	{
		if (act == LinaGX::InputAction::Released)
			return false;

		if (m_manager->GetControlsOwner() != this)
			return false;

		// if (keycode == LINAGX_KEY_RETURN)
		//{
		//	ChangeFold(!m_folded);
		//	return true;
		// }

		// if (keycode == LINAGX_KEY_LEFT)
		//{
		//	ChangeFold(true);
		//	return true;
		// }

		// if (keycode == LINAGX_KEY_RIGHT)
		//{
		//	ChangeFold(false);
		//	return true;
		// }

		// if (keycode == LINAGX_KEY_DOWN)
		//{
		//	if (!m_children.empty() || m_props.owner->GetChildren().back() != this)
		//		FindGetControlsManager()->MoveControlsToNext();
		//	return true;
		// }

		// if (keycode == LINAGX_KEY_UP)
		//{
		//	if (m_props.owner->GetChildren().front() != this)
		//		FindGetControlsManager()->MoveControlsToPrev();
		//	return true;
		// }

		return false;
	}

	void FoldingSelectable::ChangeFold(bool folded)
	{
		const bool actuallyChanged = m_folded != folded;

		m_folded = folded;

		for (auto* c : m_children)
		{
			if (c == m_layout)
				continue;

			c->SetIsDisabled(m_folded);
		}

		if (actuallyChanged && m_props.onFoldChanged)
			m_props.onFoldChanged(m_folded);
	}

	void FoldingSelectable::ChangeSelected(bool selected)
	{
		if (selected == m_selected)
			return;

		m_selected = selected;

		if (m_props.onSelectedChanged)
			m_props.onSelectedChanged(m_selected);
	}
} // namespace Lina
