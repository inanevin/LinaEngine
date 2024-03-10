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

#include "Editor/Widgets/EditorRoot.hpp"
#include "Editor/Widgets/Compound/WindowButtons.hpp"
#include "Editor/CommonEditor.hpp"
#include "Core/GUI/Widgets/Compound/FileMenu.hpp"
#include "Core/GUI/Widgets/Primitives/Icon.hpp"
#include "Editor/EditorLocale.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina::Editor
{
	void EditorRoot::Construct()
	{
		// Logo
		m_linaLogo					 = Allocate<Icon>("LinaLogo");
		m_linaLogo->GetProps().icon	 = ICON_LINA_LOGO;
		m_linaLogo->GetProps().color = Theme::GetDef().accentPrimary0;
		m_linaLogo->Initialize();
		AddChild(m_linaLogo);

		// Filemenu
		m_fm										   = Allocate<FileMenu>("FileMenu");
		m_fm->GetProps().buttonProps.outlineThickness  = 0.0f;
		m_fm->GetProps().buttonProps.rounding		   = 0.0f;
		m_fm->GetProps().buttonProps.colorDefaultStart = Theme::GetDef().background1;
		m_fm->GetProps().buttonProps.colorDefaultEnd   = Theme::GetDef().background1;
		m_fm->GetProps().buttonProps.colorPressed	   = Theme::GetDef().background2;
		m_fm->GetProps().buttonProps.colorHovered	   = Theme::GetDef().background3;
		m_fm->GetProps().buttons					   = {Locale::GetStr(LocaleStr::File), Locale::GetStr(LocaleStr::Edit), Locale::GetStr(LocaleStr::View), Locale::GetStr(LocaleStr::Panels), Locale::GetStr(LocaleStr::About)};
		AddChild(m_fm);

		// Window buttons
		m_windowButtons = Allocate<WindowButtons>("WindowButtons");
		AddChild(m_windowButtons);
	}

	void EditorRoot::Tick(float delta)
	{
		Widget::SetIsHovered();

		const float defaultHeight	  = Theme::GetBaseItemHeight(m_lgxWindow->GetDPIScale());
		const float indent			  = Theme::GetDef().baseIndent;
		const float windowButtonWidth = defaultHeight * 2.0f;

		SetPos(Vector2::Zero);
		SetSize(Vector2(static_cast<float>(m_lgxWindow->GetSize().x), static_cast<float>(m_lgxWindow->GetSize().y)));

		m_topRect.pos  = Vector2::Zero;
		m_topRect.size = Vector2(GetSizeX(), defaultHeight);

		m_windowButtons->SetPosX(m_rect.GetEnd().x - windowButtonWidth * 3.0f);
		m_windowButtons->SetPosY(m_rect.pos.y);
		m_windowButtons->SetSize(Vector2(windowButtonWidth * 3.0f, defaultHeight));
		m_windowButtons->Tick(delta);

		m_linaLogo->SetPosX(indent);
		m_linaLogo->SetPosY(m_topRect.GetCenter().y - m_linaLogo->GetHalfSizeY());
		m_linaLogo->Tick(delta);

		m_fm->SetPosX(indent * 2.0f + m_linaLogo->GetSizeX());
		m_fm->SetPosY(0.0f);
		m_fm->SetSizeX(GetSizeX());
		m_fm->SetSizeY(defaultHeight);
		m_fm->Tick(delta);

		// Drag rect.
		m_dragRect				  = Rect(Vector2(m_fm->GetPosX() + m_fm->GetSizeX(), 0.0f), Vector2(GetSizeX() - windowButtonWidth * 3.0f, defaultHeight));
		LinaGX::LGXRectui lgxRect = {};
		lgxRect.pos				  = LinaGX::LGXVector2ui{static_cast<uint32>(m_dragRect.pos.x), static_cast<uint32>(m_dragRect.pos.y)};
		lgxRect.size			  = LinaGX::LGXVector2ui{static_cast<uint32>(m_dragRect.size.x), static_cast<uint32>(m_dragRect.size.y)};
		m_lgxWindow->SetDragRect(lgxRect);
	}

	void EditorRoot::Draw(int32 threadIndex)
	{
		LinaVG::StyleOptions opts;
		opts.color = Theme::GetDef().background1.AsLVG4();
		LinaVG::DrawRect(threadIndex, m_topRect.pos.AsLVG(), m_topRect.GetEnd().AsLVG(), opts, 0.0f, m_drawOrder);

		m_linaLogo->Draw(threadIndex);
		m_fm->Draw(threadIndex);
		m_windowButtons->Draw(threadIndex);
	}

	bool EditorRoot::OnMouse(uint32 button, LinaGX::InputAction act)
	{
		if (button == LINAGX_MOUSE_0 && act == LinaGX::InputAction::Repeated && m_dragRect.IsPointInside(m_lgxWindow->GetMousePosition()))
		{
			if (m_lgxWindow->GetIsMaximized())
				m_lgxWindow->Restore();
			else
				m_lgxWindow->Maximize();
			return true;
		}

		return Widget::OnMouse(button, act);
	}
} // namespace Lina::Editor
