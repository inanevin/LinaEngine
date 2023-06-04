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

#include "GUI/Nodes/Widgets/GUINodeSelection.hpp"
#include "GUI/Utility/GUIUtility.hpp"
#include "Graphics/Interfaces/ISwapchain.hpp"
#include "Graphics/Platform/LinaVGIncl.hpp"
#include "Graphics/Interfaces/IWindow.hpp"
#include "Math/Math.hpp"
#include "Input/Core/InputMappings.hpp"
#include "Core/SystemInfo.hpp"
#include "Data/CommonData.hpp"

namespace Lina::Editor
{
#define CARET_SPEED 22.0f

	GUINodeSelection::GUINodeSelection(GUIDrawerBase* drawer, int drawOrder) : GUINode(drawer, drawOrder)
	{
	}

	void GUINodeSelection::Draw(int threadID)
	{
		if (!GetIsVisible())
			return;

		const float padding		 = Theme::GetProperty(ThemeProperty::GeneralItemPadding, m_window->GetDPIScale());
		const float widgetHeight = Theme::GetProperty(ThemeProperty::WidgetHeightShort, m_window->GetDPIScale());
		const float iconPadding	 = m_xOffset + padding;
		float		totalSizeY	 = widgetHeight;
		SetWidgetHeight(ThemeProperty::WidgetHeightShort);

		// Childs
		{
			if (m_currentReorderRequest.state != 0)
			{
				if (m_currentReorderRequest.state == 1)
					Internal::PlaceAfter(m_childSelections, m_currentReorderRequest.src, m_currentReorderRequest.target);
				else
					Internal::PlaceBefore(m_childSelections, m_currentReorderRequest.src, m_currentReorderRequest.target);

				m_currentReorderRequest.state = 0;
			}

			if (m_isExpanded)
			{
				for (auto c : m_childSelections)
				{
					c->SetSize(Vector2(m_rect.size.x, widgetHeight));
					c->SetXOffset(m_xOffset + padding);
					c->SetPos(m_rect.pos + Vector2(0.0f, totalSizeY));
					c->Draw(threadID);
					totalSizeY += c->GetRect().size.y;
				}
			}
		}

		m_ownRect	  = Rect(m_rect.pos, Vector2(m_rect.size.x, m_rect.size.y));
		m_rect.size.y = totalSizeY;

		DrawBackground(threadID);
		DrawIcons(threadID, iconPadding);
		DrawTitleText(threadID, iconPadding + padding);
		HandleDrag(threadID);
		HandlePayload(threadID);
	}

	void GUINodeSelection::AddChildSelection(GUINodeSelection* sel)
	{
		AddChildren(sel);
		m_childSelections.push_back(sel);
		sel->SetVisible(m_isExpanded);
	}

	void GUINodeSelection::RemoveChildSelection(GUINodeSelection* sel)
	{
		RemoveChildren(sel);
		m_childSelections.erase(linatl::find_if(m_childSelections.begin(), m_childSelections.end(), [sel](GUINodeSelection* s) { return s == sel; }));
	}

	void GUINodeSelection::PlaceChildAfter(GUINodeSelection* child, GUINodeSelection* placeAfter)
	{
		m_currentReorderRequest.state  = 1;
		m_currentReorderRequest.src	   = child;
		m_currentReorderRequest.target = placeAfter;
	}

	void GUINodeSelection::PlaceChildBefore(GUINodeSelection* child, GUINodeSelection* placeBefore)
	{
		m_currentReorderRequest.state  = 2;
		m_currentReorderRequest.src	   = child;
		m_currentReorderRequest.target = placeBefore;
	}

	void GUINodeSelection::DrawBackground(int threadID)
	{
		LinaVG::StyleOptions opts;

		if (m_isHighlightEnabled)
		{
			opts.color = LV4(m_highlightColor);
			LinaVG::DrawRect(threadID, LV2(m_ownRect.pos), LV2((m_ownRect.pos + m_ownRect.size)), opts, 0.0f, m_drawOrder);
		}
		else
		{
			if (m_isSelected)
			{
				opts.color = LV4(Theme::TC_Dark2);
				LinaVG::DrawRect(threadID, LV2(m_ownRect.pos), LV2((m_ownRect.pos + m_ownRect.size)), opts, 0.0f, m_drawOrder);
			}
			else if (m_isHovered)
			{
				opts.color		   = LV4(Theme::TC_Dark1);
				opts.color.start.w = opts.color.end.w = 0.3f;
				LinaVG::DrawRect(threadID, LV2(m_ownRect.pos), LV2((m_ownRect.pos + m_ownRect.size)), opts, 0.0f, m_drawOrder);
			}
		}
	}

	Vector2 GUINodeSelection::DrawIcons(int threadID, float iconsStart)
	{
		Vector2 iconSize = Vector2::Zero;

		if (!m_childSelections.empty())
		{
			m_caretRotationTarget = Math::Lerp(m_caretRotationTarget, m_isExpanded ? 90.0f : 0.0f, SystemInfo::GetDeltaTimeF() * CARET_SPEED);
			iconSize = GUIUtility::DrawIcon(threadID, m_window->GetDPIScale(), TI_CARET_SLD_RIGHT, Vector2(m_ownRect.pos.x + iconsStart, m_ownRect.pos.y + m_ownRect.size.y * 0.5f), 1.0f, Theme::TC_Silent3, m_drawOrder, m_caretRotationTarget, true);

			// For click detection
			m_caretRect.pos	 = m_ownRect.pos + Vector2(iconsStart - iconSize.x * 0.5f, 0.0f);
			m_caretRect.size = Vector2(iconSize.x, m_ownRect.size.y);
		}

		return iconSize;
	}

	void GUINodeSelection::DrawTitleText(int threadID, float textStart)
	{
		LinaVG::TextOptions textOpts;
		const FontType		fontType = m_isBoldFont ? FontType::DefaultEditorBold : FontType::DefaultEditor;
		textOpts.font				 = Theme::GetFont(fontType, m_window->GetDPIScale());
		const Vector2 titleSize		 = GetStoreSize("TitleSize"_hs, m_title, fontType);
		const Vector2 textPos		 = Vector2(m_ownRect.pos.x + textStart, m_ownRect.pos.y + m_ownRect.size.y * 0.5f + titleSize.y * 0.5f);
		LinaVG::DrawTextNormal(threadID, m_title.c_str(), LV2(textPos), textOpts, 0.0f, m_drawOrder);
	}

	void GUINodeSelection::HandleDrag(int threadID)
	{
		// Drag and detach
		if (m_isPressed)
		{
			const Vector2 mousePosNow = m_window->GetMousePosition();
			const Vector2 deltaPress  = mousePosNow - Vector2(m_pressStartMousePos);

			if (!m_ownRect.IsPointInside(mousePosNow))
			{
				m_isPressed		= false;
				const Vector2 delta = Vector2(5, 5);
				if (m_onDetached)
					m_onDetached(this, delta);
			}
		}
	}

	void GUINodeSelection::HandlePayload(int threadID)
	{
		if (m_payloadAvailable)
		{
			const Vector2 mousePos = m_window->GetMousePosition();

			if (m_ownRect.IsPointInside(mousePos))
			{
				Rect halfRectDown = Rect(Vector2(m_ownRect.pos.x + m_xOffset, m_ownRect.pos.y + m_ownRect.size.y * 0.85f), Vector2(m_ownRect.size.x - m_xOffset, m_ownRect.size.y * 0.15f));

				// Draw underline or full rect.
				LinaVG::StyleOptions opts;
				opts.color = LV4(Theme::TC_CyanAccent);

				if (halfRectDown.IsPointInside(mousePos))
				{
					if (m_isExpanded)
						LinaVG::DrawRect(threadID, LV2(Vector2(halfRectDown.pos.x, m_rect.pos.y + m_rect.size.y - halfRectDown.size.y)), LV2((m_rect.pos + m_rect.size)), opts, 0.0f, m_drawOrder + 1);
					else
						LinaVG::DrawRect(threadID, LV2(halfRectDown.pos), LV2((halfRectDown.pos + halfRectDown.size)), opts, 0.0f, m_drawOrder + 1);

					m_payloadDropState = 1;
				}
				else
				{
					opts.color.start.w = opts.color.end.w = 0.3f;
					LinaVG::DrawRect(threadID, LV2(m_ownRect.pos), LV2((m_ownRect.pos + m_ownRect.size)), opts, 0.0f, m_drawOrder + 1);

					m_payloadDropState = 2;
				}
			}
			else
				m_payloadDropState = 0;
		}
	}

	void GUINodeSelection::SetExpandStateImpl(bool expandState)
	{
		m_isExpanded = expandState;

		for (auto c : m_childSelections)
			c->SetVisible(expandState);
	}

	void GUINodeSelection::OnPressBegin(uint32 button)
	{
		if (button != LINA_MOUSE_0)
			return;

		if (m_onClicked)
			m_onClicked(this);

		if (!m_childSelections.empty() && m_caretRect.IsPointInside(m_window->GetMousePosition()))
			SetExpandStateImpl(!m_isExpanded);
	}

	void GUINodeSelection::OnDoubleClicked()
	{
		if (m_childSelections.empty() || m_caretRect.IsPointInside(m_window->GetMousePosition()))
			return;

		SetExpandStateImpl(!m_isExpanded);
	}

	void GUINodeSelection::OnPayloadAccepted()
	{
		if (m_payloadDropState != 0)
		{
			if (m_onPayloadAccepted)
				m_onPayloadAccepted(this, &m_payloadDropState);

			m_payloadDropState = 0;
		}
	}
} // namespace Lina::Editor
