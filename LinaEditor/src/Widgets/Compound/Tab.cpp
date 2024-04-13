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

#include "Editor/Widgets/Compound/Tab.hpp"
#include "Editor/Widgets/Compound/TabRow.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/Primitives/Icon.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Common/Tween/TweenManager.hpp"
#include "Common/Math/Math.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina::Editor
{
	void Tab::Construct()
	{
		m_selectionRectAnim = TweenManager::Get()->AddTween(&m_selectionRectAnimValue, 0.0f, 1.0f, SELECTION_ANIM_TIME, TweenType::EaseIn);
		m_selectionRectAnim->SetPersistent(true);
		m_selectionRectAnim->SetTimeScale(0.0f);

		DirectionalLayout* layout = m_manager->Allocate<DirectionalLayout>("Layout");
		layout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		layout->SetAlignedSize(Vector2(1.0f, 1.0f));
		layout->SetAlignedPos(Vector2::Zero);
		layout->GetChildMargins() = {.left = Theme::GetDef().baseIndent * 2.0f, .right = Theme::GetDef().baseIndent};
		layout->SetChildPadding(Theme::GetDef().baseIndent);
		AddChild(layout);

		m_text = m_manager->Allocate<Text>("Title");
		m_text->GetFlags().Set(WF_POS_ALIGN_Y | WF_CONTROLS_DRAW_ORDER);
		m_text->SetAlignedPosY(0.5f);
		m_text->SetPosAlignmentSourceY(PosAlignmentSource::Center);
		m_text->GetProps().isDynamic = true;
		layout->AddChild(m_text);

		m_icon = m_manager->Allocate<Icon>("Icon");
		m_icon->GetFlags().Set(WF_POS_ALIGN_Y | WF_CONTROLS_DRAW_ORDER);
		m_icon->SetAlignedPosY(0.5f);
		m_icon->SetPosAlignmentSourceY(PosAlignmentSource::Center);
		m_icon->GetProps().icon					  = ICON_XMARK;
		m_icon->GetProps().enableHoverPressColors = true;
		m_icon->GetProps().textScale			  = 0.5f;
		m_icon->GetProps().colorStart.w			  = 0.5f;
		m_icon->GetProps().colorEnd.w			  = 0.5f;
		m_icon->GetProps().colorHovered			  = Theme::GetDef().foreground0;
		m_icon->GetProps().colorPressed			  = Theme::GetDef().foreground0;
		m_icon->GetProps().colorPressed.w		  = 0.25f;
		m_icon->GetProps().colorDisabled		  = Color(0.0f, 0.0f, 0.0f, 0.0f);
		m_icon->GetProps().onClicked			  = [this]() { m_ownerRow->Close(m_props.tiedWidget); };
		layout->AddChild(m_icon);
	}

	void Tab::Initialize()
	{
		m_text->GetProps().text = m_props.tiedWidget->GetDebugName();
		Widget::Initialize();
	}

	void Tab::Destruct()
	{
		m_selectionRectAnim->Kill();
	}

	void Tab::PreTick()
	{
		if (m_isPressed && !m_icon->GetIsDisabled())
		{
			const Vector2& mp	  = m_lgxWindow->GetMousePosition();
			const float	   margin = Theme::GetDef().baseIndent * 4.0f;
			if (mp.y < m_rect.pos.y - margin || mp.y > m_rect.GetEnd().y + margin)
				m_ownerRow->DockOut(m_props.tiedWidget);
		}
	}

	void Tab::CalculateSize(float delta)
	{
		const float indent	 = Theme::GetDef().baseIndent;
		const float iconSize = m_icon->GetIsDisabled() ? 0.0f : m_icon->GetSizeX();
		SetSizeX(SELECTION_RECT_WIDTH + m_text->GetSizeX() + iconSize + indent * 4.0f);
	}

	void Tab::Tick(float delta)
	{
		// Hover
		if (m_ownerRow->GetAnyPressed() && !m_isPressed)
			m_isHovered = false;

		// Press movement
		const Vector2& mp = m_lgxWindow->GetMousePosition();
		if (m_isPressed)
		{
			float desiredX = mp.x - m_offsetAtPress.x;
			desiredX	   = Math::Clamp(desiredX, m_ownerRow->GetPosX(), m_ownerRow->GetRect().GetEnd().x - GetSizeX());
			m_rect.pos.x   = desiredX;
		}
		else
			m_rect.pos.x = Math::Lerp(GetPosX(), m_props.desiredX, delta * INTERP_SPEED);

		m_alpha = Math::Clamp(Math::Remap(m_rect.pos.x, m_ownerRow->GetRect().GetEnd().x - GetSizeX() * 2.5f, m_ownerRow->GetRect().GetEnd().x, 1.0f, 0.1f), 0.1f, 1.0f);

		if (!m_wasSelected && m_props.isSelected)
		{
			m_selectionRectAnim->SetStart(0.0f);
			m_selectionRectAnim->SetEnd(1.0f);
			m_selectionRectAnim->SetTime(0.0f);
			m_selectionRectAnim->SetTimeScale(1.0f);
		}
		else if (m_wasSelected && !m_props.isSelected)
		{
			m_selectionRectAnim->SetStart(1.0f);
			m_selectionRectAnim->SetEnd(0.0f);
			m_selectionRectAnim->SetTime(0.0f);
			m_selectionRectAnim->SetTimeScale(1.0f);
		}

		m_wasSelected = m_props.isSelected;

		// Selection rect calculation
		const float	  indent			 = Theme::GetDef().baseIndentInner;
		const Vector2 selectionRectStart = Vector2(m_rect.pos.x + indent, m_rect.pos.y + indent * 0.5f);
		const Vector2 selectionRectEnd	 = Vector2(m_selectionRect.pos.x + SELECTION_RECT_WIDTH, m_rect.GetEnd().y - indent * 0.5f);
		m_selectionRect.pos				 = Vector2(selectionRectStart.x, Math::Remap(m_selectionRectAnimValue, 0.0f, 1.0f, selectionRectEnd.y, selectionRectStart.y));
		m_selectionRect.size.x			 = 2.0f;
		m_selectionRect.size.y			 = selectionRectEnd.y - m_selectionRect.pos.y;
	}

	void Tab::Draw(int32 threadIndex)
	{
		const int32 drawOrder = m_isPressed ? m_drawOrder + 1 : m_drawOrder;

		// Draw background.
		LinaVG::StyleOptions background;
		if (m_props.isSelected)
		{
			// background.color.start = Theme::GetDef().background3.AsLVG4();
			// background.color.end   = Theme::GetDef().background4.AsLVG4();
			background.color = Theme::GetDef().background2.AsLVG4();
		}
		else
			background.color = m_isHovered ? Theme::GetDef().background2.AsLVG4() : Theme::GetDef().background0.AsLVG4();

		background.color.start.w = background.color.end.w = m_alpha;

		LinaVG::DrawRect(threadIndex, m_rect.pos.AsLVG(), m_rect.GetEnd().AsLVG(), background, 0.0f, drawOrder);

		// Draw selection indicator rect.
		LinaVG::StyleOptions selectionRect;

		if (m_props.isSelected)
		{
			selectionRect.color.start		 = Theme::GetDef().accentPrimary0.AsLVG4();
			selectionRect.color.end			 = Theme::GetDef().accentPrimary1.AsLVG4();
			selectionRect.color.gradientType = LinaVG::GradientType::Vertical;
		}
		else
			selectionRect.color = Theme::GetDef().silent0.AsLVG4();

		selectionRect.color.start.w = selectionRect.color.end.w = m_alpha;
		LinaVG::DrawRect(threadIndex, m_selectionRect.pos.AsLVG(), m_selectionRect.GetEnd().AsLVG(), selectionRect, 0.0f, drawOrder);

		m_icon->GetProps().colorEnd.w = m_icon->GetProps().colorStart.w = m_text->GetProps().color.w = m_alpha;

		m_icon->SetDrawOrder(drawOrder);
		m_text->SetDrawOrder(drawOrder);
		m_icon->Draw(threadIndex);
		m_text->Draw(threadIndex);
	}

	bool Tab::OnMouse(uint32 button, LinaGX::InputAction action)
	{
		if (button != LINAGX_MOUSE_0)
			return false;

		if (m_isPressed && action == LinaGX::InputAction::Released)
		{
			m_isPressed = false;
			return true;
		}

		if (m_isHovered && action == LinaGX::InputAction::Pressed && !m_icon->GetIsHovered())
		{
			if (!m_props.isSelected)
				m_ownerRow->SelectionChanged(m_props.tiedWidget);

			if (!m_props.disableMovement)
			{
				m_isPressed		= true;
				m_offsetAtPress = m_lgxWindow->GetMousePosition() - m_rect.pos;
			}

			return true;
		}

		return false;
	}

	void Tab::DisableClosing(bool disabled)
	{
		m_icon->SetIsDisabled(disabled);
	}
} // namespace Lina::Editor
