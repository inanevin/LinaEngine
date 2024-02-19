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

#include "GUI/Nodes/Custom/GUINodeTitleSection.hpp"
#include "GUI/Nodes/Widgets/GUINodeWindowButtons.hpp"
#include "Data/CommonData.hpp"
#include "Core/Theme.hpp"
#include "Platform/LinaVGIncl.hpp"
#include "GUI/Utility/GUIUtility.hpp"
#include "Core/Editor.hpp"

namespace Lina::Editor
{
	GUINodeTitleSection::GUINodeTitleSection(GUIDrawerBase* drawer, int drawOrder) : GUINode(drawer, drawOrder)
	{
		m_windowButtons = new GUINodeWindowButtons(drawer, drawOrder);
		m_windowButtons->SetCallbackDismissed([this](GUINode* node) { m_editor->CloseWindow(m_window->GetSID()); });
		AddChildren(m_windowButtons);
	}

	void GUINodeTitleSection::Draw(int threadID)
	{
		// Background
		{
			LinaVG::StyleOptions style;
			style.color = Theme::TC_Dark1.AsLVG4();
			LinaVG::DrawRect(threadID, m_rect.pos.AsLVG2(), (m_rect.pos + m_rect.size).AsLVG2(), style, 0.0f, m_drawOrder);
		}

		// Setup
		const float imageHeight = m_rect.size.y * 0.5f;

		// Window buttons
		{
			const float	  buttonY	 = m_rect.size.y;
			const float	  buttonX	 = buttonY * 16.0f / 9.0f;
			const Vector2 buttonSize = Vector2(buttonX, buttonY);
			m_windowButtons->SetRect(Rect(Vector2(m_rect.size.x - buttonX * 3.0f, 0.0f), Vector2(buttonX * 3.0f, buttonY)));
			m_windowButtons->SetMinSize(Vector2(0, m_rect.size.y));
			m_windowButtons->Draw(threadID);
		}

		float logoEndX = 0.0f;

		// Logo
		{
			const float			 padding  = Theme::GetProperty(ThemeProperty::GeneralItemPadding, m_window->GetDPIScale());
			const Vector2		 logoSize = Vector2(imageHeight, imageHeight);
			const Vector2		 logoPos  = Vector2(padding, m_rect.size.y * 0.5f - logoSize.y * 0.5f);
			LinaVG::StyleOptions logo;
			logo.textureHandle = "Resources/Core/Textures/StubLinaLogo.png"_hs;
			LinaVG::DrawRect(threadID, logoPos.AsLVG2(), (logoPos + logoSize).AsLVG2(), logo, 0.0f, m_drawOrder);
			logoEndX = logoPos.x + logoSize.x * 0.5f + padding * 1.5f;
		}

		// Title
		{
			const char*			title = m_window->GetTitle().c_str();
			LinaVG::TextOptions opts;
			opts.font			   = Theme::GetFont(FontType::TitleEditor, m_window->GetDPIScale());
			const Vector2 textSize = LinaVG::CalculateTextSize(title, opts);
			const Vector2 titlePos = Vector2(logoEndX, m_rect.pos.y + m_rect.size.y * 0.5f + textSize.y * 0.5f);
			LinaVG::DrawTextNormal(threadID, title, titlePos.AsLVG2(), opts, 0.0f, m_drawOrder, true);
		}

		// Drag rect
		{
			const Rect				   dragRect = Rect(Vector2::Zero, Vector2(m_windowButtons->GetRect().pos.x, m_rect.size.y));
			const LinaGX::LGXVector2ui pos		= {dragRect.pos.x, dragRect.pos.y};
			const LinaGX::LGXVector2ui size		= {dragRect.size.x, dragRect.size.y};
			m_window->SetDragRect({.pos = pos, .size = size});
		}
	}

} // namespace Lina::Editor
