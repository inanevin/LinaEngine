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
#include "Editor/Widgets/Docking/DockArea.hpp"
#include "Editor/CommonEditor.hpp"
#include "Editor/EditorLocale.hpp"
#include "Editor/Widgets/CommonWidgets.hpp"
#include "Core/GUI/Widgets/Primitives/Icon.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina::Editor
{
	void EditorRoot::Construct()
	{
		GetProps().direction = DirectionOrientation::Vertical;
		GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		SetAlignedPos(Vector2::Zero);
		SetAlignedSize(Vector2::One);

		DirectionalLayout* titleBar = Allocate<DirectionalLayout>("TitleBar");
		titleBar->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		titleBar->SetAlignedPosX(0.0f);
		titleBar->SetAlignedSizeX(1.0f);
		titleBar->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		titleBar->GetChildMargins().left	 = Theme::GetDef().baseIndent;
		titleBar->GetProps().drawBackground	 = true;
		titleBar->GetProps().colorBackground = Theme::GetDef().background1;
		titleBar->SetChildPadding(Theme::GetDef().baseIndent);
		AddChild(titleBar);

		Widget* editorArea = Allocate<Widget>("Editor Area");
		editorArea->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		editorArea->SetAlignedPosX(0.0f);
		editorArea->SetAlignedSize(Vector2(1.0f, 0.0f));
		AddChild(editorArea);

		Icon* linaLogo					= Allocate<Icon>("LinaLogo");
		linaLogo->GetProps().icon		= ICON_LINA_LOGO;
		linaLogo->GetProps().colorStart = linaLogo->GetProps().colorEnd = Theme::GetDef().accentPrimary0;
		linaLogo->GetFlags().Set(WF_POS_ALIGN_Y);
		linaLogo->SetAlignedPosY(0.5f);
		linaLogo->SetPosAlignmentSourceY(PosAlignmentSource::Center);
		titleBar->AddChild(linaLogo);

		FileMenu* fm = Allocate<FileMenu>("FileMenu");
		fm->GetFlags().Set(WF_SIZE_ALIGN_Y | WF_POS_ALIGN_Y);
		fm->SetAlignedPosY(0.0f);
		fm->SetAlignedSizeY(1.0f);

		fm->GetFileMenuProps().buttonProps.outlineThickness	 = 0.0f;
		fm->GetFileMenuProps().buttonProps.rounding			 = 0.0f;
		fm->GetFileMenuProps().buttonProps.colorDefaultStart = Theme::GetDef().background1;
		fm->GetFileMenuProps().buttonProps.colorDefaultEnd	 = Theme::GetDef().background1;
		fm->GetFileMenuProps().buttonProps.colorPressed		 = Theme::GetDef().background2;
		fm->GetFileMenuProps().buttonProps.colorHovered		 = Theme::GetDef().background3;
		fm->GetFileMenuProps().buttons						 = {Locale::GetStr(LocaleStr::File), Locale::GetStr(LocaleStr::Edit), Locale::GetStr(LocaleStr::View), Locale::GetStr(LocaleStr::Panels), Locale::GetStr(LocaleStr::About)};
		fm->SetListener(this);
		titleBar->AddChild(fm);

		DirectionalLayout* wb = CommonWidgets::BuildWindowButtons(this);
		wb->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y | WF_USE_FIXED_SIZE_X | WF_POS_ALIGN_X);
		wb->SetPosAlignmentSourceX(PosAlignmentSource::End);
		wb->SetAlignedPos(Vector2(1.0f, 0.0f));
		wb->SetAlignedSizeY(1.0f);
		wb->SetFixedSizeX(Theme::GetDef().baseItemHeight * 6.0f);
		titleBar->AddChild(wb);

		DockArea* dummyDock = Allocate<DockArea>("DummyDock");
		dummyDock->SetAlignRect(Rect(Vector2::Zero, Vector2::One));
		dummyDock->AddChild(Allocate<Widget>("Widget"));
		editorArea->AddChild(dummyDock);

		DirectionalLayout::Construct();

		m_fileMenu		= fm;
		m_windowButtons = wb;
	}

	void EditorRoot::Tick(float delta)
	{
		DirectionalLayout::Tick(delta);

		m_dragRect				  = Rect(Vector2(m_fileMenu->GetRect().GetEnd().x, 0.0f), Vector2(m_windowButtons->GetPos().x - m_fileMenu->GetRect().GetEnd().x, m_fileMenu->GetParent()->GetSizeY()));
		LinaGX::LGXRectui lgxRect = {};
		lgxRect.pos				  = LinaGX::LGXVector2ui{static_cast<uint32>(m_dragRect.pos.x), static_cast<uint32>(m_dragRect.pos.y)};
		lgxRect.size			  = LinaGX::LGXVector2ui{static_cast<uint32>(m_dragRect.size.x), static_cast<uint32>(m_dragRect.size.y)};
		m_lgxWindow->SetDragRect(lgxRect);

		return;
		//
		// const float defaultHeight	  = Theme::GetDef().baseItemHeight;
		// const float indent			  = Theme::GetDef().baseIndent;
		// const float windowButtonWidth = defaultHeight * 2.0f;
		//
		// SetPos(Vector2::Zero);
		// SetSize(Vector2(static_cast<float>(m_lgxWindow->GetSize().x), static_cast<float>(m_lgxWindow->GetSize().y)));
		//
		// m_topRect.pos  = Vector2::Zero;
		// m_topRect.size = Vector2(GetSizeX(), defaultHeight);
		//
		//
		// m_linaLogo->SetPosX(indent);
		// m_linaLogo->SetPosY(m_topRect.GetCenter().y - m_linaLogo->GetHalfSizeY());
		//
		// m_fm->SetPosX(indent * 2.0f + m_linaLogo->GetSizeX());
		// m_fm->SetPosY(0.0f);
		// m_fm->SetSizeX(GetSizeX());
		// m_fm->SetSizeY(defaultHeight);
		//
		// // Drag rect.
		//
		//
		//
		//
		//

		// m_dummyDock->SetPos(Vector2(0.0f, defaultHeight));
		// m_dummyDock->SetSize(Vector2(GetSizeX(), GetSizeY() - defaultHeight));
	}

	void EditorRoot::Draw(int32 threadIndex)
	{
		DirectionalLayout::Draw(threadIndex);
		return;

		LinaVG::StyleOptions opts;
		// opts.color = Theme::GetDef().background1.AsLVG4();
		// LinaVG::DrawRect(threadIndex, m_topRect.pos.AsLVG(), m_topRect.GetEnd().AsLVG(), opts, 0.0f, m_drawOrder);
		// Widget::Draw(threadIndex);
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

	void EditorRoot::OnPopupCreated(Popup* popup, StringID sid)
	{
	}
} // namespace Lina::Editor
