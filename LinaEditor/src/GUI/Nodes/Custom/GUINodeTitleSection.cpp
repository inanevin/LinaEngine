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
#include "Graphics/Platform/LinaVGIncl.hpp"
#include "Graphics/Interfaces/ISwapchain.hpp"
#include "Graphics/Interfaces/IWindow.hpp"
#include "Input/Core/InputMappings.hpp"
#include "GUI/Utility/GUIUtility.hpp"
#include "Core/Editor.hpp"

namespace Lina::Editor
{
	GUINodeTitleSection::GUINodeTitleSection(Editor* editor, ISwapchain* swapchain, int drawOrder, EditorPanel panel) : GUINode(editor, swapchain, drawOrder)
	{
		m_windowButtons = new GUINodeWindowButtons(editor, swapchain, drawOrder);
		m_windowButtons->SetCallbackOnClose([editor, panel]() { editor->ClosePanel(panel); });
		AddChildren(m_windowButtons);
	}

	void GUINodeTitleSection::Draw(int threadID)
	{
		LinaVG::StyleOptions style;
		style.color = LV4(Theme::TC_Dark1);
		LinaVG::DrawRect(threadID, LV2(m_rect.pos), LV2((m_rect.pos + m_rect.size)), style, 0.0f, m_drawOrder);

		const float	  imageHeight = m_rect.size.y * 0.5f;
		const float	  buttonY	  = m_rect.size.y;
		const float	  buttonX	  = buttonY * 16.0f / 9.0f;
		const Vector2 buttonSize  = Vector2(buttonX, buttonY);
		m_windowButtons->SetRect(Rect(Vector2(m_rect.size.x - buttonX * 3.0f, 0.0f), Vector2(buttonX * 3.0f, buttonY)));
		m_windowButtons->Draw(threadID);

		const float	  padding  = Theme::GetProperty(ThemeProperty::GeneralItemPadding, m_swapchain->GetWindowDPIScale());
		const Vector2 logoSize = Vector2(imageHeight, imageHeight);
		const Vector2 logoPos  = Vector2(padding, m_rect.size.y * 0.5f - logoSize.y * 0.5f);

		LinaVG::StyleOptions aq;
		aq.textureHandle = "Resources/Core/Textures/Logo_White_512.png"_hs;
		LinaVG::DrawRect(threadID, LV2(logoPos), LV2((logoPos + logoSize)), aq, 0.0f, m_drawOrder);

		const Rect dragRect = Rect(Vector2::Zero, Vector2(m_windowButtons->GetRect().pos.x, m_rect.size.y));
		m_swapchain->GetWindow()->SetDragRect(dragRect);
	}
	bool GUINodeTitleSection::OnMouse(uint32 button, InputAction act)
	{
		const bool ret = GUINode::OnMouse(button, act);

		if (button == LINA_MOUSE_0 && m_isHovered)
		{
			if (act == InputAction::Repeated)
			{
				if (m_swapchain->GetWindow()->IsMaximized())
					m_swapchain->GetWindow()->Restore();
				else
					m_swapchain->GetWindow()->Maximize();
			}
		}
		return ret;
	}

} // namespace Lina::Editor
