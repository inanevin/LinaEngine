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

#include "Core/GUI/Widgets/Primitives/Icon.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Common/Math/Math.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina
{
	void Icon::Construct()
	{
		GetWidgetProps().material = ENGINE_MATERIAL_GUI_SDFTEXT_ID;
	}

	void Icon::Initialize()
	{
		CalculateIconSize();
	}

	void Icon::CalculateSize(float dt)
	{
		const float dpiScale = m_manager->GetScalingFactor();

		if (!Math::Equals(dpiScale, m_calculatedUIScaling, 0.01f))
			CalculateIconSize();

		if (m_props.dynamicSizeToParent)
		{
			const Vector2 sz		 = m_parent->GetEndFromMargins() - m_parent->GetStartFromMargins();
			const float	  targetSize = Math::Min(sz.x, sz.y) * m_props.dynamicSizeScale;
			const float	  scale		 = targetSize / static_cast<float>(m_lvgFont->size);
			m_props.textScale		 = scale;

			if (!Math::Equals(m_props.textScale, m_lastDynScale, 0.01f))
				CalculateIconSize();
		}
	}

	void Icon::Draw()
	{
		if (m_lvgFont == nullptr)
			return;

		m_textOptions.uniqueID	= m_widgetProps.material;
		m_textOptions.color		= m_props.color.AsLVG();
		m_textOptions.textScale = m_props.textScale;

		if (m_widgetProps.hoveredIsDifferentColor && m_isHovered)
			m_textOptions.color = m_widgetProps.colorHovered.AsLVG();
		else if (m_widgetProps.pressedIsDifferentColor && m_isPressed)
			m_textOptions.color = m_widgetProps.colorPressed.AsLVG();

		if (GetFlags().IsSet(WF_DISABLED))
			m_textOptions.color = m_widgetProps.colorDisabled.AsLVG();

		m_lvg->DrawTextDefault(m_props.useAltIcon ? m_props.iconAlt.c_str() : m_props.icon.c_str(), (m_rect.pos + Vector2(0.0f, m_rect.size.y)).AsLVG(), m_textOptions, 0.0f, m_drawOrder, m_props.isDynamic);
	}

	void Icon::CalculateIconSize()
	{
		auto*		font	  = m_resourceManager->GetResource<Font>(m_props.font);
		const float uiScaling = m_manager->GetScalingFactor();
		m_lvgFont			  = font->GetFont(uiScaling);

		if (m_lvgFont == nullptr)
			return;

		m_lastDynScale			= m_props.textScale;
		m_calculatedUIScaling	= uiScaling;
		m_textOptions.font		= m_lvgFont;
		m_textOptions.textScale = m_props.textScale;
		m_rect.size				= m_lvg->CalculateTextSize(m_props.useAltIcon ? m_props.iconAlt.c_str() : m_props.icon.c_str(), m_textOptions);
	}

	bool Icon::OnMouse(uint32 button, LinaGX::InputAction act)
	{
		if (button != LINAGX_MOUSE_0)
			return false;

		if ((act == LinaGX::InputAction::Pressed || act == LinaGX::InputAction::Repeated) && m_isHovered && m_props.onClicked)
		{
			m_isPressed = true;
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

	void Icon::CollectResourceReferences(HashSet<ResourceID>& out)
	{
		Widget::CollectResourceReferences(out);
		out.insert(m_props.font);
	}

} // namespace Lina
