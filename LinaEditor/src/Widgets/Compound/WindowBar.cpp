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

#include "Editor/Widgets/Compound/WindowBar.hpp"
#include "Editor/CommonEditor.hpp"
#include "Editor/Widgets/CommonWidgets.hpp"
#include "Core/GUI/Widgets/Primitives/Icon.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Common/Math/Math.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina::Editor
{
	void WindowBar::Construct()
	{
		GetProps().direction				  = DirectionOrientation::Horizontal;
		GetWidgetProps().outlineThickness	  = 0.0f;
		GetWidgetProps().rounding			  = 0.0f;
		GetWidgetProps().drawBackground		  = true;
		GetWidgetProps().dropshadow.enabled	  = true;
		GetWidgetProps().dropshadow.direction = Direction::Bottom;
		GetWidgetProps().dropshadow.steps	  = 4;
		GetWidgetProps().childPadding		  = Theme::GetDef().baseIndent;
		GetWidgetProps().childMargins		  = {.left = Theme::GetDef().baseIndent};
	}

	void WindowBar::Initialize()
	{
		if (!m_barProps.icon.empty())
		{
			Icon* icon			  = m_manager->Allocate<Icon>("WindowBarIcon");
			icon->GetProps().icon = m_barProps.icon;
			icon->GetFlags().Set(WF_POS_ALIGN_Y);
			icon->SetAlignedPosY(0.5f);
			icon->SetAnchorY(Anchor::Center);
			icon->GetProps().dynamicSizeScale	 = 0.7f;
			icon->GetProps().dynamicSizeToParent = true;
			AddChild(icon);
		}

		Text* text			  = m_manager->Allocate<Text>("WindowBarTitle");
		text->GetProps().text = m_barProps.title;
		text->GetFlags().Set(WF_POS_ALIGN_Y);
		text->SetAlignedPosY(0.5f);
		text->SetAnchorY(Anchor::Center);
		AddChild(text);

		if (m_barProps.hasWindowButtons)
		{
			DirectionalLayout* wb = CommonWidgets::BuildWindowButtons(this);
			wb->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y | WF_USE_FIXED_SIZE_X);
			wb->SetAlignedSizeY(1.0f);
			wb->SetAlignedPos(Vector2(1.0f, 0.0f));
			wb->SetAnchorX(Anchor::End);
			wb->SetFixedSizeX(Theme::GetDef().baseItemHeight * 6.0f);
			m_windowButtons = wb;
			AddChild(wb);
		}
	}

	void WindowBar::PreTick()
	{
		DirectionalLayout::PreTick();

		if (m_barProps.controlsDragRect)
		{
			m_dragRect = Rect(m_rect.pos / m_lgxWindow->GetDPIScale(), m_rect.size / m_lgxWindow->GetDPIScale());

			if (m_windowButtons != nullptr)
				m_dragRect.size.x -= m_windowButtons->GetSizeX() / m_lgxWindow->GetDPIScale();

			LinaGX::LGXRectui lgxRect;
			lgxRect.pos	 = {static_cast<uint32>(m_dragRect.pos.x), static_cast<uint32>(m_dragRect.pos.y)};
			lgxRect.size = {static_cast<uint32>(m_dragRect.size.x), static_cast<uint32>(m_dragRect.size.y)};
			m_lgxWindow->SetDragRect(lgxRect);
		}
	}

	void WindowBar::Tick(float delta)
	{
		DirectionalLayout::Tick(delta);

		const Color targetColor				   = m_lgxWindow->HasFocus() ? Theme::GetDef().accentPrimary0 : Theme::GetDef().background2;
		GetWidgetProps().colorBackground.start = Math::Lerp(GetWidgetProps().colorBackground.start, targetColor, delta * COLOR_SPEED);
		GetWidgetProps().colorBackground.end   = GetWidgetProps().colorBackground.start;
	}

	bool WindowBar::OnMouse(uint32 button, LinaGX::InputAction act)
	{
		if (button == LINAGX_MOUSE_0 && act == LinaGX::InputAction::Repeated && m_dragRect.IsPointInside(m_manager->GetMousePosition()))
		{
			if (m_lgxWindow->GetIsMaximized())
				m_lgxWindow->Restore();
			else
				m_lgxWindow->Maximize();
			return true;
		}

		return false;
	}

	void WindowBar::SetCloseDisabled(bool isDisabled)
	{
		m_windowButtons->GetChildren().at(2)->GetFlags().Set(WF_DISABLED, isDisabled);
	}

	void WindowBar::SetMinimizeDisabled(bool isDisabled)
	{
		m_windowButtons->GetChildren().at(0)->GetFlags().Set(WF_DISABLED, isDisabled);
	}

	void WindowBar::SetMaximizeDisabled(bool isDisabled)
	{
		m_windowButtons->GetChildren().at(1)->GetFlags().Set(WF_DISABLED, isDisabled);
	}
} // namespace Lina::Editor
