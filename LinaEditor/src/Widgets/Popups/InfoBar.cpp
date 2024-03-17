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

#include "Editor/Widgets/Popups/InfoBar.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Common/Math/Math.hpp"
#include "Common/Tween/TweenManager.hpp"
#include "Common/System/System.hpp"
#include "Editor/Editor.hpp"

namespace Lina::Editor
{
	void InfoBar::Construct()
	{
		m_text = Allocate<Text>();
		m_text->GetFlags().Set(WF_POS_ALIGN_Y);
		m_text->SetPosAlignmentSourceY(PosAlignmentSource::Center);
		m_text->SetAlignedPosY(0.5f);
		AddChild(m_text);

		TweenManager::Get()->AddTween(&m_tweenValue, 0.0f, 1.0f, TWEEN_TIME, TweenType::EaseOut);
	}

	void InfoBar::CalculateSize(float delta)
	{
		const float minX	= m_lgxWindow->GetSize().x * 0.15f;
		const float targetX = Math::Clamp(m_text->GetSizeX() + Theme::GetDef().baseIndent * 2.0f, minX, static_cast<float>(m_lgxWindow->GetSize().x));
		SetSizeX(targetX * m_tweenValue);
		SetSizeY(m_text->GetSizeY() + Theme::GetDef().baseIndent * 2.0f);
	}

	void InfoBar::Tick(float delta)
	{
		const float y = (m_lgxWindow->GetSize().y - m_lgxWindow->GetSize().y * 0.15f);
		SetPos(Vector2(0.0f, y));
		m_text->SetPosX(m_rect.pos.x + Theme::GetDef().baseIndent);

		m_counter += delta;

		if (!Math::Equals(m_maxTime, 0.0f, 0.0001f))
		{
			if (m_counter > m_maxTime && !m_destroying)
			{
				m_destroying = true;
				TweenManager::Get()->AddTween(&m_tweenValue, 1.0f, 0.0f, TWEEN_TIME, TweenType::EaseIn)->SetOnCompleted([this]() { m_system->CastSubsystem<Editor>(SubsystemType::Editor)->DestroyInfoBar(); });
			}
		}
	}

	void InfoBar::Draw(int32 threadIndex)
	{
		LinaVG::StyleOptions options;
		options.color	 = Theme::GetDef().background2.AsLVG4();
		options.rounding = Theme::GetDef().baseRounding;
		options.onlyRoundTheseCorners.push_back(1);
		options.onlyRoundTheseCorners.push_back(2);
		LinaVG::DrawRect(threadIndex, m_rect.pos.AsLVG(), m_rect.GetEnd().AsLVG(), options, 0.0f, m_drawOrder);

		m_manager->SetClip(threadIndex, m_rect, {});
		m_text->Draw(threadIndex);
		m_manager->UnsetClip(threadIndex);
	}

	void InfoBar::AddInfo(const String& str, LogLevel level, float time)
	{
		m_destroying = false;
		m_counter	 = 0.0f;
		m_maxTime	 = time;

		m_text->GetProps().text = str;
		m_text->CalculateTextSize();
	}

	void InfoBar::RemoveInfo()
	{
		m_destroying = true;
		TweenManager::Get()->AddTween(&m_tweenValue, 1.0f, 0.0f, TWEEN_TIME, TweenType::EaseIn)->SetOnCompleted([this]() { m_system->CastSubsystem<Editor>(SubsystemType::Editor)->DestroyInfoBar(); });
	}
} // namespace Lina::Editor
