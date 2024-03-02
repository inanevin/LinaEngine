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
#include "Common/Tween/TweenManager.hpp"
#include "Common/Math/Math.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina::Editor
{

	void Tab::Construct()
	{
		m_text = Allocate<Text>();
		m_icon = Allocate<Icon>();

		AddChild(m_text);
		AddChild(m_icon);

		m_closeRectAnim = TweenManager::Get()->AddTween(&m_closeRectAnimValue, 0.0f, 1.0f, CLOSEBG_ANIM_TIME, TweenType::EaseIn);
		m_closeRectAnim->SetPersistent(true);
		m_closeRectAnim->SetTimeScale(0.0f);

		m_selectionRectAnim = TweenManager::Get()->AddTween(&m_selectionRectAnimValue, 0.0f, 1.0f, 0.5f, TweenType::EaseIn);
		m_selectionRectAnim->SetPersistent(true);
		m_selectionRectAnim->SetTimeScale(0.0f);
	}

	void Tab::Initialize()
	{
		m_text->GetProps().text		 = m_props.tiedWidget->GetDisplayName();
		m_icon->GetProps().icon		 = ICON_XMARK;
		m_icon->GetProps().textScale = 0.4f;
		m_text->Initialize();
		m_icon->Initialize();
	}

	void Tab::Destruct()
	{
		m_closeRectAnim->Kill();
		m_selectionRectAnim->Kill();
	}

	void Tab::PreTick()
	{
		const float indent = Theme::GetDef().baseIndentInner;

		if (m_props.disableClose)
			SetSizeX(indent + SELECTION_RECT_WIDTH + indent + m_text->GetSizeX() + indent);
		else
			SetSizeX(indent + SELECTION_RECT_WIDTH + indent + m_text->GetSizeX() + indent + m_icon->GetSizeX() + indent);

		SetSizeY(m_parent->GetSizeY());
		SetPosY(m_parent->GetPosY());
	}

	void Tab::Tick(float delta)
	{
		Widget::SetIsHovered();

		const Vector2& mp		 = m_lgxWindow->GetMousePosition();
		const Rect	   closeRect = Rect(Vector2(m_rect.pos.x + m_rect.size.x * 0.75f, m_rect.pos.y), Vector2(m_rect.size.x * 0.2f, m_rect.size.y));

		const bool wasCloseRectHovered = m_closeRectHovered;
		m_closeRectHovered			   = closeRect.IsPointInside(mp);

		if (!wasCloseRectHovered && m_closeRectHovered)
		{
			m_closeRectAnim->SetStart(0.0f);
			m_closeRectAnim->SetEnd(1.0f);
			m_closeRectAnim->SetTime(0.0f);
			m_closeRectAnim->SetTimeScale(1.0f);
		}

		if (!m_closeRectHovered && wasCloseRectHovered)
		{
			m_closeRectAnim->SetStart(1.0f);
			m_closeRectAnim->SetEnd(0.0f);
			m_closeRectAnim->SetTime(0.0f);
			m_closeRectAnim->SetTimeScale(1.0f);
		}

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

		const float indent = Theme::GetDef().baseIndentInner;

		// Calculate selection rect
		const Vector2 selectionRectStart = Vector2(m_rect.pos.x + indent, m_rect.pos.y + indent * 0.5f);
		const Vector2 selectionRectEnd	 = Vector2(m_selectionRect.pos.x + SELECTION_RECT_WIDTH, m_rect.GetEnd().y - indent * 0.5f);
		m_selectionRect.pos				 = Vector2(selectionRectStart.x, Math::Remap(m_selectionRectAnimValue, 0.0f, 1.0f, selectionRectEnd.y, selectionRectStart.y));
		m_selectionRect.size			 = selectionRectEnd - m_selectionRect.pos;

		// Calculate close rect
		m_closeRect.pos	 = Vector2(Math::Remap(m_closeRectAnimValue, 0.0f, 1.0f, m_rect.GetEnd().x, m_rect.pos.x + m_rect.size.x * 0.9f), m_rect.pos.y);
		m_closeRect.size = Vector2(Math::Remap(m_closeRectAnimValue, 0.0f, 1.0f, 0.0f, m_rect.size.x * 0.1f), m_rect.size.y);

		m_text->SetPos(Vector2(m_selectionRect.GetEnd().x + indent, m_rect.GetCenter().y - m_text->GetHalfSizeY()));
		m_text->Tick(delta);

		m_icon->GetProps().color = Math::Lerp(Theme::GetDef().foreground1, Theme::GetDef().foreground0, m_closeRectAnimValue).AsLVG4();
		m_icon->SetPos(Vector2(m_rect.pos.x + m_rect.size.x * 0.9f - m_icon->GetHalfSizeX(), m_rect.GetCenter().y - m_icon->GetHalfSizeY()));
		m_icon->Tick(delta);
	}

	void Tab::Draw(int32 threadIndex)
	{
		// Draw background.
		LinaVG::StyleOptions background;
		if (m_props.isSelected)
		{
			background.color.start = Theme::GetDef().background3.AsLVG4();
			background.color.end   = Theme::GetDef().background4.AsLVG4();
		}
		else
			background.color = Theme::GetDef().background2.AsLVG4();

		LinaVG::DrawRect(threadIndex, m_rect.pos.AsLVG(), m_rect.GetEnd().AsLVG(), background, 0.0f, m_drawOrder);

		// Draw selection indicator rect.
		LinaVG::StyleOptions selectionRect;

		if (m_props.isSelected)
		{
			selectionRect.color.start		 = Theme::GetDef().accentPrimary0.AsLVG4();
			selectionRect.color.end			 = Theme::GetDef().accentPrimary1.AsLVG4();
			selectionRect.color.gradientType = LinaVG::GradientType::Vertical;
		}
		else
			selectionRect.color = Theme::GetDef().silent.AsLVG4();

		LinaVG::DrawRect(threadIndex, m_selectionRect.pos.AsLVG(), m_selectionRect.GetEnd().AsLVG(), selectionRect, 0.0f, m_drawOrder);

		if (!m_props.disableClose)
		{
			// Draw close bg.
			LinaVG::StyleOptions closeBg;

			if (m_closeRectPressed)
				closeBg.color = Theme::GetDef().accentPrimary1.AsLVG4();
			else
				closeBg.color = m_closeRectHovered ? Theme::GetDef().accentPrimary0.AsLVG4() : Theme::GetDef().background5.AsLVG4();

			const float closeBgX = Math::Remap(m_closeRectAnimValue, 0.0f, 1.0f, m_rect.GetEnd().x, m_rect.pos.x + m_rect.size.x * 0.8f);
			LinaVG::DrawRect(threadIndex, m_closeRect.pos.AsLVG(), m_closeRect.GetEnd().AsLVG(), closeBg, 0.0f, m_drawOrder);

			m_icon->Draw(threadIndex);
		}

		m_text->Draw(threadIndex);
	}

	bool Tab::OnMouse(uint32 button, LinaGX::InputAction action)
	{
		if (button != LINAGX_MOUSE_0)
			return false;

		if (!m_props.disableClose && m_closeRectHovered && action == LinaGX::InputAction::Pressed)
		{
			m_closeRectPressed = true;
			return true;
		}

		if (!m_props.disableClose && m_closeRectPressed && action == LinaGX::InputAction::Released)
		{
			m_closeRectPressed = false;

			if (m_closeRectHovered)
				m_ownerRow->Close(m_props.tiedWidget);

			return true;
		}

		if (m_isHovered && action == LinaGX::InputAction::Pressed)
		{
			m_ownerRow->SetSelected(m_props.tiedWidget);
			return true;
		}

		return false;
	}
} // namespace Lina::Editor
