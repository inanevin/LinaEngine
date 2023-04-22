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
	void GUINodeFMPopupElement::Draw(int threadID)
	{
		if (!m_visible)
			return;

		LinaVG::TextOptions textOpts;
		textOpts.font		= Theme::GetFont(FontType::DefaultEditor, m_window->GetDPIScale());
		const float padding = Theme::GetProperty(ThemeProperty::GeneralItemPadding, m_window->GetDPIScale());

		if (m_isHovered)
		{
			LinaVG::StyleOptions bgRect;
			bgRect.color = LV4(Theme::TC_CyanAccent);
			LinaVG::DrawRect(threadID, LV2(m_rect.pos), LV2((m_rect.pos + m_rect.size)), bgRect, 0.0f, m_drawOrder);
		}

		const Vector2 textPos = Vector2(m_rect.pos.x + padding, m_rect.pos.y + m_rect.size.y * 0.5f + m_lastCalculatedSize.y * 0.5f);
		LinaVG::DrawTextNormal(threadID, m_title.c_str(), LV2(textPos), textOpts, 0.0f, m_drawOrder);
	}

	Vector2 GUINodeFMPopupElement::CalculateSize()
	{
		const float windowDPI = m_window->GetDPIScale();
		if (Math::Equals(m_lastDpi, windowDPI, 0.0001f))
			return m_lastCalculatedSize;

		m_lastDpi = windowDPI;
		LinaVG::TextOptions textOpts;
		textOpts.font		 = Theme::GetFont(FontType::DefaultEditor, windowDPI);
		m_lastCalculatedSize = FL2(LinaVG::CalculateTextSize(m_title.c_str(), textOpts));
		return m_lastCalculatedSize;
	}

	void GUINodeFMPopupElement::OnClicked(uint32 button)
	{
		if (button != LINA_MOUSE_0)
			return;

		if (m_onClicked)
			m_onClicked(this);
	}

	void GUINodeFMPopupElementDivider::Draw(int threadID)
	{
		if (!m_visible)
			return;

		LinaVG::TextOptions textOpts;
		textOpts.font		= Theme::GetFont(FontType::AltEditor, m_window->GetDPIScale());
		textOpts.color		= LV4(Theme::TC_Silent2);
		textOpts.alignment	= LinaVG::TextAlignment::Right;
		const float padding = Theme::GetProperty(ThemeProperty::GeneralItemPadding, m_window->GetDPIScale());

		const float centerY = m_rect.pos.y + m_rect.size.y * 0.5f;

		const Vector2 textPos	= Vector2(m_rect.pos.x + m_rect.size.x - padding, centerY + m_lastCalculatedSize.y * 0.5f);
		const Vector2 textSize	= FL2(LinaVG::CalculateTextSize(m_title.c_str(), textOpts));
		const Vector2 lineStart = Vector2(m_rect.pos.x, centerY);
		const Vector2 lineEnd	= Vector2(m_rect.pos.x + m_rect.size.x - padding - textSize.x * 1.1f, centerY);

		LinaVG::StyleOptions lineStyle;
		lineStyle.thickness	  = 1.5f * m_window->GetDPIScale();
		lineStyle.color		  = LV4(Theme::TC_Silent2);
		lineStyle.color.end.w = 0.2f;
		LinaVG::DrawLine(threadID, LV2(lineStart), LV2(lineEnd), lineStyle, LinaVG::LineCapDirection::None, 0.0f, m_drawOrder);

		LinaVG::DrawTextNormal(threadID, m_title.c_str(), LV2(textPos), textOpts, 0.0f, m_drawOrder);
	}

	Vector2 GUINodeFMPopupElementDivider::CalculateSize()
	{
		const float windowDPI = m_window->GetDPIScale();
		if (Math::Equals(m_lastDpi, windowDPI, 0.0001f))
			return m_lastCalculatedSize;

		m_lastDpi = windowDPI;
		LinaVG::TextOptions textOpts;
		textOpts.font		 = Theme::GetFont(FontType::AltEditor, windowDPI);
		m_lastCalculatedSize = FL2(LinaVG::CalculateTextSize(m_title.c_str(), textOpts));
		m_lastCalculatedSize.x *= 2.5f; // there is an offset to where the text will start.
		return m_lastCalculatedSize;
	}

	void GUINodeFMPopupElementToggle::Draw(int threadID)
	{
		if (!m_visible)
			return;

		GUINodeFMPopupElement::Draw(threadID);

		if (!m_value)
			return;

		const float	  padding = Theme::GetProperty(ThemeProperty::GeneralItemPadding, m_window->GetDPIScale());
		const Vector2 iconPos = Vector2(m_rect.pos.x + m_rect.size.x - padding - m_iconSize.x * 0.5f, m_rect.pos.y + m_rect.size.y * 0.5f);
		GUIUtility::DrawIcon(0, m_window->GetDPIScale(), TI_CHECKMARK, iconPos, 1.0f, Theme::TC_Silent3, m_drawOrder);
	}

	Vector2 GUINodeFMPopupElementToggle::CalculateSize()
	{
		const float windowDPI = m_window->GetDPIScale();
		if (Math::Equals(m_lastDpi, windowDPI, 0.0001f))
			return m_lastCalculatedSize;

		m_lastDpi = windowDPI;

		LinaVG::TextOptions textOpts;
		textOpts.font		 = Theme::GetFont(FontType::DefaultEditor, windowDPI);
		m_lastCalculatedSize = FL2(LinaVG::CalculateTextSize(m_title.c_str(), textOpts));

		LinaVG::SDFTextOptions icon;
		icon.font		 = Theme::GetFont(FontType::EditorIcons, windowDPI);
		Vector2 iconSize = FL2(LinaVG::CalculateTextSize(TI_CHECKMARK, icon));
		m_lastCalculatedSize.x += iconSize.x * 5.0f;
		m_iconSize = iconSize;
		return m_lastCalculatedSize;
	}

	void GUINodeFMPopupElementToggle::OnClicked(uint32 button)
	{
		if (button != LINA_MOUSE_0)
			return;

		GUINodeFMPopupElement::OnClicked(button);
		m_value = !m_value;
	}

	void GUINodeFMPopupElementExpandable::Draw(int threadID)
	{
		if (!m_visible)
			return;

		// Background color will adjust according to the hover state
		// But we wanna keep the state as lastHovered.
		const bool isHovered = m_isHovered;
		m_isHovered			 = m_isLastHovered;
		GUINodeFMPopupElement::Draw(threadID);
		m_isHovered = isHovered;

		const float	  padding = Theme::GetProperty(ThemeProperty::GeneralItemPadding, m_window->GetDPIScale());
		const Vector2 iconPos = Vector2(m_rect.pos.x + m_rect.size.x - padding * 0.5f - m_iconSize.x * 0.5f, m_rect.pos.y + m_rect.size.y * 0.5f);
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

	Vector2 GUINodeFMPopupElementExpandable::CalculateSize()
	{
		const float windowDPI = m_window->GetDPIScale();
		if (Math::Equals(m_lastDpi, windowDPI, 0.0001f))
			return m_lastCalculatedSize;

		m_lastDpi = windowDPI;

		LinaVG::TextOptions textOpts;
		textOpts.font		 = Theme::GetFont(FontType::DefaultEditor, windowDPI);
		m_lastCalculatedSize = FL2(LinaVG::CalculateTextSize(m_title.c_str(), textOpts));

		LinaVG::SDFTextOptions icon;
		icon.font		 = Theme::GetFont(FontType::EditorIcons, windowDPI);
		Vector2 iconSize = FL2(LinaVG::CalculateTextSize(TI_CARET_SLD_RIGHT, icon));
		m_lastCalculatedSize.x += iconSize.x * 8.0f;
		m_iconSize = iconSize;
		return m_lastCalculatedSize;
	}

	GUINodeFMPopup* GUINodeFMPopup::AddDivider(const char* title)
	{
		GUINodeFMPopupElementDivider* e = new GUINodeFMPopupElementDivider(m_drawer, m_drawOrder + 1);
		e->SetTitle(title);
		e->SetSID(TO_SIDC(title));
		m_layout->AddChildren(e);
		m_elements.push_back(e);
		return this;
	}

	GUINodeFMPopup* GUINodeFMPopup::AddDefault(const char* title)
	{
		GUINodeFMPopupElement* e = new GUINodeFMPopupElement(m_drawer, m_drawOrder + 1, FMPopupElementType::Default);
		e->SetTitle(title);
		e->SetSID(TO_SIDC(title));
		m_layout->AddChildren(e);
		m_elements.push_back(e);
		e->SetCallbackClicked(BIND(&GUINodeFMPopup::OnPressedItem, this, std::placeholders::_1));
		return this;
	}

	GUINodeFMPopup* GUINodeFMPopup::AddToggle(const char* title, bool initialValue)
	{
		GUINodeFMPopupElementToggle* e = new GUINodeFMPopupElementToggle(m_drawer, m_drawOrder + 1);
		e->SetValue(initialValue);
		e->SetTitle(title);
		e->SetSID(TO_SIDC(title));
		m_layout->AddChildren(e);
		m_elements.push_back(e);
		e->SetCallbackClicked(BIND(&GUINodeFMPopup::OnPressedItem, this, std::placeholders::_1));
		return this;
	}

	GUINodeFMPopup* GUINodeFMPopup::AddExpandable(const char* title, GUINodeFMPopup* popup)
	{
		GUINodeFMPopupElementExpandable* e = new GUINodeFMPopupElementExpandable(m_drawer, m_drawOrder + 1);
		e->SetExpandedPopup(popup);
		e->SetTitle(title);
		e->SetSID(TO_SIDC(title));
		e->AddChildren(popup);
		m_layout->AddChildren(e);
		m_elements.push_back(e);
		popup->SetVisible(false);
		return this;
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
		if (!m_visible)
			return;

		Vector2 maxSize = Vector2::Zero;

		for (auto e : m_elements)
			maxSize = maxSize.Max(e->CalculateSize());

		const float padding		 = Theme::GetProperty(ThemeProperty::GeneralItemPadding, m_window->GetDPIScale());
		const float heightMargin = padding * 0.5f;
		maxSize.x += padding * 2;
		maxSize.y += padding * 1.35f;

		float totalY = 0.0f;
		for (auto e : m_elements)
		{
			if (e->GetType() == FMPopupElementType::Divider)
				e->SetSize(Vector2(maxSize.x, maxSize.y * 0.8f));
			else
				e->SetSize(maxSize);

			totalY += e->GetRect().size.y;

			if (e->GetIsHovered())
				m_lastHoveredElement = e;

			e->SetIsLastHovered(m_lastHoveredElement == e);
		}

		const Vector2 sz = Vector2(maxSize.x, totalY + heightMargin * 2.0f);
		SetSize(sz);
		SetLayoutOffset(heightMargin);

		GUIUtility::DrawPopupBackground(threadID, m_rect, 1.0f * m_window->GetDPIScale(), FRONT_DRAW_ORDER);
		m_layout->SetPos(m_rect.pos + Vector2(0, m_layoutOffset));
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
		if (!m_visible)
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
		const char*	   title	  = popup->GetTitle().c_str();
		const FontType targetFont = FontType::DefaultEditor;
		popup->SetVisible(false);
		popup->SetCallbackClicked(BIND(&GUINodeFileMenu::OnPressedPopupElement, this, std::placeholders::_1));

		GUINodeButton* but = new GUINodeButton(m_drawer, m_drawOrder);
		but->SetFontType(targetFont);
		but->SetDefaultColor(Theme::TC_Dark1);
		but->SetHoveredColor(Theme::TC_Light1);
		but->SetPressedColor(Theme::TC_Dark3);
		but->SetTitle(title);
		but->SetFitType(ButtonFitType::AutoFitFromTextAndPadding);
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
