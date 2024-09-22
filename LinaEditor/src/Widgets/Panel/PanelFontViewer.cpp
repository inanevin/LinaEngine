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

#include "Editor/Widgets/Panel/PanelFontViewer.hpp"
#include "Editor/Editor.hpp"
#include "Editor/EditorLocale.hpp"
#include "Editor/Widgets/Panel/PanelResourceBrowser.hpp"
#include "Editor/Widgets/Compound/ResourceDirectoryBrowser.hpp"
#include "Core/Meta/ProjectData.hpp"
#include "Core/Platform/PlatformProcess.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/GUI/Widgets/WidgetUtility.hpp"
#include "Core/GUI/Widgets/Layout/ScrollArea.hpp"
#include "Core/GUI/Widgets/Layout/Popup.hpp"
#include "Core/GUI/Widgets/Primitives/InputField.hpp"
#include "Core/GUI/Widgets/Primitives/Dropdown.hpp"
#include "Core/GUI/Widgets/Primitives/Button.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Editor/Widgets/CommonWidgets.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/Layout/FoldLayout.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Common/FileSystem/FileSystem.hpp"

namespace Lina::Editor
{
	void PanelFontViewer::Construct()
	{
		m_editor = Editor::Get();

		DirectionalLayout* horizontal = m_manager->Allocate<DirectionalLayout>("Horizontal");
		horizontal->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		horizontal->SetAlignedPos(Vector2::Zero);
		horizontal->SetAlignedSize(Vector2::One);
		horizontal->GetProps().direction = DirectionOrientation::Horizontal;
		horizontal->GetProps().mode		 = DirectionalLayout::Mode::Bordered;
		AddChild(horizontal);

		Widget* fontBG = m_manager->Allocate<Widget>("FontBG");
		fontBG->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		fontBG->SetAlignedPos(Vector2::Zero);
		fontBG->SetAlignedSize(Vector2(0.7f, 1.0f));
		fontBG->GetWidgetProps().childMargins = TBLR::Eq(Theme::GetDef().baseIndent);
		horizontal->AddChild(fontBG);

		Widget* fontPanel = m_manager->Allocate<Widget>("FontPanel");
		fontPanel->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		fontPanel->SetAlignedPos(Vector2::Zero);
		fontPanel->SetAlignedSize(Vector2::One);
		fontPanel->GetWidgetProps().drawBackground	 = true;
		fontPanel->GetWidgetProps().colorBackground	 = Theme::GetDef().background0;
		fontPanel->GetWidgetProps().outlineThickness = 0.0f;
		fontPanel->GetWidgetProps().rounding		 = 0.0f;
		fontPanel->GetWidgetProps().childMargins	 = TBLR::Eq(Theme::GetDef().baseIndent);
		fontBG->AddChild(fontPanel);

		Text* fontDisplay = m_manager->Allocate<Text>("Font Display");
		fontDisplay->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
		fontDisplay->SetAlignedPos(Vector2::One * 0.5f);
		fontDisplay->SetAnchorX(Anchor::Center);
		fontDisplay->SetAnchorY(Anchor::Center);
		fontDisplay->GetProps().text = "ABCDEFGHIJKLMNabcdefghijklmn0123456789";
		fontPanel->AddChild(fontDisplay);

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

		m_fontPanel	  = fontPanel;
		m_fontDisplay = fontDisplay;
		m_inspector	  = inspector;
	}

	void PanelFontViewer::Initialize()
	{
		if (m_font != nullptr)
			return;

		if (!m_editor->GetProjectManager().GetProjectData()->GetResourceRoot().FindResource(m_subData))
		{
			Text* text = m_manager->Allocate<Text>("Info");
			text->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
			text->SetAlignedPos(Vector2(0.5f, 0.5f));
			text->SetAnchorX(Anchor::Center);
			text->SetAnchorY(Anchor::Center);
			text->GetProps().text = Locale::GetStr(LocaleStr::ThisResourceNoLongerExists);
			m_fontPanel->AddChild(text);
			return;
		}

		ResourceDef def = {
			.id	 = m_subData,
			.tid = GetTypeID<Font>(),
		};
		m_editor->GetResourceManagerV2().LoadResourcesFromFile(m_editor, m_editor->GetProjectManager().GetProjectData(), {def}, 0);
		m_editor->GetResourceManagerV2().WaitForAll();
		m_font						   = m_editor->GetResourceManagerV2().GetResource<Font>(def.id);
		m_fontDisplay->GetProps().font = def.id;
		GetWidgetProps().debugName	   = "Font: " + m_font->GetName();

		m_fontName = m_font->GetName();

		FoldLayout* foldGeneral = CommonWidgets::BuildFoldTitle(this, Locale::GetStr(LocaleStr::General), &m_generalFold);
		FoldLayout* foldFont	= CommonWidgets::BuildFoldTitle(this, "Font", &m_generalFold);
		m_inspector->AddChild(foldGeneral);
		m_inspector->AddChild(foldFont);

		CommonWidgets::BuildClassReflection(foldGeneral, this, ReflectionSystem::Get().Resolve<PanelFontViewer>(), [this](const MetaType& meta, FieldBase* field) {

		});

		CommonWidgets::BuildClassReflection(foldFont, &m_font->GetMeta(), ReflectionSystem::Get().Resolve<Font::Metadata>(), [this](const MetaType& meta, FieldBase* field) { SetRuntimeDirty(true); });

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

		Widget* buttonLayout1 = buildButtonLayout();
		Widget* buttonLayout2 = buildButtonLayout();
		foldGeneral->AddChild(buttonLayout1);
		foldGeneral->AddChild(buttonLayout2);
		static_cast<DirectionalLayout*>(buttonLayout1)->GetProps().mode = DirectionalLayout::Mode::EqualSizes;
		static_cast<DirectionalLayout*>(buttonLayout2)->GetProps().mode = DirectionalLayout::Mode::EqualSizes;

		Button* importButton = WidgetUtility::BuildIconTextButton(this, ICON_IMPORT, Locale::GetStr(LocaleStr::Import));
		importButton->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y);
		importButton->SetAlignedPosY(0.0f);
		importButton->SetAlignedSizeY(1.0f);
		importButton->SetFixedSizeX(Theme::GetDef().baseItemHeight * 4);
		importButton->GetProps().onClicked = [this]() {
			const Vector<String> paths = PlatformProcess::OpenDialog({
				.title				   = Locale::GetStr(LocaleStr::Import),
				.primaryButton		   = Locale::GetStr(LocaleStr::Import),
				.extensionsDescription = "",
				.extensions			   = {"ttf", "otf"},
				.mode				   = PlatformProcess::DialogMode::SelectFile,
			});
			if (paths.empty())
				return;

			RegenFont(paths.front());
			SetRuntimeDirty(true);
		};
		buttonLayout1->AddChild(importButton);

		Button* reimportButton = WidgetUtility::BuildIconTextButton(this, ICON_ROTATE, Locale::GetStr(LocaleStr::ReImport));
		reimportButton->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y);
		reimportButton->SetAlignedPosY(0.0f);
		reimportButton->SetAlignedSizeY(1.0f);
		reimportButton->SetFixedSizeX(Theme::GetDef().baseItemHeight * 4);
		reimportButton->GetProps().onClicked = [this, reimportButton]() {
			const String path = m_font->GetPath();
			if (!FileSystem::FileOrPathExists(path))
			{
				CommonWidgets::ThrowInfoTooltip(Locale::GetStr(LocaleStr::FileNotFound), LogLevel::Error, 3.0f, reimportButton);
				return;
			}

			RegenFont(path);
			SetRuntimeDirty(true);
		};

		buttonLayout1->AddChild(reimportButton);

		Button* save = WidgetUtility::BuildIconTextButton(this, ICON_SAVE, Locale::GetStr(LocaleStr::Save));
		save->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y);
		save->SetAlignedPosY(0.0f);
		save->SetAlignedSizeY(1.0f);
		save->SetFixedSizeX(Theme::GetDef().baseItemHeight * 4);
		save->GetProps().onClicked = [this]() {
			if (m_containsRuntimeChanges)
			{
				m_resourceManager->SaveResource(m_editor->GetProjectManager().GetProjectData(), m_font);
				m_editor->GetResourcePipeline().GenerateThumbnailForResource(m_editor->GetProjectManager().GetProjectData()->GetResourceRoot().FindResource(m_font->GetID()), m_font);
				DockArea* outDockArea = nullptr;
				Panel*	  p			  = m_editor->GetWindowPanelManager().FindPanelOfType(PanelType::ResourceBrowser, 0, outDockArea);
				if (p)
					static_cast<PanelResourceBrowser*>(p)->GetBrowser()->RefreshDirectory();
				SetRuntimeDirty(false);
			}
		};

		m_saveButton = save;
		buttonLayout2->AddChild(save);
	}

	void PanelFontViewer::Destruct()
	{
		Panel::Destruct();
		if (m_font == nullptr)
			return;
		m_editor->GetResourceManagerV2().UnloadResources({m_font});
	}

	void PanelFontViewer::SetRuntimeDirty(bool isDirty)
	{
		m_containsRuntimeChanges = isDirty;
		Text* txt				 = GetWidgetOfType<Text>(m_saveButton);
		if (isDirty)
			txt->GetProps().text = Locale::GetStr(LocaleStr::Save) + " *";
		else
			txt->GetProps().text = Locale::GetStr(LocaleStr::Save);
		txt->CalculateTextSize();
	}

	void PanelFontViewer::RegenFont(const String& path)
	{

		// m_texture->DestroyHW();
		//
		// if(path.empty())
		//     m_texture->LoadFromBuffer(m_textureBuffer.pixels, m_textureBuffer.width, m_textureBuffer.height, m_textureBuffer.bytesPerPixel);
		// else
		// {
		//     m_texture->LoadFromFile(path);
		//     m_texture->SetPath(path);
		// }
		//
		// m_texture->GenerateHW();
		// m_texture->AddToUploadQueue(m_editor->GetEditorRenderer().GetUploadQueue(), false);
		// m_editor->GetEditorRenderer().MarkBindlessDirty();
		//
		// m_formatDropdown->GetText()->GetProps().text = ReflectionSystem::Get().Meta<LinaGX::Format>().GetProperty<String>(static_cast<StringID>(m_texture->GetMeta().format));
		// m_formatDropdown->GetText()->CalculateTextSize();
	}

	void PanelFontViewer::SaveLayoutToStream(OStream& stream)
	{
	}

	void PanelFontViewer::LoadLayoutFromStream(IStream& stream)
	{
	}
} // namespace Lina::Editor
