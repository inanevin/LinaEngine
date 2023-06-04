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

#include "GUI/Nodes/Widgets/GUINodeFileMenu.hpp"
#include "Data/CommonData.hpp"
#include "GUI/Nodes/Widgets/GUINodeButton.hpp"
#include "GUI/Nodes/Layouts/GUINodeLayoutHorizontal.hpp"
#include "GUI/Nodes/Layouts/GUINodeLayoutVertical.hpp"
#include "Graphics/Interfaces/ISwapchain.hpp"
#include "Graphics/Platform/LinaVGIncl.hpp"
#include "Math/Math.hpp"
#include "GUI/Utility/GUIUtility.hpp"
#include "Input/Core/InputMappings.hpp"
#include "Graphics/Interfaces/IWindow.hpp"

namespace Lina::Editor
{
#define SHORTCUT_TEXT_SCALE 1.0f

	void GUINodeFMPopupElement::Draw(int threadID)
	{
		if (!GetIsVisible())
			return;

		LinaVG::TextOptions textOpts;
		textOpts.font  = Theme::GetFont(FontType::DefaultEditor, m_window->GetDPIScale());
		textOpts.color = m_disabled ? LV4(Theme::TC_SilentTransparent) : LV4(Color::White);

		const float padding = Theme::GetProperty(ThemeProperty::GeneralItemPadding, m_window->GetDPIScale());

		if (m_isHovered)
		{
			LinaVG::StyleOptions bgRect;
			bgRect.color = LV4(Theme::TC_CyanAccent);
			LinaVG::DrawRect(threadID, LV2(m_rect.pos), LV2((m_rect.pos + m_rect.size)), bgRect, 0.0f, m_drawOrder);
		}

		const Vector2 titleSize = GetStoreSize("TitleSize"_hs, m_title);
		const Vector2 textPos	= Vector2(m_rect.pos.x + padding, m_rect.pos.y + m_rect.size.y * 0.5f + titleSize.y * 0.5f);
		LinaVG::DrawTextNormal(threadID, m_title.c_str(), LV2(textPos), textOpts, 0.0f, m_drawOrder, true);

		if (m_shortcut != Shortcut::None)
		{
			if (Math::Equals(m_shortcutXStartRight, 0.0f, 0.01f))
				m_shortcutXStartRight = m_rect.pos.x + m_rect.size.x - padding;

			LinaVG::TextOptions scOpts;
			scOpts.font		 = Theme::GetFont(FontType::AltEditor, m_window->GetDPIScale());
			scOpts.textScale = SHORTCUT_TEXT_SCALE;
			scOpts.color	 = LV4(Theme::TC_Light1);

			const Vector2 shortcutSize = GetStoreSize("ShortcutSize"_hs, SHORTCUT_TO_NAME_MAP.at(m_shortcut), FontType::AltEditor);
			const Vector2 shortcutPos  = Vector2(m_shortcutXStartRight - shortcutSize.x, m_rect.pos.y + m_rect.size.y * 0.5f + shortcutSize.y * 0.5f);
			LinaVG::DrawTextNormal(threadID, SHORTCUT_TO_NAME_MAP.at(m_shortcut), LV2(shortcutPos), scOpts, 0.0f, m_drawOrder);
		}
	}

	void GUINodeFMPopupElement::OnPressEnd(uint32 button)
	{
		if (button != LINA_MOUSE_0)
			return;

		if (m_onClicked)
			m_onClicked(this);
	}

	float GUINodeFMPopupElement::GetTotalWidth()
	{
		return GetStoreSize("TitleSize"_hs, m_title).x + (m_shortcut != Shortcut::None ? GetStoreSize("ShortcutSize"_hs, SHORTCUT_TO_NAME_MAP.at(m_shortcut), FontType::AltEditor).x * 1.32f : 0.0f);
	}

	void GUINodeFMPopupElementDivider::Draw(int threadID)
	{
		if (!GetIsVisible())
			return;

		LinaVG::TextOptions textOpts;
		textOpts.font	   = Theme::GetFont(FontType::AltEditor, m_window->GetDPIScale());
		textOpts.color	   = LV4(Theme::TC_Silent2);
		textOpts.alignment = LinaVG::TextAlignment::Right;

		const float	  padding	= Theme::GetProperty(ThemeProperty::GeneralItemPadding, m_window->GetDPIScale());
		const float	  centerY	= m_rect.pos.y + m_rect.size.y * 0.5f;
		const Vector2 titleSize = GetStoreSize("TitleSize"_hs, m_title);

		const Vector2 textPos	= Vector2(m_rect.pos.x + m_rect.size.x - padding, centerY + titleSize.y * 0.5f);
		const Vector2 lineStart = Vector2(m_rect.pos.x, centerY);
		const Vector2 lineEnd	= Vector2(m_rect.pos.x + m_rect.size.x - padding - titleSize.x * 1.1f, centerY);

		LinaVG::StyleOptions lineStyle;
		lineStyle.thickness	  = 1.5f * m_window->GetDPIScale();
		lineStyle.color		  = LV4(Theme::TC_Silent2);
		lineStyle.color.end.w = 0.2f;
		LinaVG::DrawLine(threadID, LV2(lineStart), LV2(lineEnd), lineStyle, LinaVG::LineCapDirection::None, 0.0f, m_drawOrder);

		LinaVG::DrawTextNormal(threadID, m_title.c_str(), LV2(textPos), textOpts, 0.0f, m_drawOrder);
	}

	float GUINodeFMPopupElementDivider::GetTotalWidth()
	{
		return GetStoreSize("TitleSize"_hs, m_title).x + 80.0f;
	}

	void GUINodeFMPopupElementToggle::Draw(int threadID)
	{
		if (!GetIsVisible())
			return;
		const float padding = Theme::GetProperty(ThemeProperty::GeneralItemPadding, m_window->GetDPIScale());

		if (!m_value)
		{
			m_shortcutXStartRight = m_rect.pos.x + m_rect.size.x - padding;
			GUINodeFMPopupElement::Draw(threadID);
		}
		else
		{
			const Vector2 iconSize = GetStoreSize("IconSize"_hs, TI_CHECKMARK, FontType::EditorIcons);
			const Vector2 iconPos  = Vector2(m_rect.pos.x + m_rect.size.x - padding - iconSize.x * 0.5f, m_rect.pos.y + m_rect.size.y * 0.5f);
			m_shortcutXStartRight  = iconPos.x - padding - iconSize.x * 0.5f;
			GUINodeFMPopupElement::Draw(threadID);
			GUIUtility::DrawIcon(0, m_window->GetDPIScale(), TI_CHECKMARK, iconPos, 1.0f, Theme::TC_Silent3, m_drawOrder);
		}
	}

	void GUINodeFMPopupElementToggle::OnPressEnd(uint32 button)
	{
		if (button != LINA_MOUSE_0)
			return;

		GUINodeFMPopupElement::OnPressEnd(button);
		m_value = !m_value;
	}

	float GUINodeFMPopupElementToggle::GetTotalWidth()
	{
		return GUINodeFMPopupElement::GetTotalWidth() + GetStoreSize("IconSize"_hs, TI_CARET_SLD_RIGHT, FontType::EditorIcons).x * 5.0f;
	}

	void GUINodeFMPopupElementExpandable::Draw(int threadID)
	{
		if (!GetIsVisible())
			return;

		// Background color will adjust according to the hover state
		// But we wanna keep the state as lastHovered.
		const bool isHovered = m_isHovered;
		m_isHovered			 = m_isLastHovered;
		GUINodeFMPopupElement::Draw(threadID);
		m_isHovered = isHovered;

		const float	  padding  = Theme::GetProperty(ThemeProperty::GeneralItemPadding, m_window->GetDPIScale());
		const Vector2 iconSize = GetStoreSize("IconSize"_hs, TI_CARET_SLD_RIGHT, FontType::EditorIcons);

		const Vector2 iconPos = Vector2(m_rect.pos.x + m_rect.size.x - padding * 0.5f - iconSize.x * 0.5f, m_rect.pos.y + m_rect.size.y * 0.5f);
		GUIUtility::DrawIcon(0, m_window->GetDPIScale(), TI_CARET_SLD_RIGHT, iconPos, 1.0f, Theme::TC_Silent3, m_drawOrder);

		if (m_isLastHovered)
		{
			m_popup->SetVisible(true);
			m_popup->SetPos(m_rect.pos + m_rect.size - Vector2(0, m_rect.size.y));
			m_popup->Draw(threadID);
		}
		else
			m_popup->SetVisible(false);
	}

	float GUINodeFMPopupElementExpandable::GetTotalWidth()
	{
		return GUINodeFMPopupElement::GetTotalWidth() + GetStoreSize("IconSize"_hs, TI_CARET_SLD_RIGHT, FontType::EditorIcons).x * 5.0f;
	}

	GUINodeFMPopupElementDivider* GUINodeFMPopup::AddDivider(const char* title)
	{
		GUINodeFMPopupElementDivider* e = new GUINodeFMPopupElementDivider(m_drawer, m_drawOrder + 1);
		e->SetTitle(title);
		e->SetSID(TO_SIDC(title));
		m_layout->AddChildren(e);
		m_elements.push_back(e);
		return e;
	}

	GUINodeFMPopupElement* GUINodeFMPopup::AddDefault(const char* title)
	{
		GUINodeFMPopupElement* e = new GUINodeFMPopupElement(m_drawer, m_drawOrder + 1, FMPopupElementType::Default);
		e->SetTitle(title);
		e->SetSID(TO_SIDC(title));
		m_layout->AddChildren(e);
		m_elements.push_back(e);
		e->SetCallbackClicked(BIND(&GUINodeFMPopup::OnPressedItem, this, std::placeholders::_1));
		return e;
	}

	GUINodeFMPopupElementToggle* GUINodeFMPopup::AddToggle(const char* title, bool initialValue)
	{
		GUINodeFMPopupElementToggle* e = new GUINodeFMPopupElementToggle(m_drawer, m_drawOrder + 1);
		e->SetValue(initialValue);
		e->SetTitle(title);
		e->SetSID(TO_SIDC(title));
		m_layout->AddChildren(e);
		m_elements.push_back(e);
		e->SetCallbackClicked(BIND(&GUINodeFMPopup::OnPressedItem, this, std::placeholders::_1));
		return e;
	}

	GUINodeFMPopupElementExpandable* GUINodeFMPopup::AddExpandable(const char* title, GUINodeFMPopup* popup)
	{
		GUINodeFMPopupElementExpandable* e = new GUINodeFMPopupElementExpandable(m_drawer, m_drawOrder + 1);
		e->SetExpandedPopup(popup);
		e->SetTitle(title);
		e->SetSID(TO_SIDC(title));
		e->AddChildren(popup);
		m_layout->AddChildren(e);
		m_elements.push_back(e);
		popup->SetVisible(false);
		return e;
	}

	void GUINodeFMPopup::OnPressedItem(GUINode* node)
	{
		if (m_onClicked)
			m_onClicked(node);
	}

	GUINodeFMPopup::GUINodeFMPopup(GUIDrawerBase* drawer, int drawOrder) : GUINode(drawer, drawOrder)
	{
		m_layout = new GUINodeLayoutVertical(drawer, 0);
		AddChildren(m_layout);
	}

	void GUINodeFMPopup::Draw(int threadID)
	{
		if (!GetIsVisible())
			return;

		const float padding		 = Theme::GetProperty(ThemeProperty::GeneralItemPadding, m_window->GetDPIScale());
		const float widgetHeight = Theme::GetProperty(ThemeProperty::WidgetHeightShort, m_window->GetDPIScale());

		float maxWidth	= 0.0f;
		float maxHeight = widgetHeight;

		for (auto e : m_elements)
			maxWidth = Math::Max(maxWidth, e->GetTotalWidth());

		maxWidth += padding * 2;

		const float heightMargin = padding * 0.5f;

		float totalY = 0.0f;
		for (auto e : m_elements)
		{
			if (e->GetType() == FMPopupElementType::Divider)
				e->SetSize(Vector2(maxWidth, maxHeight * 0.8f));
			else
				e->SetSize(Vector2(maxWidth, maxHeight));

			totalY += e->GetRect().size.y;

			if (e->GetIsHovered())
				m_lastHoveredElement = e;

			e->SetIsLastHovered(m_lastHoveredElement == e);
		}

		const Vector2 sz = Vector2(maxWidth, totalY + heightMargin * 2.0f);
		SetSize(sz);

		GUIUtility::DrawPopupBackground(threadID, m_rect, 1.0f * m_window->GetDPIScale(), m_drawOrder);
		m_layout->SetPos(m_rect.pos + Vector2(0, heightMargin));
		m_layout->Draw(threadID);
	}

	GUINodeFileMenu::GUINodeFileMenu(GUIDrawerBase* drawer, int drawOrder) : GUINode(drawer, drawOrder)
	{
		m_layout = new GUINodeLayoutHorizontal(drawer, 0);
		m_layout->SetSpacing(0.0f);
		AddChildren(m_layout);
	}

	void GUINodeFileMenu::Draw(int threadID)
	{
		if (!GetIsVisible())
			return;

		// Layout - this will draw buttons
		{
			m_layout->SetPos(m_rect.pos);
			m_layout->Draw(threadID);
			SetSize(m_layout->GetRect().size);
		}

		// Draw the target (open) popup
		{
			if (m_targetPopup != nullptr)
			{
				m_targetPopup->SetVisible(true);
				m_targetButton->SetDefaultColor(Theme::TC_CyanAccent);
				m_targetButton->SetHoveredColor(Theme::TC_CyanAccent);

				Vector2 pos = m_targetButton->GetRect().pos;
				pos.y += m_targetButton->GetRect().size.y;
				m_targetPopup->SetPos(pos);
				m_targetPopup->Draw(threadID);

				for (auto b : m_buttons)
				{
					if (b->GetIsHovered() && b != m_targetButton)
					{
						ResetTargets();
						m_targetButton = b;
						m_targetPopup  = static_cast<GUINodeFMPopup*>(b->GetChildren()[0]);
					}
				}
			}
		}
	}

	bool GUINodeFileMenu::OnMouse(uint32 button, InputAction act)
	{
		bool retVal = GUINode::OnMouse(button, act);

		if (button = LINA_MOUSE_0 && act == InputAction::Released && m_targetPopup && !GUIUtility::IsInRect(m_window->GetMousePosition(), m_rect) && !GUIUtility::IsInRect(m_window->GetMousePosition(), m_targetPopup->GetRect()))
			ResetTargets();

		return retVal;
	}

	void GUINodeFileMenu::AddSinglePopup(GUINodeFMPopup* popup)
	{
		const char* title = popup->GetTitle().c_str();
		popup->SetVisible(false);
		popup->SetCallbackClicked(BIND(&GUINodeFileMenu::OnPressedPopupElement, this, std::placeholders::_1));

		GUINodeButton* but = new GUINodeButton(m_drawer, m_drawOrder);
		but->SetDefaultColor(Theme::TC_Dark1);
		but->SetHoveredColor(Theme::TC_Light1);
		but->SetPressedColor(Theme::TC_Dark3);
		but->SetTitle(title);
		but->SetCallbackClicked(BIND(&GUINodeFileMenu::OnButtonClicked, this, std::placeholders::_1));
		but->SetEnableHoverOutline(true);
		but->SetOutlineColor(Theme::TC_Silent2);
		but->AddChildren(popup);

		m_buttons.push_back(but);
		m_layout->AddChildren(but);
	}

	void GUINodeFileMenu::OnButtonClicked(GUINode* but)
	{
		m_targetPopup  = static_cast<GUINodeFMPopup*>(but->GetChildren()[0]);
		m_targetButton = static_cast<GUINodeButton*>(but);
	}

	void GUINodeFileMenu::OnPressedPopupElement(GUINode* node)
	{
		ResetTargets();

		if (m_onClicked)
			m_onClicked(node);
	}

	void GUINodeFileMenu::ResetTargets()
	{
		if (m_targetPopup)
		{
			m_targetPopup->SwitchedFrom();
			m_targetButton->SetDefaultColor(Theme::TC_Dark1);
			m_targetButton->SetHoveredColor(Theme::TC_Light1);
			m_targetPopup->SetVisible(false);
		}
		m_targetButton = nullptr;
		m_targetPopup  = nullptr;
	}

} // namespace Lina::Editor
