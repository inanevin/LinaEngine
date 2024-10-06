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

		Widget* resBG = m_manager->Allocate<Widget>("ResBG");
		resBG->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		resBG->SetAlignedPos(Vector2::Zero);
		resBG->SetAlignedSize(Vector2(0.7f, 1.0f));
		resBG->GetWidgetProps().childMargins	 = TBLR::Eq(Theme::GetDef().baseIndent);
		resBG->GetWidgetProps().drawBackground	 = true;
		resBG->GetWidgetProps().colorBackground	 = Theme::GetDef().background0;
		resBG->GetWidgetProps().outlineThickness = 0.0f;
		resBG->GetWidgetProps().rounding		 = 0.0f;
		resBG->GetWidgetProps().childMargins	 = TBLR::Eq(Theme::GetDef().baseIndent);
		horizontal->AddChild(resBG);

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

		m_resourceBuffer.Destroy();

		ResourceDef def = {.id = m_resource->GetID(), .tid = m_resource->GetTID(), .name = m_resource->GetName()};
		m_editor->GetResourceManagerV2().UnloadResources({m_resource});
		m_editor->GetEditorRenderer().OnResourcesUnloaded({def});
		m_resource = nullptr;
	}

	void PanelResourceViewer::Initialize()
	{
		if (m_resource != nullptr)
			return;

		const bool dontExists = m_editor->GetProjectManager().GetProjectData() == nullptr || m_editor->GetProjectManager().GetProjectData()->GetResourceRoot().FindResource(m_subData);

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

		ResourceDef def = {
			.id	 = m_subData,
			.tid = m_resourceTID,
		};

		Resource* r = m_editor->GetResourceManagerV2().GetIfExists(m_resourceTID, m_subData);

		// Can not modify the resource if its alive in editor.
		if (r != nullptr)
			m_previewOnly = true;
		else
		{
			HashSet<Resource*> resources = m_editor->GetResourceManagerV2().LoadResourcesFromProject(m_editor, m_editor->GetProjectManager().GetProjectData(), {def}, NULL);
			r							 = *resources.begin();
			m_editor->GetEditorRenderer().OnResourcesLoaded({r});
		}

		m_resource				   = r;
		GetWidgetProps().debugName = m_resource->GetName();
		StoreBuffer();
		BuildInspector();
		Panel::Initialize();
	}

	void PanelResourceViewer::StoreBuffer()
	{
		if (m_previewOnly)
			return;

		OStream stream;
		m_resource->SaveToStream(stream);
		m_resourceBuffer.Destroy();
		m_resourceBuffer.Create(stream);
		stream.Destroy();
	}

	void PanelResourceViewer::BuildInspector()
	{
		m_inspector->DeallocAllChildren();
		m_inspector->RemoveAllChildren();

		auto buildButtonLayout = [this]() -> Widget* {
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
		};

		// General reflection.
		{
			FoldLayout* foldGeneral = CommonWidgets::BuildFoldTitle(m_inspector, Locale::GetStr(LocaleStr::General), &m_foldGeneral);
			m_inspector->AddChild(foldGeneral);

			CommonWidgets::BuildClassReflection(foldGeneral, this, ReflectionSystem::Get().Resolve(m_panelTID), [this](const MetaType& meta, FieldBase* field) { OnGeneralMetaChanged(meta, field); });

			Widget* buttonLayout1 = buildButtonLayout();
			foldGeneral->AddChild(buttonLayout1);

			m_revertButton = BuildButton(Locale::GetStr(LocaleStr::Revert), ICON_REVERT);
			m_saveButton   = BuildButton(Locale::GetStr(LocaleStr::Save), ICON_SAVE);

			buttonLayout1->AddChild(m_saveButton);
			buttonLayout1->AddChild(m_revertButton);

			m_revertButton->GetProps().onClicked = []() { RevertResource(); };

			m_saveButton->GetProps().onClicked = []() { SaveResource(); };

			if (m_previewOnly)
				DisableRecursively(foldGeneral);
		}

		// Resource reflection
		{
			FoldLayout* foldResource = CommonWidgets::BuildFoldTitle(m_inspector, Locale::GetStr(LocaleStr::Resource), &m_foldResource);
			m_inspector->AddChild(foldResource);

			// Build resource reflection.
			CommonWidgets::BuildClassReflection(foldResource, m_resource, ReflectionSystem::Get().Resolve(m_resourceTID), [this](const MetaType& meta, FieldBase* field) { OnResourceMetaChanged(meta, field); });

			if (m_previewOnly)
				DisableRecursively(foldResource);
		}
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
		m_runtimeDirty = runtimeDirty;
		Text* txt	   = GetWidgetOfType<Text>(m_saveButton);
		txt->UpdateTextAndCalcSize(runtimeDirty ? (Locale::GetStr(LocaleStr::Save) + " *") : Locale::GetStr(LocaleStr::Save));
		m_revertButton->SetIsDisabled(!runtimeDirty);
		m_saveButton->SetIsDisabled(runtimeDirty);
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

		Taskflow tf;
		tf.emplace([this]() {
			// Save resource
			ResourceDirectory* dir = m_editor->GetProjectManager().GetProjectData()->GetResourceRoot().FindResource(m_resource->GetID());
			m_resourceManager->SaveResource(m_editor->GetProjectManager().GetProjectData(), m_resource);

			// Restore buffer to latest.
			StoreBuffer();

			// Generate new thumbnail.
			dir->thumbnailBuffer.Destroy();
			m_editor->GetProjectManager().RemoveDirectoryThumbnails(dir);
			const LinaGX::TextureBuffer thumb = ThumbnailGenerator::GenerateThumbnailForResource(m_resource);
			ThumbnailGenerator::CreateThumbnailBuffer(dir->thumbnailBuffer, thumb);

			// Save project
			m_editor->GetProjectManager().SaveProjectChanges();

			m_editor->QueueTask([this, dir]() {
				// Gen & upload atlases, unlock GUI.
				Application::GetLGX()->Join();
				m_editor->GetProjectManager().GenerateMissingAtlasImages(dir);
				m_editor->GetAtlasManager().RefreshPoolAtlases();
				m_editor->GetWindowPanelManager().UnlockAllForegrounds();
				m_editor->GetProjectManager().NotifyProjectResourcesRefreshed();
				SetRuntimeDirty(false);
			});
		});

		m_editor->GetExecutor().RunMove(tf);
	}

	void PanelResourceViewer::RevertResource()
	{
		// Add GUI lock.
		Widget* lock = m_editor->GetWindowPanelManager().LockAllForegrounds(m_lgxWindow, Locale::GetStr(LocaleStr::WorkInProgressInAnotherWindow));
		Widget* pp	 = CommonWidgets::BuildGenericPopupProgress(lock, Locale::GetStr(LocaleStr::Reverting), true);
		lock->AddChild(pp);

		Taskflow tf;
		tf.emplace([this]() {
			IStream stream;
			stream.Create(m_resourceBuffer.GetRaw(), m_resourceBuffer.GetSize());
			m_resource->LoadFromStream(stream);
			stream.Destroy();

			m_editor->QueueTask([this, dir]() {
				// Gen & upload atlases, unlock GUI.
				Application::GetLGX()->Join();
				RegenGPU();
				m_editor->GetProjectManager().GenerateMissingAtlasImages(dir);
				m_editor->GetAtlasManager().RefreshPoolAtlases();
				m_editor->GetWindowPanelManager().UnlockAllForegrounds();
				m_editor->GetProjectManager().NotifyProjectResourcesRefreshed();
				SetRuntimeDirty(false);
				BuildInspector();
			});
		});

		m_editor->GetExecutor().RunMove(tf);
	}

} // namespace Lina::Editor
