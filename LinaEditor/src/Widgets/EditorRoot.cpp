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
#include "Editor/Widgets/Panel/Panel.hpp"
#include "Editor/Editor.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/Primitives/ShapeRect.hpp"
#include "Core/GUI/Widgets/Compound/Popup.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/WidgetUtility.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Platform/PlatformProcess.hpp"
#include "Common/System/System.hpp"
#include "Common/Math/Math.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina::Editor
{
	void EditorRoot::Construct()
	{
		Editor* editor = m_system->CastSubsystem<Editor>(SubsystemType::Editor);

		m_titleImage		 = m_resourceManager->GetResource<Texture>("Resources/Editor/Textures/LinaLogoTitleHorizontal.png"_hs);
		const String tooltip = "Lina Engine v." + TO_STRING(LINA_MAJOR) + "." + TO_STRING(LINA_MINOR) + "." + TO_STRING(LINA_PATCH) + " - b: " + TO_STRING(LINA_BUILD);
		SetTooltip(tooltip);

		GetProps().direction = DirectionOrientation::Vertical;

		GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		SetAlignedPos(Vector2::Zero);
		SetAlignedSize(Vector2::One);

		DirectionalLayout* titleBar = m_manager->Allocate<DirectionalLayout>("TitleBar");

		titleBar->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		titleBar->GetProps().direction = DirectionOrientation::Vertical;
		titleBar->SetAlignedPosX(0.0f);
		titleBar->SetAlignedSizeX(1.0f);
		titleBar->SetFixedSizeY(Theme::GetDef().baseItemHeight * 3.0f);
		titleBar->GetProps().backgroundStyle	  = BackgroundStyle::CentralGradient;
		titleBar->GetProps().colorBackgroundStart = Theme::GetDef().background0;
		titleBar->GetProps().colorBackgroundEnd	  = Theme::GetDef().accentPrimary0;
		AddChild(titleBar);

		DirectionalLayout* layout1 = m_manager->Allocate<DirectionalLayout>("Layout1");
		layout1->GetFlags().Set(WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y | WF_POS_ALIGN_X);
		layout1->SetAlignedSizeX(1.0f);
		layout1->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		layout1->SetAlignedPosX(0.0f);
		layout1->GetChildMargins().left = Theme::GetDef().baseIndent;
		titleBar->AddChild(layout1);

		DirectionalLayout* layout2 = m_manager->Allocate<DirectionalLayout>("Layout2");
		layout2->GetFlags().Set(WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y | WF_POS_ALIGN_X);
		layout2->SetAlignedSizeX(1.0f);
		layout2->SetAlignedSizeY(0.0f);
		layout2->SetAlignedPosX(0.0f);
		titleBar->AddChild(layout2);

		DirectionalLayout* projectName = m_manager->Allocate<DirectionalLayout>("Project Name");
		projectName->GetFlags().Set(WF_SIZE_X_MAX_CHILDREN | WF_USE_FIXED_SIZE_Y | WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
		projectName->SetAlignedPos(Vector2(1.0f, 0.5f));
		projectName->SetPosAlignmentSourceX(PosAlignmentSource::End);
		projectName->SetPosAlignmentSourceY(PosAlignmentSource::Center);
		projectName->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		projectName->GetChildMargins()				 = {.left = Theme::GetDef().baseIndent, .right = Theme::GetDef().baseIndent};
		projectName->GetProps().backgroundStyle		 = BackgroundStyle::Default;
		projectName->GetProps().colorBackgroundStart = Theme::GetDef().background0;
		projectName->GetProps().colorBackgroundEnd	 = Theme::GetDef().background3;
		projectName->GetProps().outlineThickness	 = Theme::GetDef().baseOutlineThickness;
		projectName->GetProps().colorOutline		 = Theme::GetDef().black;
		projectName->GetProps().rounding			 = Theme::GetDef().baseRounding;
		layout2->AddChild(projectName);

		Text* projectNameText = m_manager->Allocate<Text>("ProjectNameText");
		projectNameText->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
		projectNameText->SetAlignedPos(Vector2(0.5f, 0.5f));
		projectNameText->SetPosAlignmentSourceX(PosAlignmentSource::Center);
		projectNameText->SetPosAlignmentSourceY(PosAlignmentSource::Center);
		projectNameText->GetProps().text = Locale::GetStr(LocaleStr::NoProject);
		projectName->AddChild(projectNameText);

		FileMenu* fm = m_manager->Allocate<FileMenu>("FileMenu");
		fm->GetFlags().Set(WF_SIZE_ALIGN_Y | WF_POS_ALIGN_Y);
		fm->SetAlignedPosY(0.0f);
		fm->SetAlignedSizeY(1.0f);

		fm->GetFileMenuProps().buttons = {
			Locale::GetStr(LocaleStr::File),
			Locale::GetStr(LocaleStr::Edit),
			Locale::GetStr(LocaleStr::View),
			Locale::GetStr(LocaleStr::Panels),
			Locale::GetStr(LocaleStr::About),
		};

		fm->SetListener(this);
		layout1->AddChild(fm);

		DirectionalLayout* wb = CommonWidgets::BuildWindowButtons(this);
		wb->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y | WF_USE_FIXED_SIZE_X | WF_POS_ALIGN_X);
		wb->SetPosAlignmentSourceX(PosAlignmentSource::End);
		wb->SetAlignedPos(Vector2(1.0f, 0.0f));
		wb->SetAlignedSizeY(1.0f);
		wb->SetFixedSizeX(Theme::GetDef().baseItemHeight * 6.0f);
		layout1->AddChild(wb);

		Widget* panelArea = m_manager->Allocate<Widget>("Editor Area");
		panelArea->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		panelArea->SetAlignedPosX(0.0f);
		panelArea->SetAlignedSize(Vector2(1.0f, 0.0f));
		AddChild(panelArea);

		m_panelArea = panelArea;

		DirectionalLayout::Construct();

		m_fileMenu		  = fm;
		m_windowButtons	  = wb;
		m_projectNameText = projectNameText;
		m_titleBar		  = titleBar;
	}

	void EditorRoot::Tick(float delta)
	{
		const Color targetColor					  = m_lgxWindow->HasFocus() ? Theme::GetDef().accentPrimary0 : Theme::GetDef().background2;
		m_titleBar->GetProps().colorBackgroundEnd = Math::Lerp(m_titleBar->GetProps().colorBackgroundEnd, targetColor, delta * COLOR_SPEED);

		DirectionalLayout::Tick(delta);

		m_dragRect				  = Rect(Vector2(m_fileMenu->GetRect().GetEnd().x, 0.0f), Vector2(m_windowButtons->GetPos().x - m_fileMenu->GetRect().GetEnd().x, m_fileMenu->GetParent()->GetSizeY()));
		LinaGX::LGXRectui lgxRect = {};
		lgxRect.pos				  = LinaGX::LGXVector2ui{static_cast<uint32>(m_dragRect.pos.x), static_cast<uint32>(m_dragRect.pos.y)};
		lgxRect.size			  = LinaGX::LGXVector2ui{static_cast<uint32>(m_dragRect.size.x), static_cast<uint32>(m_dragRect.size.y)};
		m_lgxWindow->SetDragRect(lgxRect);
	}

	void EditorRoot::Draw(int32 threadIndex)
	{
		DirectionalLayout::Draw(threadIndex);

		const Vector2 monitor = GetMonitorSize();
		const Vector2 size	  = Vector2(monitor.x * 0.15f, Theme::GetDef().baseItemHeight * 2.25f);
		const Vector2 center  = Vector2(GetRect().GetCenter().x, size.y * 0.5f);

		LinaVG::StyleOptions opts;
		opts.color					  = Theme::GetDef().background0.AsLVG4();
		opts.outlineOptions.thickness = Theme::GetDef().baseOutlineThickness * 2.0f;
		opts.outlineOptions.color	  = Theme::GetDef().black.AsLVG4();

		Vector<LinaVG::Vec2> points;
		points.push_back({center.x - size.x * 0.5f, center.y - size.y * 0.5f});
		points.push_back({center.x + size.x * 0.5f, center.y - size.y * 0.5f});
		points.push_back({center.x + size.x * 0.5f - size.x * 0.025f, center.y + size.y * 0.5f});
		points.push_back({center.x - size.x * 0.5f + size.x * 0.025f, center.y + size.y * 0.5f});

		LinaVG::DrawConvex(threadIndex, points.data(), static_cast<int>(points.size()), opts, 0.0f, m_drawOrder + 1);

		const float	  imageY	= size.y * 0.5f;
		const float	  imageX	= imageY * (m_titleImage->GetSizeF().x / m_titleImage->GetSizeF().y);
		const Vector2 imageSize = Vector2(imageX, imageY);
		LinaVG::DrawImage(threadIndex, m_titleImage->GetSID(), center.AsLVG(), imageSize.AsLVG(), Color::White.AsLVG4(), 0.0f, m_drawOrder + 1);

		const Rect	  imgRect = Rect(Vector2(center - imageSize * 0.5f), imageSize);
		const Vector2 mp	  = Vector2(static_cast<float>(m_lgxWindow->GetMousePosition().x), static_cast<float>(m_lgxWindow->GetMousePosition().y));
		if (imgRect.IsPointInside(mp))
			Widget::DrawTooltip(threadIndex);

		WidgetUtility::DrawDropShadow(threadIndex, points[3], points[2], m_drawOrder + 1, Theme::GetDef().black, 12);
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

	void EditorRoot::SetProjectName(const String& name)
	{
		m_projectNameText->GetProps().text = name;
		m_projectNameText->Initialize();
	}

	bool EditorRoot::OnItemClicked(StringID sid)
	{
		Editor* editor = m_system->CastSubsystem<Editor>(SubsystemType::Editor);

		if (sid == TO_SID(Locale::GetStr(LocaleStr::NewProject)))
		{
			editor->OpenPopupProjectSelector(true, true);
			return true;
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::LoadProject)))
		{
			editor->OpenPopupProjectSelector(true, false);
			return true;
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::SaveProject)))
		{
			editor->SaveProjectChanges();
			return true;
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::NewWorld)))
		{
			return true;
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::LoadWorld)))
		{
			return true;
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::SaveWorld)))
		{

			return true;
		}
		if (sid == TO_SID(Locale::GetStr(LocaleStr::SaveWorldAs)))
		{

			return true;
		}
		if (sid == TO_SID(Locale::GetStr(LocaleStr::Exit)))
		{
			editor->RequestExit();
			return true;
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::Resources)))
		{
			editor->OpenPanel(PanelType::Resources, 0, this);
			return true;
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::Entities)))
		{
			editor->OpenPanel(PanelType::Entities, 0, this);
			return true;
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::World)))
		{
			return true;
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::Performance)))
		{
			return true;
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::Website)))
		{
			PlatformProcess::OpenURL("https://inanevin.com");
			return true;
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::Github)))
		{
			PlatformProcess::OpenURL("https://github.com/inanevin");
			return true;
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::More)))
		{
			return true;
		}

		return false;
	}

	bool EditorRoot::IsItemDisabled(StringID sid)
	{
		Editor* editor = m_system->CastSubsystem<Editor>(SubsystemType::Editor);

		if (sid == TO_SID(Locale::GetStr(LocaleStr::SaveProject)))
			return editor->GetProjectData() == nullptr;

		if (sid == TO_SID(Locale::GetStr(LocaleStr::SaveWorld)) || sid == TO_SID(Locale::GetStr(LocaleStr::SaveWorldAs)))
			return editor->GetCurrentWorld() == nullptr;

		return false;
	}

	void EditorRoot::OnGetItemData(StringID sid, Vector<FileMenuItem::Data>& outData)
	{
		if (sid == TO_SID(Locale::GetStr(LocaleStr::SaveProject)))
		{
			outData = {
				FileMenuItem::Data{.text = "This is my test"},
				FileMenuItem::Data{.text = "This is my test too", .altText = "CTRL + K"},
			};
			return;
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::File)))
		{
			outData = {
				FileMenuItem::Data{.text = Locale::GetStr(LocaleStr::NewProject)},
				FileMenuItem::Data{
					.text = Locale::GetStr(LocaleStr::LoadProject),
				},
				FileMenuItem::Data{.text = Locale::GetStr(LocaleStr::SaveProject)},
				FileMenuItem::Data{.isDivider = true},
				FileMenuItem::Data{.text = Locale::GetStr(LocaleStr::NewWorld)},
				FileMenuItem::Data{.text = Locale::GetStr(LocaleStr::LoadWorld)},
				FileMenuItem::Data{.text = Locale::GetStr(LocaleStr::SaveWorld), .altText = "CTRL + S"},
				FileMenuItem::Data{.text = Locale::GetStr(LocaleStr::SaveWorldAs), .altText = "CTRL + SHIFT + S"},
				FileMenuItem::Data{.isDivider = true},
				FileMenuItem::Data{.text = Locale::GetStr(LocaleStr::Exit)},
			};
			return;
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::Panels)))
		{
			outData = {
				FileMenuItem::Data{.text = Locale::GetStr(LocaleStr::Entities)},
				FileMenuItem::Data{.text = Locale::GetStr(LocaleStr::World)},
				FileMenuItem::Data{.text = Locale::GetStr(LocaleStr::Resources)},
				FileMenuItem::Data{.text = Locale::GetStr(LocaleStr::Performance)},
			};
			return;
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::About)))
		{
			outData = {
				FileMenuItem::Data{.text = Locale::GetStr(LocaleStr::Website)},
				FileMenuItem::Data{.text = Locale::GetStr(LocaleStr::Github)},
				FileMenuItem::Data{.text = Locale::GetStr(LocaleStr::More)},
			};
			return;
		}
	}
} // namespace Lina::Editor
