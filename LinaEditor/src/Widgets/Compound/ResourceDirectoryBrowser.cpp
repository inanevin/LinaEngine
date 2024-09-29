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

#include "Editor/Editor.hpp"
#include "Editor/EditorLocale.hpp"
#include "Editor/Widgets/Compound/ResourceDirectoryBrowser.hpp"
#include "Editor/Widgets/Layout/ItemController.hpp"
#include "Editor/Widgets/CommonWidgets.hpp"
#include "Editor/Widgets/Popups/GenericPopup.hpp"
#include "Editor/Widgets/FX/ProgressCircleFill.hpp"
#include "Core/Meta/ProjectData.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/GUI/Widgets/Primitives/InputField.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/Layout/ScrollArea.hpp"
#include "Core/Graphics/Resource/GUIWidget.hpp"
#include "Core/Graphics/Resource/Shader.hpp"
#include "Core/Graphics/Resource/Material.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Core/Graphics/Resource/Model.hpp"
#include "Core/Audio/Audio.hpp"
#include "Core/Graphics/Resource/TextureSampler.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Core/Physics/PhysicsMaterial.hpp"
#include "Core/Platform/PlatformProcess.hpp"

namespace Lina::Editor
{
	void ResourceDirectoryBrowser::Construct()
	{
		m_editor = Editor::Get();

		// GetWidgetProps().childMargins = TBLR::Eq(Theme::GetDef().baseIndent);

		ScrollArea* scroll = m_manager->Allocate<ScrollArea>("Scroll");
		scroll->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		scroll->SetAlignedPos(Vector2::Zero);
		scroll->SetAlignedSize(Vector2::One);
		scroll->GetProps().direction = DirectionOrientation::Vertical;
		AddChild(scroll);

		ItemController* controller = m_manager->Allocate<ItemController>("Controller");
		controller->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		controller->SetAlignedPos(Vector2::Zero);
		controller->SetAlignedSize(Vector2::One);
		controller->GetWidgetProps().childMargins		  = {.top = Theme::GetDef().baseIndentInner, .bottom = Theme::GetDef().baseIndentInner};
		controller->GetWidgetProps().drawBackground		  = true;
		controller->GetWidgetProps().colorBackground	  = Theme::GetDef().background0;
		controller->GetWidgetProps().outlineThickness	  = 0.0f;
		controller->GetWidgetProps().dropshadow.enabled	  = true;
		controller->GetWidgetProps().dropshadow.steps	  = 4;
		controller->GetWidgetProps().dropshadow.direction = Direction::Top;
		controller->GetWidgetProps().dropshadow.isInner	  = true;
		controller->GetContextMenu()->SetListener(this);
		scroll->AddChild(controller);

		DirectionalLayout* layout = m_manager->Allocate<DirectionalLayout>("VerticalLayout");
		layout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		layout->SetAlignedPos(Vector2::Zero);
		layout->SetAlignedSize(Vector2::One);
		layout->GetProps().direction		  = DirectionOrientation::Vertical;
		layout->GetWidgetProps().clipChildren = true;
		controller->AddChild(layout);

		scroll->SetTarget(layout);

		m_controller = controller;
		m_layout	 = layout;

		controller->GetProps().onItemRenamed = [this](void* ud) { RequestRename(static_cast<ResourceDirectory*>(ud)); };

		controller->GetProps().onDelete = [this]() {
			Vector<ResourceDirectory*> selection = m_controller->GetSelectedUserData<ResourceDirectory>();
			RequestDelete(selection);
		};

		controller->GetProps().onInteract = [this]() {
			Vector<ResourceDirectory*> selection = m_controller->GetSelectedUserData<ResourceDirectory>();

			ResourceDirectory* item = selection.front();

			if (item->isFolder)
				return;

			if (item->resourceTID == GetTypeID<Texture>())
			{
				m_editor->GetWindowPanelManager().OpenPanel(PanelType::TextureViewer, item->resourceID, this);
			}
			else if (item->resourceTID == GetTypeID<Font>())
			{
				m_editor->GetWindowPanelManager().OpenPanel(PanelType::FontViewer, item->resourceID, this);
			}
			else if (item->resourceTID == GetTypeID<Model>())
			{
				m_editor->GetWindowPanelManager().OpenPanel(PanelType::ModelViewer, item->resourceID, this);
			}
			else if (item->resourceTID == GetTypeID<Material>())
			{
				m_editor->GetWindowPanelManager().OpenPanel(PanelType::MaterialViewer, item->resourceID, this);
			}
			else if (item->resourceTID == GetTypeID<Audio>())
			{
				m_editor->GetWindowPanelManager().OpenPanel(PanelType::AudioViewer, item->resourceID, this);
			}
		};

		controller->GetProps().payloadType	   = PayloadType::Resource;
		controller->GetProps().onCreatePayload = [this](void* ud) {
			Widget*			   root = m_editor->GetWindowPanelManager().GetPayloadRoot();
			ResourceDirectory* dir	= static_cast<ResourceDirectory*>(ud);
			Text*			   t	= root->GetWidgetManager()->Allocate<Text>();
			t->GetProps().text		= dir->name;
			t->Initialize();
			m_payloadItem = dir;
			Editor::Get()->GetWindowPanelManager().CreatePayload(t, PayloadType::Resource, t->GetSize());
		};

		controller->GetProps().onPayloadAccepted = [this](void* ud) { DropPayload(static_cast<ResourceDirectory*>(ud)); };

		controller->GetProps().onCheckCanCreatePayload = [this](void* ud) {
			ResourceDirectory* dir	= static_cast<ResourceDirectory*>(ud);
			ResourceDirectory* root = &m_editor->GetProjectManager().GetProjectData()->GetResourceRoot();
			return dir != root;
		};

		controller->GetProps().onDuplicate = [this]() {
			Vector<ResourceDirectory*> selection = m_controller->GetSelectedUserData<ResourceDirectory>();
			RequestDuplicate(selection);
		};
	}

	void ResourceDirectoryBrowser::Initialize()
	{
		Widget::Initialize();
		RefreshDirectory();
		m_editor->GetProjectManager().AddListener(this);
	}

	void ResourceDirectoryBrowser::Destruct()
	{
		m_editor->GetProjectManager().RemoveListener(this);
	}

	void ResourceDirectoryBrowser::RefreshDirectory()
	{
		m_layout->DeallocAllChildren();
		m_layout->RemoveAllChildren();
		m_controller->ClearItems();

		ProjectData* currentProject = m_editor->GetProjectManager().GetProjectData();
		if (currentProject == nullptr)
			return;
		ResourceDirectory& root = currentProject->GetResourceRoot();

		Widget* rootFold = CommonWidgets::BuildDefaultFoldItem(this, &root, Theme::GetDef().baseIndent, ICON_FOLDER, Theme::GetDef().foreground0, root.name, !root.children.empty(), &root.unfolded, true);
		m_layout->AddChild(rootFold);
		m_controller->AddItem(rootFold->GetChildren().front());

		AddItemForDirectory(&root, Theme::GetDef().baseIndent * 2);
	}

	void ResourceDirectoryBrowser::AddItemForDirectory(ResourceDirectory* dir, float margin)
	{
		for (ResourceDirectory* child : dir->children)
		{
			if (m_props.itemTypeIDFilter != 0 && !child->isFolder && m_props.itemTypeIDFilter != child->resourceTID)
				continue;

			if (child->isFolder)
			{
				Widget* w = CommonWidgets::BuildDefaultFoldItem(this, child, margin, ICON_FOLDER, Theme::GetDef().foreground0, child->name, !child->children.empty(), &child->unfolded);
				m_controller->GetItem(dir)->GetParent()->AddChild(w);
				m_controller->AddItem(w->GetChildren().front());
				AddItemForDirectory(child, margin + Theme::GetDef().baseIndent * 2.0f);
			}
			else
			{
				const bool hasChildren = child->children.empty();

				TextureAtlasImage* img = child->_thumbnailAtlasImage ? child->_thumbnailAtlasImage : m_editor->GetAtlasManager().GetImageFromAtlas("ProjectIcons"_hs, "FileShaderSmall"_hs);
				Widget*			   w   = CommonWidgets::BuildTexturedListItem(this, child, margin, img, child->name);
				m_controller->GetItem(dir)->GetParent()->AddChild(w);
				m_controller->AddItem(w);
			}
		}
	}

	void ResourceDirectoryBrowser::OnFileMenuGetItems(FileMenu* filemenu, StringID sid, Vector<FileMenuItem::Data>& outData, void* userData)
	{
		ProjectData* currentProject = m_editor->GetProjectManager().GetProjectData();
		if (currentProject == nullptr)
			return;
		ResourceDirectory& root = currentProject->GetResourceRoot();

		bool renameDisabled	   = false;
		bool createDisabled	   = false;
		bool deleteDisabled	   = false;
		bool duplicateDisabled = false;
		bool favDisabled	   = false;
		bool alreadyInFav	   = false;
		bool importDisabled	   = false;

		if (m_controller->IsItemSelected(m_controller->GetItem(&root)))
		{
			renameDisabled	  = true;
			deleteDisabled	  = true;
			duplicateDisabled = true;
			favDisabled		  = true;
		}

		if (m_controller->GetSelectedItems().empty())
		{
			renameDisabled	  = true;
			deleteDisabled	  = true;
			duplicateDisabled = true;
			favDisabled		  = true;
			importDisabled	  = true;
		}

		if (m_controller->GetSelectedItems().size() > 1)
		{
			createDisabled = true;
			renameDisabled = true;
			importDisabled = true;
		}

		if (!m_controller->GetSelectedItems().empty() && !m_controller->GetSelectedUserData<ResourceDirectory>().front()->isFolder)
		{
			importDisabled = true;
		}

		if (sid == 0)
		{
			outData.push_back(FileMenuItem::Data{
				.text		 = Locale::GetStr(LocaleStr::Rename),
				.altText	 = "CTRL + R",
				.hasDropdown = false,
				.isDisabled	 = renameDisabled,
				.userData	 = userData,
			});

			outData.push_back(FileMenuItem::Data{.isDivider = true});

			outData.push_back(FileMenuItem::Data{
				.text		 = Locale::GetStr(LocaleStr::Import),
				.hasDropdown = false,
				.isDisabled	 = importDisabled,
				.userData	 = userData,
			});

			outData.push_back(FileMenuItem::Data{.isDivider = true});

			outData.push_back(FileMenuItem::Data{
				.text		  = Locale::GetStr(LocaleStr::Create),
				.dropdownIcon = ICON_CHEVRON_RIGHT,
				.hasDropdown  = true,
				.isDisabled	  = createDisabled,
				.userData	  = userData,
			});
			outData.push_back(FileMenuItem::Data{
				.text		= Locale::GetStr(LocaleStr::Delete),
				.altText	= "DEL",
				.isDisabled = deleteDisabled,
				.userData	= userData,
			});
			outData.push_back(FileMenuItem::Data{
				.text		= Locale::GetStr(LocaleStr::Duplicate),
				.altText	= "CTRL + D",
				.isDisabled = duplicateDisabled,
				.userData	= userData,
			});
			outData.push_back(FileMenuItem::Data{.isDivider = true});

			const String favText = alreadyInFav ? Locale::GetStr(LocaleStr::RemoveFromFavourites) : Locale::GetStr(LocaleStr::AddToFavourites);

			outData.push_back(FileMenuItem::Data{
				.text			 = favText,
				.altText		 = "CTRL + F",
				.headerIcon		 = ICON_STAR,
				.headerIconColor = Theme::GetDef().accentSecondary,
				.isDisabled		 = favDisabled,
				.userData		 = userData,
			});
		}
		else if (sid == TO_SID(Locale::GetStr(LocaleStr::Create)))
		{
			outData = {
				FileMenuItem::Data{.text = Locale::GetStr(LocaleStr::Folder), .userData = userData},
				FileMenuItem::Data{.text = Locale::GetStr(LocaleStr::Shader), .dropdownIcon = ICON_CHEVRON_RIGHT, .hasDropdown = true, .userData = userData},
				FileMenuItem::Data{.text = Locale::GetStr(LocaleStr::Sampler), .userData = userData},
				FileMenuItem::Data{.text = Locale::GetStr(LocaleStr::Material), .userData = userData},
				FileMenuItem::Data{.text = Locale::GetStr(LocaleStr::PhysicsMaterial), .userData = userData},
				FileMenuItem::Data{.text = Locale::GetStr(LocaleStr::World), .userData = userData},
				FileMenuItem::Data{.text = Locale::GetStr(LocaleStr::GUIWidget), .userData = userData},
			};
		}
		else if (sid == TO_SID(Locale::GetStr(LocaleStr::Shader)))
		{
			outData = {
				FileMenuItem::Data{.text = Locale::GetStr(LocaleStr::DeferredShader), .userData = userData},
				FileMenuItem::Data{.text = Locale::GetStr(LocaleStr::LightingShader), .userData = userData},
				FileMenuItem::Data{.text = Locale::GetStr(LocaleStr::ForwardShader), .userData = userData},
				FileMenuItem::Data{.text = Locale::GetStr(LocaleStr::PostProcessShader), .userData = userData},
			};
		}
	}

	bool ResourceDirectoryBrowser::OnFileMenuItemClicked(FileMenu* filemenu, StringID sid, void* userData)
	{
		ResourceDirectory&		   root		 = m_editor->GetProjectManager().GetProjectData()->GetResourceRoot();
		Vector<ResourceDirectory*> selection = m_controller->GetSelectedUserData<ResourceDirectory>();

		if (selection.empty())
			selection.push_back(&root);

		if (sid == TO_SID(Locale::GetStr(LocaleStr::Rename)))
		{
			ResourceDirectory* dir = selection.front();
			RequestRename(dir);
			return true;
		}

		Vector<ResourceDirectory*> roots;

		for (ResourceDirectory* dir : selection)
		{
			auto it = linatl::find_if(selection.begin(), selection.end(), [dir](ResourceDirectory* selected) -> bool { return selected == dir->parent; });
			if (it == selection.end())
				roots.push_back(dir);
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::Delete)))
		{
			RequestDelete(roots);
			return true;
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::Duplicate)))
		{
			RequestDuplicate(roots);
			return true;
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::Import)))
		{
			const Vector<String> files = PlatformProcess::OpenDialog({
				.title				   = Locale::GetStr(LocaleStr::Import),
				.primaryButton		   = Locale::GetStr(LocaleStr::Import),
				.extensionsDescription = "",
				.extensions			   = {"png", "jpg", "mp3", "glb", "gltf", "otf", "ttf", "linaresource"},
				.mode				   = PlatformProcess::DialogMode::SelectFile,
				.multiSelection		   = true,
			});

			if (files.empty())
				return true;

			ShowProgress();
			m_editor->GetResourcePipeline().ImportResources(selection.front(), files, [this](uint32 importedCount, float progress, bool isComplete) {
				if (isComplete)
				{
					HideProgress();
					return;
				}
				LINA_TRACE("Updating progress {0}", progress);
				m_progress->UpdateProgress(progress);
			});
			return true;
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::AddToFavourites)))
		{

			return true;
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::RemoveFromFavourites)))
		{
			return true;
		}

		ResourceDirectory* newCreated = nullptr;
		ResourceDirectory* front	  = selection.front();

		if (sid == TO_SID(Locale::GetStr(LocaleStr::Folder)))
		{
			newCreated = front->CreateChild({
				.name	  = "Folder",
				.isFolder = true,
			});
		}
		else if (sid == TO_SID(Locale::GetStr(LocaleStr::GUIWidget)))
		{
			newCreated = front->CreateChild({
				.name		 = "GUIWidget",
				.isFolder	 = false,
				.resourceID	 = m_editor->GetResourcePipeline().SaveNewResource(GetTypeID<GUIWidget>()),
				.resourceTID = GetTypeID<GUIWidget>(),
			});
		}
		else if (sid == TO_SID(Locale::GetStr(LocaleStr::World)))
		{
			newCreated = front->CreateChild({
				.name		 = "World",
				.isFolder	 = false,
				.resourceID	 = m_editor->GetResourcePipeline().SaveNewResource(GetTypeID<EntityWorld>()),
				.resourceTID = GetTypeID<EntityWorld>(),
			});
		}
		else if (sid == TO_SID(Locale::GetStr(LocaleStr::Material)))
		{
			newCreated = front->CreateChild({
				.name		 = "Material",
				.isFolder	 = false,
				.resourceID	 = m_editor->GetResourcePipeline().SaveNewResource(GetTypeID<Material>()),
				.resourceTID = GetTypeID<Material>(),
			});
		}
		else if (sid == TO_SID(Locale::GetStr(LocaleStr::PhysicsMaterial)))
		{
			newCreated = front->CreateChild({
				.name		 = "PhysicsMaterial",
				.isFolder	 = false,
				.resourceID	 = m_editor->GetResourcePipeline().SaveNewResource(GetTypeID<PhysicsMaterial>(), 0),
				.resourceTID = GetTypeID<PhysicsMaterial>(),
			});
		}
		else if (sid == TO_SID(Locale::GetStr(LocaleStr::Sampler)))
		{
			newCreated = front->CreateChild({
				.name		 = "TextureSampler",
				.isFolder	 = false,
				.resourceID	 = m_editor->GetResourcePipeline().SaveNewResource(GetTypeID<TextureSampler>(), 0),
				.resourceTID = GetTypeID<TextureSampler>(),
			});
		}
		else if (sid == TO_SID(Locale::GetStr(LocaleStr::DeferredShader)))
		{
			newCreated = front->CreateChild({
				.name		 = "Shader",
				.isFolder	 = false,
				.resourceID	 = m_editor->GetResourcePipeline().SaveNewResource(GetTypeID<Shader>(), 0),
				.resourceTID = GetTypeID<Shader>(),
			});
		}
		else if (sid == TO_SID(Locale::GetStr(LocaleStr::LightingShader)))
		{
			newCreated = front->CreateChild({
				.name		 = "Shader",
				.isFolder	 = false,
				.resourceID	 = m_editor->GetResourcePipeline().SaveNewResource(GetTypeID<Shader>(), 1),
				.resourceTID = GetTypeID<Shader>(),
			});
		}
		else if (sid == TO_SID(Locale::GetStr(LocaleStr::ForwardShader)))
		{
			newCreated = front->CreateChild({
				.name		 = "Shader",
				.isFolder	 = false,
				.resourceID	 = m_editor->GetResourcePipeline().SaveNewResource(GetTypeID<Shader>(), 2),
				.resourceTID = GetTypeID<Shader>(),
			});
		}
		else if (sid == TO_SID(Locale::GetStr(LocaleStr::PostProcessShader)))
		{
			newCreated = front->CreateChild({
				.name		 = "Shader",
				.isFolder	 = false,
				.resourceID	 = m_editor->GetResourcePipeline().SaveNewResource(GetTypeID<Shader>(), 3),
				.resourceTID = GetTypeID<Shader>(),
			});
		}
		else
			return false;

		m_editor->GetProjectManager().SaveProjectChanges();
		RefreshDirectory();
		m_controller->MakeVisibleRecursively(m_controller->GetItem(newCreated));
		m_controller->SelectItem(m_controller->GetItem(newCreated), true, false);

		return true;
	}

	void ResourceDirectoryBrowser::RequestRename(ResourceDirectory* dir)
	{
		Widget* parent = m_controller->GetItem(dir);
		Text*	text   = parent->GetWidgetOfType<Text>(parent);

		InputField* inp = m_manager->Allocate<InputField>();
		inp->GetFlags().Set(WF_USE_FIXED_SIZE_Y);
		inp->GetText()->GetProps().text = text->GetProps().text;
		inp->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		inp->SetSizeX(parent->GetSizeX());
		inp->SetPos(text->GetParent()->GetPos());
		inp->SetFixedSizeY(text->GetParent()->GetSizeY());
		inp->Initialize();

		inp->GetProps().onEditEnd = [text, inp, dir, parent, this](const String& str) {
			text->GetProps().text = str;
			text->CalculateTextSize();
			dir->name = str;
			text->GetWidgetManager()->AddToKillList(inp);
			dir->parent->SortChildren();
			RefreshDirectory();
			m_editor->GetProjectManager().SaveProjectChanges();
		};

		text->GetWidgetManager()->AddToForeground(inp);
		inp->StartEditing();
		inp->SelectAll();

		m_controller->SetFocus(true);
		m_manager->GrabControls(inp);
	}

	void ResourceDirectoryBrowser::RequestDelete(Vector<ResourceDirectory*> dirs)
	{
		const String  text	= Locale::GetStr(LocaleStr::AreYouSureYouWantToDeleteI) + " " + TO_STRING(dirs.size()) + " " + Locale::GetStr(LocaleStr::AreYouSureYouWantToDeleteII);
		GenericPopup* popup = CommonWidgets::ThrowGenericPopup(Locale::GetStr(LocaleStr::Delete), text, ICON_TRASH, this);
		popup->AddButton({
			.text = Locale::GetStr(LocaleStr::Yes),
			.onClicked =
				[dirs, this, popup]() {
					DeleteItems(dirs);
					m_manager->AddToKillList(popup);
				},
		});
		popup->AddButton({.text = Locale::GetStr(LocaleStr::No), .onClicked = [this, popup]() { m_manager->AddToKillList(popup); }});
		m_manager->AddToForeground(popup);
	}

	void ResourceDirectoryBrowser::RequestDuplicate(Vector<ResourceDirectory*> dirs)
	{
		for (ResourceDirectory* item : dirs)
			m_editor->GetResourcePipeline().DuplicateResource(&m_editor->GetResourceManagerV2(), item, item->parent);
		m_editor->GetAtlasManager().RefreshDirtyAtlases();
		RefreshDirectory();
		m_editor->GetProjectManager().SaveProjectChanges();
	}

	void ResourceDirectoryBrowser::DeleteItems(Vector<ResourceDirectory*> dirs)
	{
		for (ResourceDirectory* item : dirs)
		{
			if (item->_thumbnailAtlasImage != nullptr)
				m_editor->GetAtlasManager().RemoveImage(item->_thumbnailAtlasImage);

			item->parent->DestroyChild(item);
		}

		m_editor->GetAtlasManager().RefreshDirtyAtlases();
		RefreshDirectory();
		m_editor->GetProjectManager().SaveProjectChanges();
	}

	void ResourceDirectoryBrowser::DropPayload(ResourceDirectory* target)
	{
		if (target == nullptr)
			return;

		if (!target->isFolder)
			return;

		ResourceDirectory* carry = m_payloadItem;
		carry->parent->RemoveChild(carry);
		target->AddChild(carry);
		RefreshDirectory();
		m_editor->GetProjectManager().SaveProjectChanges();
	}

	void ResourceDirectoryBrowser::OnProjectOpened(ProjectData* data)
	{
		RefreshDirectory();
	}

	void ResourceDirectoryBrowser::ShowProgress()
	{
		m_progressParent = m_manager->Allocate<Widget>("ProgressParent");
		m_progressParent->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		m_progressParent->SetAlignedPos(Vector2::Zero);
		m_progressParent->SetAlignedSize(Vector2::One);
		m_progressParent->GetWidgetProps().drawBackground		   = true;
		m_progressParent->GetWidgetProps().colorBackground		   = Theme::GetDef().black;
		m_progressParent->GetWidgetProps().colorBackground.start.w = m_progressParent->GetWidgetProps().colorBackground.end.w = 0.9f;
		m_progressParent->GetWidgetProps().drawOrderIncrement																  = 1;
		AddChild(m_progressParent);

		DirectionalLayout* layout = m_manager->Allocate<DirectionalLayout>("ProgressLayout");
		layout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_USE_FIXED_SIZE_X | WF_USE_FIXED_SIZE_Y);
		layout->SetAlignedPos(Vector2(0.5f, 0.5f));
		layout->SetFixedSizeX(Theme::GetDef().baseItemHeight * 3);
		layout->SetFixedSizeY(layout->GetAlignedSizeX() + Theme::GetDef().baseItemHeight);
		layout->SetAnchorX(Anchor::Center);
		layout->SetAnchorY(Anchor::Center);
		layout->GetProps().direction		  = DirectionOrientation::Vertical;
		layout->GetWidgetProps().childPadding = Theme::GetDef().baseIndent;
		m_progressParent->AddChild(layout);

		Widget* rect = m_manager->Allocate<Widget>("ProgressLayoutRect");
		rect->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_Y_COPY_X);
		rect->SetAlignedPosX(0.0f);
		rect->SetAlignedSizeX(1.0f);
		layout->AddChild(rect);

		m_progress = m_manager->Allocate<ProgressCircleFill>("Progress");
		m_progress->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		m_progress->SetAlignedPos(Vector2::Zero);
		m_progress->SetAlignedSize(Vector2::One);
		rect->AddChild(m_progress);

		Text* txt			 = m_manager->Allocate<Text>("ProgressText");
		txt->GetProps().text = Locale::GetStr(LocaleStr::Working);
		txt->GetFlags().Set(WF_POS_ALIGN_X);
		txt->SetAlignedPosX(0.5f);
		txt->SetAnchorX(Anchor::Center);
		layout->AddChild(txt);

		m_progressParent->Initialize();
	}

	void ResourceDirectoryBrowser::HideProgress()
	{
		RemoveChild(m_progressParent);
		m_manager->Deallocate(m_progressParent);
		m_progress		 = nullptr;
		m_progressParent = nullptr;
	}
} // namespace Lina::Editor
