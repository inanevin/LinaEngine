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

#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Common/System/SystemInfo.hpp"
#include "Common/Math/Math.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina
{
	void Text::Construct()
	{
		GetWidgetProps().material = ENGINE_MATERIAL_GUI_TEXT_ID;
	}

	void Text::Initialize()
	{
		CalculateTextSize();
	}

	void Text::CalculateSize(float delta)
	{
		if (m_props.fetchWrapFromParent)
			m_props.wrapWidth = m_parent->GetSizeX();

		// const bool calculate = m_calculatedFont != m_props.font || !Math::Equals(m_lastCalculatedWrapWidth, m_props.wrapWidth, 2.0f) || !Math::Equals(m_manager->GetScalingFactor(), m_calculatedUIScaling, 0.01f);
		// if (calculate)
		CalculateTextSize();
	}

	void Text::Draw()
	{
		// if (!Math::Equals(m_manager->GetScalingFactor(), m_calculatedUIScaling, 0.01f))
		// CalculateTextSize();

		if (m_props.fetchCustomClipFromParent && m_parent)
		{
			const Vector2 start = m_parent->GetRect().pos;
			const Vector2 end	= m_parent->GetRect().GetEnd();
			m_props.customClip	= Vector4(start.x, start.y, end.x - start.x, end.y - start.y + 10.0f);
		}

		if (m_props.fetchCustomClipFromSelf)
		{
			m_props.customClip = Vector4(GetPosX() - GetSizeX() * 0.1f, GetPosY() - GetSizeY() * 0.5f, GetSizeX() * 1.2f, GetSizeY() * 2.0f);
		}

		if (m_waitingOnClickedDelay)
		{
			if (SystemInfo::GetAppTimeF() - m_lastPressSeconds > 0.2)
			{
				if (m_props.onClicked)
					m_props.onClicked();
				m_waitingOnClickedDelay = false;
			}
		}

		if (m_props.valuePtr != nullptr)
			UpdateTextAndCalcSize(*m_props.valuePtr);

		if (m_lvgFont == nullptr)
			return;

		LinaVG::TextOptions opts;
		opts.font			= m_lvgFont;
		opts.textScale		= m_props.textScale;
		opts.alignment		= m_props.alignment;
		opts.wrapWidth		= m_props.wrapWidth;
		opts.newLineSpacing = 0.0f;
		opts.uniqueID		= m_widgetProps.material;

		if (GetFlags().IsSet(WF_DISABLED))
			opts.color = m_widgetProps.colorDisabled.AsLVG();
		else
			opts.color = m_props.color.AsLVG();

		opts.cpuClipping = m_props.customClip.AsLVG4();
		opts.wordWrap	 = m_props.wordWrap;

		auto p = (m_rect.pos + Vector2(0.0f, m_rect.size.y));

		m_lvg->DrawTextDefault(m_props.text.c_str(), p.AsLVG(), opts, 0.0f, m_drawOrder, m_props.isDynamic);
	}

	void Text::UpdateTextAndCalcSize(const String& txt)
	{
		m_props.text = txt;
		CalculateTextSize();
	}

	void Text::CalculateTextSize()
	{
		const float uiScaling	  = m_manager->GetScalingFactor();
		Font*		font		  = m_resourceManager->GetResource<Font>(m_props.font);
		m_lvgFont				  = font->GetFont(uiScaling);
		m_calculatedUIScaling	  = uiScaling;
		m_lastCalculatedWrapWidth = m_props.wrapWidth;
		m_calculatedFont		  = m_props.font;

		if (m_lvgFont == nullptr)
			return;

		m_isSDF = m_lvgFont->isSDF;

		LinaVG::TextOptions opts;
		opts.font	   = m_lvgFont;
		opts.textScale = m_props.textScale;
		opts.wrapWidth = m_props.wrapWidth;
		opts.wordWrap  = m_props.wordWrap;
		m_rect.size	   = m_lvg->CalculateTextSize(m_props.text.c_str(), opts);
	}

	bool Text::OnMouse(uint32 button, LinaGX::InputAction act)
	{
		if (button != LINAGX_MOUSE_0)
			return false;

		if (m_isHovered)
			m_waitingOnClickedDelay = false;

		if (act == LinaGX::InputAction::Pressed && m_isHovered)
		{
			m_isPressed = true;
			return true;
		}

		if (act == LinaGX::InputAction::Released)
		{
			if (m_isPressed && m_isHovered)
			{
				if (m_props.delayOnClicked)
				{
					m_lastPressSeconds		= SystemInfo::GetAppTimeF();
					m_waitingOnClickedDelay = true;
				}
				else
				{
					if (m_props.onClicked)
						m_props.onClicked();
				}

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

	void Text::CollectResourceReferences(HashSet<ResourceID>& out)
	{
		Widget::CollectResourceReferences(out);
		out.insert(m_props.font);
	}
} // namespace Lina
