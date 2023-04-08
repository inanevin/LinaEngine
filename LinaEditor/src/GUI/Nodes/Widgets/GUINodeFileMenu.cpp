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

namespace Lina::Editor
{
	void GUINodeFMPopupElement::Draw(int threadID)
	{
		LinaVG::TextOptions textOpts;
		textOpts.font		= Theme::GetFont(FontType::DefaultEditor, m_swapchain->GetWindowDPIScale());
		const float padding = Theme::GetProperty(ThemeProperty::GeneralItemPadding, m_swapchain->GetWindowDPIScale());

		const Vector2 textPos = Vector2(m_rect.pos.x + padding, m_rect.pos.y + m_rect.size.y * 0.5f + m_lastSize.y * 0.5f);
		LinaVG::DrawTextNormal(threadID, m_title.c_str(), LV2(textPos), textOpts, 0.0f, m_drawOrder);
	}

	Vector2 GUINodeFMPopupElement::CalculateSize()
	{
		const float windowDPI = m_swapchain->GetWindowDPIScale();
		if (Math::Equals(m_lastDPIScale, windowDPI, 0.0001f))
			return m_lastSize;

		m_lastDPIScale = windowDPI;
		LinaVG::TextOptions textOpts;
		textOpts.font = Theme::GetFont(FontType::DefaultEditor, windowDPI);
		m_lastSize	  = FL2(LinaVG::CalculateTextSize(m_title.c_str(), textOpts));
		return m_lastSize;
	}

	void GUINodeFMPopupElementDivider::Draw(int threadID)
	{
		LinaVG::TextOptions textOpts;
		textOpts.font		= Theme::GetFont(FontType::AltEditor, m_swapchain->GetWindowDPIScale());
		textOpts.color		= LV4(Theme::TC_Silent);
		textOpts.alignment	= LinaVG::TextAlignment::Right;
		const float padding = Theme::GetProperty(ThemeProperty::GeneralItemPadding, m_swapchain->GetWindowDPIScale());

		const float centerY = m_rect.pos.y + m_rect.size.y * 0.5f;

		const Vector2 textPos	= Vector2(m_rect.pos.x + m_rect.size.x - padding, centerY + m_lastSize.y * 0.5f);
		const Vector2 textSize	= FL2(LinaVG::CalculateTextSize(m_title.c_str(), textOpts));
		const Vector2 lineStart = Vector2(m_rect.pos.x, centerY);
		const Vector2 lineEnd	= Vector2(m_rect.pos.x + m_rect.size.x - padding - textSize.x * 1.1f, centerY);

		LinaVG::StyleOptions lineStyle;
		lineStyle.thickness	  = 1.5f * m_swapchain->GetWindowDPIScale();
		lineStyle.color		  = LV4(Theme::TC_Silent);
		lineStyle.color.end.w = 0.2f;
		LinaVG::DrawLine(threadID, LV2(lineStart), LV2(lineEnd), lineStyle, LinaVG::LineCapDirection::None, 0.0f, m_drawOrder);

		LinaVG::DrawTextNormal(threadID, m_title.c_str(), LV2(textPos), textOpts, 0.0f, m_drawOrder);
	}

	Vector2 GUINodeFMPopupElementDivider::CalculateSize()
	{
		const float windowDPI = m_swapchain->GetWindowDPIScale();
		if (Math::Equals(m_lastDPIScale, windowDPI, 0.0001f))
			return m_lastSize;

		m_lastDPIScale = windowDPI;
		LinaVG::TextOptions textOpts;
		textOpts.font = Theme::GetFont(FontType::AltEditor, windowDPI);
		m_lastSize	  = FL2(LinaVG::CalculateTextSize(m_title.c_str(), textOpts));
		m_lastSize.x *= 2.5f; // there is an offset to where the text will start.
		return m_lastSize;
	}

	void GUINodeFMPopupElementToggle::Draw(int threadID)
	{
	}

	Vector2 GUINodeFMPopupElementToggle::CalculateSize()
	{
		const float windowDPI = m_swapchain->GetWindowDPIScale();
		if (Math::Equals(m_lastDPIScale, windowDPI, 0.0001f))
			return m_lastSize;

		m_lastDPIScale = windowDPI;

		LinaVG::TextOptions textOpts;
		textOpts.font = Theme::GetFont(FontType::DefaultEditor, windowDPI);
		m_lastSize	  = FL2(LinaVG::CalculateTextSize(m_title.c_str(), textOpts));

		LinaVG::TextOptions icon;
		icon.font		 = Theme::GetFont(FontType::EditorIcons, windowDPI);
		Vector2 iconSize = FL2(LinaVG::CalculateTextSize(TI_CHECKMARK, icon));
		m_lastSize.x += iconSize.x;
		m_lastSize.y = Math::Max(m_lastSize.y, iconSize.y);
		return m_lastSize;
	}

	void GUINodeFMPopupElementExpandable::Draw(int threadID)
	{
	}

	Vector2 GUINodeFMPopupElementExpandable::CalculateSize()
	{
		const float windowDPI = m_swapchain->GetWindowDPIScale();
		if (Math::Equals(m_lastDPIScale, windowDPI, 0.0001f))
			return m_lastSize;

		m_lastDPIScale = windowDPI;

		LinaVG::TextOptions textOpts;
		textOpts.font = Theme::GetFont(FontType::DefaultEditor, windowDPI);
		m_lastSize	  = FL2(LinaVG::CalculateTextSize(m_title.c_str(), textOpts));

		LinaVG::TextOptions icon;
		icon.font		 = Theme::GetFont(FontType::EditorIcons, windowDPI);
		Vector2 iconSize = FL2(LinaVG::CalculateTextSize(TI_CARET_SLD_RIGHT, icon));
		m_lastSize.x += iconSize.x;
		m_lastSize.y = Math::Max(m_lastSize.y, iconSize.y);
		return m_lastSize;
	}

	GUINodeFMPopup* GUINodeFMPopup::AddDivider(const char* title)
	{
		GUINodeFMPopupElementDivider* e = new GUINodeFMPopupElementDivider(m_editor, m_swapchain, m_drawOrder + 1);
		e->SetTitle(title);
		m_layout->AddChildren(e);
		m_elements.push_back(e);
		return this;
	}

	GUINodeFMPopup* GUINodeFMPopup::AddDefault(const char* title)
	{
		GUINodeFMPopupElement* e = new GUINodeFMPopupElement(m_editor, m_swapchain, m_drawOrder + 1, FMPopupElementType::Default);
		e->SetTitle(title);
		m_layout->AddChildren(e);
		m_elements.push_back(e);
		return this;
	}

	GUINodeFMPopup* GUINodeFMPopup::AddToggle(const char* title, bool initialValue)
	{
		GUINodeFMPopupElementToggle* e = new GUINodeFMPopupElementToggle(m_editor, m_swapchain, m_drawOrder + 1);
		e->SetValue(initialValue)->SetTitle(title);
		// m_layout->AddChildren(e);
		// m_elements.push_back(e);
		return this;
	}

	GUINodeFMPopup* GUINodeFMPopup::AddExpandable(const char* title, GUINodeFMPopup* popup)
	{
		GUINodeFMPopupElementExpandable* e = new GUINodeFMPopupElementExpandable(m_editor, m_swapchain, m_drawOrder + 1);
		e->SetExpandedPopup(popup)->SetTitle(title);
		e->AddChildren(popup);
		// m_layout->AddChildren(e);
		// m_elements.push_back(e);
		return this;
	}

	GUINodeFMPopup::GUINodeFMPopup(Editor* editor, ISwapchain* swapchain, int drawOrder) : GUINode(editor, swapchain, drawOrder)
	{
		m_layout = new GUINodeLayoutVertical(editor, swapchain, 0);
		AddChildren(m_layout);
	}

	void GUINodeFMPopup::Draw(int threadID)
	{
		m_layout->SetPos(m_rect.pos);
		m_layout->Draw(threadID);
	}

	GUINodeFileMenu::GUINodeFileMenu(Editor* editor, ISwapchain* swapchain, int drawOrder) : GUINode(editor, swapchain, drawOrder)
	{
		m_layout = (new GUINodeLayoutHorizontal(editor, swapchain, 0))->SetSpacing(0.0f);
		AddChildren(m_layout);
	}

	void GUINodeFileMenu::Draw(int threadID)
	{
		m_layout->SetPos(m_rect.pos);
		m_layout->Draw(threadID);

		if (m_targetPopup != nullptr)
		{
			m_targetButton->SetDefaultColor(Theme::TC_RedAccent);
			m_targetButton->SetHoveredColor(Theme::TC_RedAccent);

			auto& elements = m_targetPopup->GetElements();

			Vector2 maxSize = Vector2::Zero;

			for (auto e : elements)
				maxSize = maxSize.Max(e->CalculateSize());

			const float padding		 = Theme::GetProperty(ThemeProperty::GeneralItemPadding, m_swapchain->GetWindowDPIScale());
			const float heightMargin = padding;
			maxSize.x += padding * 2;
			maxSize.y += padding * 1.35f;

			float totalY = 0.0f;
			for (auto e : elements)
			{
				if (e->GetType() == FMPopupElementType::Divider)
					e->SetSize(Vector2(maxSize.x, maxSize.y * 0.8f));
				else
					e->SetSize(maxSize);

				totalY += e->GetRect().size.y;
			}

			Vector2 pos = m_targetButton->GetRect().pos;
			pos.y += m_targetButton->GetRect().size.y;

			GUIUtility::DrawPopupBackground(threadID, Rect(pos, Vector2(maxSize.x, totalY + heightMargin * 2.0f)), 1.0f * m_swapchain->GetWindowDPIScale(), POPUP_DRAW_ORDER);

			pos.y += heightMargin;
			m_targetPopup->SetPos(pos);
			m_targetPopup->Draw(threadID);

			for (auto b : m_buttons)
			{
				if (b->GetIsHovered() && b != m_targetButton)
				{
					m_targetButton->SetDefaultColor(Theme::TC_Dark1);
					m_targetButton = b;
					m_targetPopup  = m_buttonPopupMap[b];
				}
			}
		}
	}

	void GUINodeFileMenu::AddSinglePopup(GUINodeFMPopup* popup)
	{
		const char*	   title	  = popup->GetTitle().c_str();
		const FontType targetFont = FontType::DefaultEditor;

		GUINodeButton* but = new GUINodeButton(m_editor, m_swapchain, m_drawOrder);
		but->SetFontType(targetFont)->SetText(title)->SetDefaultColor(Theme::TC_Dark1)->SetHoveredColor(Theme::TC_Light1)->SetPressedColor(Theme::TC_Dark2);
		but->SetFitType(ButtonFitType::AutoFitFromTextAndPadding)->SetListener(BIND(&GUINodeFileMenu::OnButtonClicked, this, std::placeholders::_1));
		m_layout->AddChildren(but);
		m_buttons.push_back(but);
		m_buttonPopupMap[but] = popup;
	}

	void GUINodeFileMenu::OnButtonClicked(GUINodeButton* but)
	{
		m_targetPopup  = m_buttonPopupMap[but];
		m_targetButton = but;
	}

} // namespace Lina::Editor
