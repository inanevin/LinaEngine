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

#include "Editor/Widgets/Panel/PanelTextureViewer.hpp"
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
#include "Core/GUI/Widgets/Layout/FoldLayout.hpp"
#include "Core/GUI/Widgets/Primitives/InputField.hpp"
#include "Core/GUI/Widgets/Primitives/Dropdown.hpp"
#include "Core/GUI/Widgets/Primitives/Button.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/Graphics/GfxManager.hpp"
#include "Editor/Widgets/CommonWidgets.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Common/FileSystem/FileSystem.hpp"
#include "Common/System/System.hpp"

namespace Lina::Editor
{
	void PanelTextureViewer::Construct()
	{
		m_editor = Editor::Get();

		DirectionalLayout* horizontal = m_manager->Allocate<DirectionalLayout>("Horizontal");
		horizontal->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		horizontal->SetAlignedPos(Vector2::Zero);
		horizontal->SetAlignedSize(Vector2::One);
		horizontal->GetProps().direction = DirectionOrientation::Horizontal;
		horizontal->GetProps().mode		 = DirectionalLayout::Mode::Bordered;
		AddChild(horizontal);

		Widget* textureBG = m_manager->Allocate<Widget>("TextureBG");
		textureBG->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		textureBG->SetAlignedPos(Vector2::Zero);
		textureBG->SetAlignedSize(Vector2(0.7f, 1.0f));
		textureBG->GetWidgetProps().childMargins = TBLR::Eq(Theme::GetDef().baseIndent);
		horizontal->AddChild(textureBG);

		Widget* texturePanel = m_manager->Allocate<Widget>("TexturePanel");
		texturePanel->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		texturePanel->SetAlignedPos(Vector2::Zero);
		texturePanel->SetAlignedSize(Vector2::One);
		texturePanel->GetWidgetProps().drawBackground	= true;
		texturePanel->GetWidgetProps().colorBackground	= Theme::GetDef().background0;
		texturePanel->GetWidgetProps().outlineThickness = 0.0f;
		texturePanel->GetWidgetProps().rounding			= 0.0f;
		texturePanel->GetWidgetProps().childMargins		= TBLR::Eq(Theme::GetDef().baseIndent);
		textureBG->AddChild(texturePanel);

		Widget* texture = m_manager->Allocate<Widget>("Texture");
		texture->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		texture->SetAlignedPos(Vector2::One * 0.5f);
		texture->SetAlignedSize(Vector2::One);
		texture->SetAnchorX(Anchor::Center);
		texture->SetAnchorY(Anchor::Center);
		texturePanel->AddChild(texture);

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

		m_texturePanel = texture;
		m_inspector	   = inspector;
	}

	void PanelTextureViewer::Initialize()
	{
		if (m_texture != nullptr)
			return;

        if(m_editor->GetProjectManager().GetProjectData() == nullptr)
            return;
        
		m_displayUserData = {};

		if (!m_editor->GetProjectManager().GetProjectData()->GetResourceRoot().FindResource(m_subData))
		{
			Text* text = m_manager->Allocate<Text>("Info");
			text->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
			text->SetAlignedPos(Vector2(0.5f, 0.5f));
			text->SetAnchorX(Anchor::Center);
			text->SetAnchorY(Anchor::Center);
			text->GetProps().text = Locale::GetStr(LocaleStr::ThisResourceNoLongerExists);
			m_texturePanel->AddChild(text);
			return;
		}

		ResourceDef def = {
			.id	 = m_subData,
			.tid = GetTypeID<Texture>(),
		};
		m_editor->GetResourceManagerV2().LoadResourcesFromProject(m_editor, m_editor->GetProjectManager().GetProjectData(), {def}, 0);
		m_editor->GetResourceManagerV2().WaitForAll();
		m_texture										  = m_editor->GetResourceManagerV2().GetResource<Texture>(def.id);
		m_texturePanel->GetWidgetProps().drawBackground	  = true;
		m_texturePanel->GetWidgetProps().fitTexture		  = true;
		m_texturePanel->GetWidgetProps().rawTexture		  = m_texture;
		m_texturePanel->GetWidgetProps().colorBackground  = Color::White;
		m_texturePanel->GetWidgetProps().outlineThickness = 0.0f;
		m_texturePanel->GetWidgetProps().rounding		  = 0.0f;
		m_texturePanel->GetWidgetProps().textureUserData  = &m_displayUserData;
		GetWidgetProps().debugName						  = "Texture: " + m_texture->GetName();

		StoreBuffer();

		m_textureName = m_texture->GetName();
		m_textureSize = TO_STRING(m_texture->GetSize().x) + " x " + TO_STRING(m_texture->GetSize().y);
		m_totalSizeKb = UtilStr::FloatToString(static_cast<float>(m_texture->GetTotalSize()) / 1000.0f, 1) + " KB";

		BuildInspector();

		Panel::Initialize();
	}

	void PanelTextureViewer::Destruct()
	{
		Panel::Destruct();
		if (m_texture == nullptr)
			return;

		delete[] m_textureBuffer.pixels;
		m_editor->GetResourceManagerV2().UnloadResources({m_texture});
	}

	void PanelTextureViewer::StoreBuffer()
	{
		if (m_textureBuffer.pixels != nullptr)
			delete[] m_textureBuffer.pixels;

		m_textureBuffer = {};

		const LinaGX::TextureBuffer base = m_texture->GetAllLevels().front();

		// Store initial buffer.
		const size_t sz		   = static_cast<size_t>(base.width * base.height * base.bytesPerPixel);
		m_textureBuffer		   = base;
		m_textureBuffer.pixels = new uint8[sz];
		MEMCPY(m_textureBuffer.pixels, base.pixels, sz);
	}

	void PanelTextureViewer::SetRuntimeDirty(bool isDirty)
	{
		m_containsRuntimeChanges = isDirty;

		Text* txt = GetWidgetOfType<Text>(m_saveButton);

		if (isDirty)
			txt->GetProps().text = Locale::GetStr(LocaleStr::Save) + " *";
		else
			txt->GetProps().text = Locale::GetStr(LocaleStr::Save);

		txt->CalculateTextSize();
	}

	void PanelTextureViewer::RegenTexture(const String& path)
	{
		m_editor->GetSystem()->CastSubsystem<GfxManager>(SubsystemType::GfxManager)->Join();
		m_texture->DestroyHW();

		if (path.empty())
			m_texture->LoadFromBuffer(m_textureBuffer.pixels, m_textureBuffer.width, m_textureBuffer.height, m_textureBuffer.bytesPerPixel);
		else
		{
			m_texture->LoadFromFile(path);
			m_texture->SetPath(path);
			m_texture->SetName(FileSystem::GetFilenameOnlyFromPath(path));
		}

		m_texture->GenerateHW();
		m_texture->AddToUploadQueue(m_editor->GetEditorRenderer().GetUploadQueue(), false);
		m_editor->GetEditorRenderer().MarkBindlessDirty();

		m_textureName			   = m_texture->GetName();
		GetWidgetProps().debugName = "Texture: " + m_textureName;
		m_textureSize			   = TO_STRING(m_texture->GetSize().x) + " x " + TO_STRING(m_texture->GetSize().y);
		m_totalSizeKb			   = UtilStr::FloatToString(static_cast<float>(m_texture->GetTotalSize()) / 1000.0f, 1) + " KB";

		m_formatDropdown->GetText()->GetProps().text = ReflectionSystem::Get().Meta<LinaGX::Format>().GetProperty<String>(static_cast<StringID>(m_texture->GetMeta().format));
		m_formatDropdown->GetText()->CalculateTextSize();
		m_txtName->GetProps().text	 = m_textureName;
		m_txtSize->GetProps().text	 = m_textureSize;
		m_txtSizeKb->GetProps().text = m_totalSizeKb;
		m_txtName->CalculateTextSize();
		m_txtSize->CalculateTextSize();
		m_txtSizeKb->CalculateTextSize();
		RefreshTab();
	}

	void PanelTextureViewer::SaveLayoutToStream(OStream& stream)
	{
	}

	void PanelTextureViewer::LoadLayoutFromStream(IStream& stream)
	{
	}

	void PanelTextureViewer::BuildInspector()
	{
		m_inspector->DeallocAllChildren();
		m_inspector->RemoveAllChildren();

		FoldLayout* generalFold = CommonWidgets::BuildFoldTitle(m_inspector, Locale::GetStr(LocaleStr::General), &m_generalInfoFold);
		m_inspector->AddChild(generalFold);

		CommonWidgets::BuildClassReflection(generalFold, this, ReflectionSystem::Get().Resolve<PanelTextureViewer>(), [this](const MetaType& meta, FieldBase* field) {
			m_displayUserData.displayChannels = m_displayChannels;
			m_displayUserData.mipLevel		  = m_mipLevel;
		});

		m_txtName	= static_cast<Text*>(generalFold->FindChildWithUserdata(&m_textureName));
		m_txtSize	= static_cast<Text*>(generalFold->FindChildWithUserdata(&m_textureSize));
		m_txtSizeKb = static_cast<Text*>(generalFold->FindChildWithUserdata(&m_totalSizeKb));

		Widget*		mipField			 = m_inspector->FindChildWithDebugName("Mip Level")->GetChildren().front();
		InputField* mipFieldInp			 = static_cast<InputField*>(mipField);
		mipFieldInp->GetProps().valueMax = static_cast<float>(m_texture->GetAllLevels().size() - 1);

		FoldLayout* textureFold = CommonWidgets::BuildFoldTitle(m_inspector, "Texture", &m_textureInfoFold);
		m_inspector->AddChild(textureFold);

		CommonWidgets::BuildClassReflection(textureFold, &m_texture->GetMeta(), ReflectionSystem::Get().Resolve<Texture::Metadata>(), [this](const MetaType& meta, FieldBase* field) {
			RegenTexture("");
			SetRuntimeDirty(true);
		});

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
		generalFold->AddChild(buttonLayout1);
		generalFold->AddChild(buttonLayout2);
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
				.extensions			   = {"png", "jpg"},
				.mode				   = PlatformProcess::DialogMode::SelectFile,
			});
			if (paths.empty())
				return;

			RegenTexture(paths.front());
			StoreBuffer();
			SetRuntimeDirty(true);
		};
		buttonLayout1->AddChild(importButton);

		Button* reimportButton = WidgetUtility::BuildIconTextButton(this, ICON_ROTATE, Locale::GetStr(LocaleStr::ReImport));
		reimportButton->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y);
		reimportButton->SetAlignedPosY(0.0f);
		reimportButton->SetAlignedSizeY(1.0f);
		reimportButton->SetFixedSizeX(Theme::GetDef().baseItemHeight * 4);
		reimportButton->GetProps().onClicked = [this, reimportButton]() {
			const String path = m_texture->GetPath();
			if (!FileSystem::FileOrPathExists(path))
			{
				CommonWidgets::ThrowInfoTooltip(Locale::GetStr(LocaleStr::FileNotFound), LogLevel::Error, 3.0f, reimportButton);
				return;
			}
			RegenTexture(path);
			StoreBuffer();
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
				ResourceDirectory* dir = m_editor->GetProjectManager().GetProjectData()->GetResourceRoot().FindResource(m_texture->GetID());
				m_resourceManager->SaveResource(m_editor->GetProjectManager().GetProjectData(), m_texture);
				m_editor->GetResourcePipeline().GenerateThumbnailForResource(dir, m_texture, true);
				dir->name = m_texture->GetName();

				Panel*	  p			  = m_editor->GetWindowPanelManager().FindPanelOfType(PanelType::ResourceBrowser, 0);
				if (p)
					static_cast<PanelResourceBrowser*>(p)->GetBrowser()->RefreshDirectory();

				SetRuntimeDirty(false);
			}
		};

		m_saveButton = save;
		buttonLayout2->AddChild(save);

		Button* sdfButton = WidgetUtility::BuildIconTextButton(this, ICON_ICONS, Locale::GetStr(LocaleStr::GenerateSDF));
		sdfButton->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y);
		sdfButton->SetAlignedPosY(0.0f);
		sdfButton->SetAlignedSizeY(1.0f);
		sdfButton->SetFixedSizeX(Theme::GetDef().baseItemHeight * 4);
		sdfButton->SetIsDisabled(true);
		buttonLayout2->AddChild(sdfButton);

		Widget*	  formatField				 = m_inspector->FindChildWithDebugName("Format")->GetChildren().front();
		Dropdown* formatdd					 = static_cast<Dropdown*>(formatField);
		m_formatDropdown					 = formatdd;
		formatdd->GetText()->GetProps().text = ReflectionSystem::Get().Meta<LinaGX::Format>().GetProperty<String>(static_cast<StringID>(m_texture->GetMeta().format));
		formatdd->GetText()->CalculateTextSize();
		formatdd->GetProps().onAddItems = [this](Popup* p) {
			const int32 ch = m_texture->GetImportedChannels();

			if (ch == 1)
			{
				if (m_texture->GetBytesPerPixel() == 1)
					p->AddToggleItem("R8_UNORM", m_texture->GetMeta().format == LinaGX::Format::R8_UNORM, (int32)LinaGX::Format::R8_UNORM);
				else if (m_texture->GetBytesPerPixel() == 2)
					p->AddToggleItem("R16_UNORM", m_texture->GetMeta().format == LinaGX::Format::R16_UNORM, (int32)LinaGX::Format::R16_UNORM);
			}
			else if (ch == 2)
			{
				if (m_texture->GetBytesPerPixel() == 2)
					p->AddToggleItem("R8G8_UNORM", m_texture->GetMeta().format == LinaGX::Format::R8G8_UNORM, (int32)LinaGX::Format::R8G8_UNORM);
				else if (m_texture->GetBytesPerPixel() == 4)
					p->AddToggleItem("R16G16_UNORM", m_texture->GetMeta().format == LinaGX::Format::R16G16_UNORM, (int32)LinaGX::Format::R16G16_UNORM);
			}
			else
			{
				if (m_texture->GetBytesPerPixel() == 4)
				{
					p->AddToggleItem("R8G8B8A8_UNORM", m_texture->GetMeta().format == LinaGX::Format::R8G8B8A8_UNORM, (int32)LinaGX::Format::R8G8B8A8_UNORM);
					p->AddToggleItem("R8G8B8A8_SRGB", m_texture->GetMeta().format == LinaGX::Format::R8G8B8A8_SRGB, (int32)LinaGX::Format::R8G8B8A8_SRGB);
				}
				else if (m_texture->GetBytesPerPixel() == 8)
				{
					p->AddToggleItem("R16G16B16A16_UNORM", m_texture->GetMeta().format == LinaGX::Format::R16G16B16A16_UNORM, (int32)LinaGX::Format::R16G16B16A16_UNORM);
				}
			}
		};
	}
} // namespace Lina::Editor
