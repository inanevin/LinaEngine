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

#include "Editor/Widgets/Panel/PanelResourceViewer.hpp"
#include "Editor/Editor.hpp"
#include "Editor/EditorLocale.hpp"
#include "Editor/Widgets/CommonWidgets.hpp"
#include "Editor/IO/ThumbnailGenerator.hpp"
#include "Core/Meta/ProjectData.hpp"
#include "Core/GUI/Widgets/WidgetUtility.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/Primitives/Button.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/Layout/ScrollArea.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/Resources/Resource.hpp"
#include "Core/GUI/Widgets/Layout/FoldLayout.hpp"
#include "Core/Application.hpp"
#include "Common/FileSystem/FileSystem.hpp"

namespace Lina::Editor
{
	void PanelResourceViewer::Construct()
	{
		m_editor = Editor::Get();

		DirectionalLayout* horizontal = m_manager->Allocate<DirectionalLayout>("Horizontal");
		horizontal->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		horizontal->SetAlignedPos(Vector2::Zero);
		horizontal->SetAlignedSize(Vector2::One);
		horizontal->GetProps().direction = DirectionOrientation::Horizontal;
		horizontal->GetProps().mode		 = DirectionalLayout::Mode::Bordered;
		AddChild(horizontal);

		Widget* bg = m_manager->Allocate<Widget>("BG");
		bg->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		bg->SetAlignedPos(Vector2::Zero);
		bg->SetAlignedSize(Vector2(0.7f, 1.0f));
		bg->GetWidgetProps().childMargins	  = TBLR::Eq(Theme::GetDef().baseIndent);
		bg->GetWidgetProps().drawBackground	  = true;
		bg->GetWidgetProps().colorBackground  = Theme::GetDef().background1;
		bg->GetWidgetProps().outlineThickness = 0.0f;
		bg->GetWidgetProps().rounding		  = 0.0f;
		bg->GetWidgetProps().childMargins	  = TBLR::Eq(Theme::GetDef().baseIndent);
		horizontal->AddChild(bg);

		Widget* resBG = m_manager->Allocate<Widget>("ResBG");
		resBG->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		resBG->SetAlignedPos(Vector2::Zero);
		resBG->SetAlignedSize(Vector2::One);
		resBG->GetWidgetProps().childMargins	 = TBLR::Eq(Theme::GetDef().baseIndent);
		resBG->GetWidgetProps().drawBackground	 = true;
		resBG->GetWidgetProps().colorBackground	 = Theme::GetDef().background0;
		resBG->GetWidgetProps().outlineThickness = 0.0f;
		resBG->GetWidgetProps().rounding		 = 0.0f;
		resBG->GetWidgetProps().childMargins	 = TBLR::Eq(Theme::GetDef().baseIndent);
		bg->AddChild(resBG);

		ScrollArea* scroll = m_manager->Allocate<ScrollArea>("Scroll");
		scroll->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		scroll->SetAlignedPos(Vector2(0.7f, 0.0f));
		scroll->SetAlignedSize(Vector2(0.3f, 1.0f));
		scroll->GetProps().direction = DirectionOrientation::Vertical;
		horizontal->AddChild(scroll);

		DirectionalLayout* inspector = m_manager->Allocate<DirectionalLayout>("Inspector");
		inspector->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		inspector->SetAlignedPos(Vector2::Zero);
		inspector->SetAlignedSize(Vector2::One);
		inspector->GetProps().direction				  = DirectionOrientation::Vertical;
		inspector->GetWidgetProps().childPadding	  = Theme::GetDef().baseIndentInner;
		inspector->GetWidgetProps().clipChildren	  = true;
		inspector->GetWidgetProps().childMargins.left = Theme::GetDef().baseBorderThickness;
		inspector->GetWidgetProps().childMargins.top  = Theme::GetDef().baseIndent;
		scroll->AddChild(inspector);

		m_resourceBG = resBG;
		m_inspector	 = inspector;
	}

	void PanelResourceViewer::Destruct()
	{
		Panel::Destruct();

		if (m_resource == nullptr)
			return;

		if (m_previewOnly)
			return;

		m_editor->GetResourceManagerV2().UnloadResources({m_resource});
		m_editor->GetEditorRenderer().MarkBindlessDirty();
		m_resource = nullptr;
	}

	void PanelResourceViewer::Initialize()
	{
		Panel::Initialize();

		if (m_resource != nullptr)
			return;

		if (m_editor->GetProjectManager().GetProjectData() == nullptr)
			return;

		ResourceDirectory* resDir	  = m_editor->GetProjectManager().GetProjectData()->GetResourceRoot().FindResourceDirectory(m_subData);
		const bool		   dontExists = m_editor->GetProjectManager().GetProjectData() == nullptr || !resDir;

		if (dontExists)
		{
			Text* text = m_manager->Allocate<Text>("Info");
			text->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
			text->SetAlignedPos(Vector2(0.5f, 0.5f));
			text->SetAnchorX(Anchor::Center);
			text->SetAnchorY(Anchor::Center);
			text->GetProps().text = Locale::GetStr(LocaleStr::ThisResourceNoLongerExists);
			m_resourceBG->AddChild(text);
			return;
		}

		Resource* r = m_editor->GetResourceManagerV2().GetIfExists(m_resourceTID, m_subData);

		// Can not modify the resource if its alive in editor.
		if (r != nullptr)
			m_previewOnly = true;
		else
		{
			m_editor->GetResourceManagerV2().LoadResourcesFromProject(m_editor->GetProjectManager().GetProjectData(), {m_subData}, NULL);
			m_editor->GetEditorRenderer().VerifyResources();
			r = m_editor->GetResourceManagerV2().GetIfExists(m_resourceTID, m_subData);
		}

		if (resDir->parent == m_editor->GetProjectManager().GetProjectData()->GetResourceRoot().GetChildByName(EDITOR_DEF_RESOURCES_FOLDER))
			m_previewOnly = true;

		m_resource = r;

		if (m_resource == nullptr)
			return;

		GetWidgetProps().debugName = m_resource->GetName();
	}

	Widget* PanelResourceViewer::BuildButtonLayout()
	{
		DirectionalLayout* layout = m_manager->Allocate<DirectionalLayout>();
		layout->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		layout->SetAlignedPosX(0.0f);
		layout->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		layout->SetAlignedSizeX(1.0f);
		layout->GetProps().mode						= DirectionalLayout::Mode::EqualSizes;
		layout->GetProps().direction				= DirectionOrientation::Horizontal;
		layout->GetWidgetProps().childMargins.left	= Theme::GetDef().baseIndent;
		layout->GetWidgetProps().childMargins.right = Theme::GetDef().baseIndent;
		layout->GetWidgetProps().childPadding		= Theme::GetDef().baseIndent;
		return layout;
	}

	Button* PanelResourceViewer::BuildButton(const String& title, const String& icon)
	{
		Button* button = WidgetUtility::BuildIconTextButton(this, icon, title);
		button->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y);
		button->SetAlignedPosY(0.0f);
		button->SetAlignedSizeY(1.0f);
		return button;
	}

	void PanelResourceViewer::SetRuntimeDirty(bool runtimeDirty)
	{
	}

	void PanelResourceViewer::DisableRecursively(Widget* parent)
	{
		for (Widget* c : parent->GetChildren())
		{
			c->SetIsDisabled(true);
			DisableRecursively(c);
		}
	}

	void PanelResourceViewer::SaveResource()
	{
		// Add GUI lock.
		Widget* lock = m_editor->GetWindowPanelManager().LockAllForegrounds(m_lgxWindow, Locale::GetStr(LocaleStr::WorkInProgressInAnotherWindow));
		Widget* pp	 = CommonWidgets::BuildGenericPopupProgress(lock, Locale::GetStr(LocaleStr::Saving), true);
		lock->AddChild(pp);

		ResourceDirectory* dir = m_editor->GetProjectManager().GetProjectData()->GetResourceRoot().FindResourceDirectory(m_resource->GetID());

		m_editor->AddFreeTask(
			[this, dir]() {
				// Save resource
				m_resource->SaveToFileAsBinary(m_editor->GetProjectManager().GetProjectData()->GetResourcePath(m_resource->GetID()));

				// Save project
				m_editor->GetProjectManager().SaveProjectChanges();
			},
			[this, dir]() {
				// m_editor->GetProjectManager().AddToThumbnailQueue(dir->resourceID);

				// Gen & upload atlases, unlock GUI.
				m_editor->GetWindowPanelManager().UnlockAllForegrounds();
			});
	}

	void PanelResourceViewer::ReimportResource()
	{
		// Add GUI lock.
		Widget* lock = m_editor->GetWindowPanelManager().LockAllForegrounds(m_lgxWindow, Locale::GetStr(LocaleStr::WorkInProgressInAnotherWindow));
		Widget* pp	 = CommonWidgets::BuildGenericPopupProgress(lock, Locale::GetStr(LocaleStr::Reimporting), true);
		lock->AddChild(pp);

		m_editor->AddFreeTask(
			[this]() {
				ResourceDirectory* dir = m_editor->GetProjectManager().GetProjectData()->GetResourceRoot().FindResourceDirectory(m_resource->GetID());

				if (dir->resourceType == ResourceType::ExternalSource)
				{
					const String path = FileSystem::GetFilePath(m_editor->GetProjectManager().GetProjectData()->GetPath()) + dir->sourcePathRelativeToProject;
					if (FileSystem::FileOrPathExists(path))
						m_resource->LoadFromFile(path);
				}
				else
					m_resource->LoadFromFile(m_editor->GetProjectManager().GetProjectData()->GetResourcePath(m_resource->GetID()));
			},
			[this]() {
				Application::GetLGX()->Join();
				// RegenHW();
				m_editor->GetWindowPanelManager().UnlockAllForegrounds();
			});
	}

} // namespace Lina::Editor
