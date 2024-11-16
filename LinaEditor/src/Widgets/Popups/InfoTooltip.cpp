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

#include "Editor/Widgets/Popups/InfoTooltip.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/Primitives/Icon.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Common/Math/Math.hpp"

#include "Editor/Editor.hpp"
#include "Editor/CommonEditor.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina::Editor
{
	void InfoTooltip::Construct()
	{
		GetProps().direction			  = DirectionOrientation::Horizontal;
		GetWidgetProps().drawBackground	  = true;
		GetWidgetProps().colorBackground  = Theme::GetDef().background0;
		GetWidgetProps().rounding		  = 0;
		GetWidgetProps().outlineThickness = Theme::GetDef().baseOutlineThickness;
		GetFlags().Set(WF_SIZE_X_TOTAL_CHILDREN | WF_SIZE_Y_MAX_CHILDREN | WF_SIZE_AFTER_CHILDREN);
		SetAlignedSizeX(1.0f);
		SetAlignedSizeY(1.0f);
		GetWidgetProps().childPadding = Theme::GetDef().baseIndent;
		GetWidgetProps().childMargins = {.top = Theme::GetDef().baseIndent * 0.5f, .bottom = Theme::GetDef().baseIndent * 0.5f, .left = Theme::GetDef().baseIndent, .right = Theme::GetDef().baseIndent};
	}

	void InfoTooltip::Initialize()
	{
		// Icon
		m_icon = m_manager->Allocate<Icon>("InfoIcon");
		m_icon->GetFlags().Set(WF_POS_ALIGN_Y);
		m_icon->SetAnchorY(Anchor::Center);
		m_icon->SetAlignedPosY(0.5f);
		m_icon->GetProps().textScale = 0.6f;

		if (m_tooltipProps.level == LogLevel::LOG_LEVEL_ERROR)
		{
			m_icon->GetProps().icon	 = ICON_XMARK_CIRCLE;
			m_icon->GetProps().color = Theme::GetDef().accentError;
		}
		else if (m_tooltipProps.level == LogLevel::LOG_LEVEL_WARNING)
		{
			m_icon->GetProps().icon	 = ICON_EXCLAMATION_TRIANGLE;
			m_icon->GetProps().color = Theme::GetDef().accentWarn;
		}
		else
		{
			m_icon->GetProps().icon	 = ICON_INFO_CIRCLE;
			m_icon->GetProps().color = Theme::GetDef().foreground0;
		}
		AddChild(m_icon);

		// Text
		m_text = m_manager->Allocate<Text>("InfoText");
		m_text->GetFlags().Set(WF_POS_ALIGN_Y);
		m_text->SetAnchorY(Anchor::Center);
		m_text->SetAlignedPosY(0.5f);
		m_text->GetProps().text	 = m_tooltipProps.text;
		m_text->GetProps().color = Theme::GetDef().foreground0;
		AddChild(m_text);

		m_tween = Tween(0.0f, 1.0f, TWEEN_TIME, TweenType::EaseOut);
		DirectionalLayout::Initialize();
	}

	void InfoTooltip::PreTick()
	{
		if (m_lgxWindow->GetInput()->GetMouseButtonDown(LINAGX_MOUSE_0))
		{
			m_manager->AddToKillList(this);
			return;
		}
	}

	void InfoTooltip::Tick(float delta)
	{

		if (m_tooltipProps.direction != Direction::Center)
			m_tween.Tick(delta);

		if (m_tooltipProps.direction == Direction::Center)
		{
			DirectionalLayout::Tick(delta);
			return;
		}

		if (m_firstTick && !m_initializing)
		{
			m_firstTick		= false;
			m_startPosition = GetPos();
		}

		const Vector2 desiredEnd = m_startPosition + DirectionToVector(m_tooltipProps.direction) * 10.0f;
		SetPos(Math::Lerp(m_startPosition, desiredEnd, m_tween.GetValue()));

		DirectionalLayout::Tick(delta);

		m_counter += delta;
		if (m_counter > m_tooltipProps.time)
			m_manager->AddToKillList(this);
	}

	void InfoTooltip::Draw()
	{
		GetWidgetProps().colorOutline = GetColorFromLevel();
		DirectionalLayout::Draw();
	}

	Color InfoTooltip::GetColorFromLevel()
	{
		if (m_tooltipProps.level == LogLevel::LOG_LEVEL_ERROR)
			return Theme::GetDef().accentError;

		if (m_tooltipProps.level == LogLevel::LOG_LEVEL_WARNING)
			return Theme::GetDef().accentWarn;

		return Theme::GetDef().outlineColorBase;
	}
} // namespace Lina::Editor
