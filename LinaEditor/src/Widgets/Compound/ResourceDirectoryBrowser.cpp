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
#include "Editor/Widgets/Panel/PanelResourceBrowser.hpp"
#include "Editor/Widgets/Panel/PanelWorld.hpp"
#include "Editor/Resources/ResourcePipeline.hpp"
#include "Editor/IO/ThumbnailGenerator.hpp"
#include "Editor/IO/ExtensionSupport.hpp"
#include "Editor/Actions/EditorActionResources.hpp"
#include "Common/FileSystem/FileSystem.hpp"
#include "Core/Meta/ProjectData.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/GUI/Widgets/Primitives/InputField.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/Layout/FoldLayout.hpp"
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
#include "Core/GUI/Widgets/Layout/ScrollArea.hpp"
#include "Core/Application.hpp"
#include "Core/GUI/Widgets/Primitives/Dropdown.hpp"
#include "Core/GUI/Widgets/Primitives/Icon.hpp"
#include "Core/GUI/Widgets/Layout/Popup.hpp"
#include "Core/World/EntityTemplate.hpp"
#include "Common/Serialization/Serialization.hpp"

namespace Lina::Editor
{

	void ResourceDirectoryBrowser::Construct()
	{
		m_editor					= Editor::Get();
		ParameterCollection& params = Editor::Get()->GetSettings().GetParams();

		DirectionalLayout* vertical = m_manager->Allocate<DirectionalLayout>();
		vertical->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		vertical->GetProps().direction				   = DirectionOrientation::Vertical;
		vertical->GetWidgetProps().childPadding		   = Theme::GetDef().baseIndent;
		vertical->GetWidgetProps().childMargins.top	   = Theme::GetDef().baseIndent;
		vertical->GetWidgetProps().childMargins.bottom = Theme::GetDef().baseIndent;
		AddChild(vertical);

		DirectionalLayout* header = m_manager->Allocate<DirectionalLayout>();
		header->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		header->SetAlignedPosX(0.0f);
		header->SetAlignedSizeX(1.0f);
		header->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		header->GetWidgetProps().childPadding		= Theme::GetDef().baseIndent;
		header->GetWidgetProps().childMargins.left	= Theme::GetDef().baseIndent;
		header->GetWidgetProps().childMargins.right = Theme::GetDef().baseIndent;
		vertical->AddChild(header);

		Dropdown* filterDD = m_manager->Allocate<Dropdown>("Fiter");
		filterDD->GetFlags().Set(WF_POS_ALIGN_Y | WF_USE_FIXED_SIZE_X | WF_SIZE_ALIGN_Y);
		filterDD->SetAlignedPosY(0.0f);
		filterDD->SetAlignedSizeY(1.0f);
		filterDD->SetFixedSizeX(Theme::GetDef().baseItemWidth * 1.5f);
		filterDD->GetIcon()->GetProps().icon = ICON_FILTER;
		filterDD->GetText()->GetProps().text = Locale::GetStr(LocaleStr::Filter) + ": " + ResourceDirectoryBrowser::GetFilterStr(static_cast<ResourceDirectoryBrowser::Filter>(params.GetParamUint8("panelResBrowserFilter"_hs, 0)));
		filterDD->GetProps().onAddItems		 = [this](Popup* popup) {
			 const uint8 max	= static_cast<uint8>(ResourceDirectoryBrowser::Filter::Max);
			 const uint8 filter = Editor::Get()->GetSettings().GetParams().GetParamUint8("panelResBrowserFilter"_hs, 0);

			 for (uint8 i = 0; i < max; i++)
				 popup->AddToggleItem(ResourceDirectoryBrowser::GetFilterStr(static_cast<ResourceDirectoryBrowser::Filter>(i)), filter == i, i);
		};

		filterDD->GetProps().onSelected = [this](int32 idx, String& newTitle) -> bool {
			Editor::Get()->GetSettings().GetParams().SetParamUint8("panelResBrowserFilter"_hs, static_cast<uint8>(idx));
			Editor::Get()->SaveSettings();
			newTitle = Locale::GetStr(LocaleStr::Filter) + ": " + ResourceDirectoryBrowser::GetFilterStr(static_cast<ResourceDirectoryBrowser::Filter>(idx));
			SetFilter(static_cast<ResourceDirectoryBrowser::Filter>(idx));
			return true;
		};

		header->AddChild(filterDD);

		InputField* searchField = m_manager->Allocate<InputField>();
		searchField->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		searchField->SetAlignedPosY(0.0f);
		searchField->SetAlignedSize(Vector2(0.0f, 1.0f));
		searchField->GetProps().usePlaceHolder	= true;
		searchField->GetProps().placeHolderText = Locale::GetStr(LocaleStr::Search);
		searchField->GetProps().placeHolderIcon = ICON_SEARCH;
		searchField->GetCallbacks().onEdited	= [searchField, this]() { SetSearchStr(searchField->GetValueStr()); };
		header->AddChild(searchField);

		ScrollArea* scroll = m_manager->Allocate<ScrollArea>("Scroll");
		scroll->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		scroll->SetAlignedPosX(0.0f);
		scroll->SetAlignedSize(Vector2::One);
		scroll->GetProps().direction = DirectionOrientation::Vertical;
		vertical->AddChild(scroll);

		ItemController* controller = m_manager->Allocate<ItemController>("Controller");
		controller->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		controller->SetAlignedPos(Vector2::Zero);
		controller->SetAlignedSize(Vector2::One);
		controller->GetWidgetProps().childMargins		  = {.top = Theme::GetDef().baseIndentInner, .bottom = Theme::GetDef().baseIndentInner};
		controller->GetWidgetProps().drawBackground		  = true;
		controller->GetWidgetProps().colorBackground	  = Theme::GetDef().background1;
		controller->GetWidgetProps().outlineThickness	  = 0.0f;
		controller->GetWidgetProps().dropshadow.enabled	  = true;
		controller->GetWidgetProps().dropshadow.color	  = Theme::GetDef().background0;
		controller->GetWidgetProps().dropshadow.steps	  = Theme::GetDef().baseDropShadowSteps;
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

			if (!CheckIfContainsEngineResource(selection))
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
			else if (item->resourceTID == GetTypeID<PhysicsMaterial>())
			{
				m_editor->GetWindowPanelManager().OpenPanel(PanelType::PhysicsMaterialViewer, item->resourceID, this);
			}
			else if (item->resourceTID == GetTypeID<TextureSampler>())
			{
				m_editor->GetWindowPanelManager().OpenPanel(PanelType::SamplerViewer, item->resourceID, this);
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
			else if (item->resourceTID == GetTypeID<GUIWidget>())
			{
				m_editor->GetWindowPanelManager().OpenPanel(PanelType::WidgetEditor, item->resourceID, this);
			}
			else if (item->resourceTID == GetTypeID<Audio>())
			{
				m_editor->GetWindowPanelManager().OpenPanel(PanelType::AudioViewer, item->resourceID, this);
			}
			else if (item->resourceTID == GetTypeID<EntityWorld>())
			{
				Panel* panel = m_editor->GetWindowPanelManager().OpenPanel(PanelType::World, 0, this);
				static_cast<PanelWorld*>(panel)->SetWorldToOpen(item->resourceID);
			}
		};

		controller->GetProps().payloadTypes	   = {PayloadType::Resource, PayloadType::EntitySelectable};
		controller->GetProps().onCreatePayload = [this]() {
			Widget*					   root		= m_editor->GetWindowPanelManager().GetPayloadRoot();
			Vector<ResourceDirectory*> payloads = m_controller->GetSelectedUserData<ResourceDirectory>();

			Text* t			   = root->GetWidgetManager()->Allocate<Text>();
			t->GetProps().text = payloads.size() == 1 ? payloads.front()->name : Locale::GetStr(LocaleStr::MultipleItems);
			t->Initialize();

			m_payloadItems = payloads;
			Editor::Get()->GetWindowPanelManager().CreatePayload(t, PayloadType::Resource, t->GetSize());
		};

		controller->GetProps().onPayloadAccepted = [this](void* ud, void* payloadUserData, PayloadType type) {
			if (type == PayloadType::Resource)
				DropPayload(static_cast<ResourceDirectory*>(ud));
			else
				DropPayload(static_cast<ResourceDirectory*>(ud), static_cast<Entity*>(payloadUserData));
		};

		controller->GetProps().onCheckCanCreatePayload = [this](void* ud) {
			ResourceDirectory* dir = static_cast<ResourceDirectory*>(ud);

			if (m_filter != Filter::None)
				return false;

			return true;
		};

		controller->GetProps().onDuplicate = [this]() {
			Vector<ResourceDirectory*> selection = m_controller->GetSelectedUserData<ResourceDirectory>();
			if (!CheckIfContainsEngineResource(selection))
				RequestDuplicate(selection);
		};

		m_linaAssets = ResourceDirectory{
			.name	  = Locale::GetStr(LocaleStr::LinaAssets),
			.isFolder = true,
			.userData = static_cast<uint32>(ResourceDirectoryType::LinaAssetsFolder),
		};

		m_editor->GetProjectManager().AddListener(this);

	} // namespace Lina::Editor

	void ResourceDirectoryBrowser::Initialize()
	{
		Widget::Initialize();
		ParameterCollection& params = Editor::Get()->GetSettings().GetParams();

		SetFilter(static_cast<ResourceDirectoryBrowser::Filter>(params.GetParamUint8("panelResBrowserFilter"_hs, 0)));
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

		const CommonWidgets::ResDirItemProperties linaAssetsProps = {
			.chevron	   = ICON_ARROW_RIGHT,
			.chevronAlt	   = ICON_ARROW_DOWN,
			.mainIcon	   = ICON_LINA_LOGO,
			.mainIconColor = Theme::GetDef().accentPrimary2,
			.title		   = m_linaAssets.name,
			.margin		   = Theme::GetDef().baseIndent,
			.unfoldValue   = &m_linaAssets.userData.unfolded,
			.userData	   = &m_linaAssets,
		};

		FoldLayout* layoutLinaAssets = CommonWidgets::BuildTreeItem(this, linaAssetsProps);
		m_layout->AddChild(layoutLinaAssets);

		// engine res.
		for (ResourceDirectory* child : root.children)
		{
			if (child->userData.directoryType != static_cast<uint32>(ResourceDirectoryType::EngineResource))
				continue;

			if (m_props.itemTypeIDFilter != 0 && !child->isFolder && m_props.itemTypeIDFilter != child->resourceTID)
				continue;

			if (!m_searchStr.empty())
			{
				if (!CheckIfContainsSearchStr(child))
					continue;
			}

			if (m_filter != Filter::None)
			{
				if (m_filter == Filter::Favourites)
				{
					if (!child->userData.isInFavourites)
						continue;
				}
			}

			AddItem(layoutLinaAssets, child, Theme::GetDef().baseIndent * 2);
		}

		String rootName		 = root.name;
		String rootIcon		 = ICON_FOLDER;
		Color  rootIconColor = Theme::GetDef().foreground0;
		if (m_filter == Filter::Favourites)
		{
			rootName += ": (" + Locale::GetStr(LocaleStr::Favourites) + ")";
			rootIcon	  = ICON_STAR;
			rootIconColor = Theme::GetDef().accentSecondary;
		}

		const CommonWidgets::ResDirItemProperties rootProps = {
			.chevron	   = ICON_ARROW_RIGHT,
			.chevronAlt	   = ICON_ARROW_DOWN,
			.mainIcon	   = rootIcon,
			.mainIconColor = rootIconColor,
			.title		   = rootName,
			.margin		   = Theme::GetDef().baseIndent,
			.unfoldValue   = &root.userData.unfolded,
			.userData	   = &root,
		};

		FoldLayout* layoutRoot = CommonWidgets::BuildTreeItem(this, rootProps);
		m_layout->AddChild(layoutRoot);

		AddItemForDirectory(layoutRoot, &root, Theme::GetDef().baseIndent * 2);

		m_controller->GatherItems(m_layout);
	}

	void ResourceDirectoryBrowser::SetFilter(Filter filter)
	{
		m_filter = filter;
		RefreshDirectory();
	}

	void ResourceDirectoryBrowser::SetSearchStr(const String& searchStr)
	{
		m_searchStr = UtilStr::ToLower(searchStr);
		RefreshDirectory();
	}

	String ResourceDirectoryBrowser::GetFilterStr(Filter filter)
	{
		if (filter == Filter::None)
			return Locale::GetStr(LocaleStr::None);

		if (filter == Filter::Favourites)
			return Locale::GetStr(LocaleStr::Favourites);

		return String();
	}

	void ResourceDirectoryBrowser::AddItem(Widget* parent, ResourceDirectory* item, float margin)
	{
		const CommonWidgets::ResDirItemProperties props = {
			.chevron		 = item->isFolder ? ICON_ARROW_RIGHT : "",
			.chevronAlt		 = item->isFolder ? ICON_ARROW_DOWN : "",
			.typeText		 = item->isFolder ? "" : ReflectionSystem::Get().Resolve(item->resourceTID)->GetProperty<String>("TypeAbbv"_hs),
			.typeColor		 = item->isFolder ? Color::White : ReflectionSystem::Get().Resolve(item->resourceTID)->GetProperty<Color>("Color"_hs),
			.mainIcon		 = item->isFolder ? ICON_FOLDER : "",
			.mainIconColor	 = Theme::GetDef().foreground0,
			.image			 = item->isFolder ? nullptr : m_editor->GetProjectManager().GetThumbnail(item),
			.title			 = item->name,
			.footerIcon		 = item->userData.isInFavourites ? ICON_STAR : "",
			.footerIconColor = Theme::GetDef().accentSecondary,
			.margin			 = margin,
			.unfoldValue	 = &item->userData.unfolded,
			.userData		 = item,
		};

		FoldLayout* layout = CommonWidgets::BuildTreeItem(this, props);
		parent->AddChild(layout);

		if (item->isFolder)
			AddItemForDirectory(layout, item, margin + Theme::GetDef().baseIndent * 2.0f);

	} // namespace Lina::Editor

	void ResourceDirectoryBrowser::AddItemForDirectory(Widget* parent, ResourceDirectory* dir, float margin)
	{
		for (ResourceDirectory* child : dir->children)
		{
			if (m_props.itemTypeIDFilter != 0 && !child->isFolder && m_props.itemTypeIDFilter != child->resourceTID)
				continue;

			// Handled specially
			if (child->userData.directoryType == static_cast<uint32>(ResourceDirectoryType::EngineResource))
				continue;

			if (!m_searchStr.empty())
			{
				if (!CheckIfContainsSearchStr(child))
					continue;
			}

			if (m_filter == Filter::Favourites)
			{
				if (!child->userData.isInFavourites && !IsChildInFavs(child))
					continue;
			}

			AddItem(parent, child, margin);
		}
	}

	void ResourceDirectoryBrowser::OnFileMenuGetItems(FileMenu* filemenu, StringID sid, Vector<FileMenuItem::Data>& outData, void* userData)
	{
		ProjectData* currentProject = m_editor->GetProjectManager().GetProjectData();
		if (currentProject == nullptr)
			return;
		ResourceDirectory& root = currentProject->GetResourceRoot();

		bool createDisabled	   = false;
		bool renameDisabled	   = false;
		bool deleteDisabled	   = false;
		bool duplicateDisabled = false;
		bool favDisabled	   = false;
		bool changeSrcDisabled = false;

		Vector<ResourceDirectory*> selection = m_controller->GetSelectedUserData<ResourceDirectory>();

		auto checkPredicate = [&](Delegate<bool(ResourceDirectory*)> pred) -> bool {
			for (ResourceDirectory* dir : selection)
			{
				if (pred(dir))
					return true;
			}

			return false;
		};

		const int32 size			  = static_cast<int32>(selection.size());
		const bool	anyFile			  = checkPredicate([](ResourceDirectory* dir) -> bool { return !dir->isFolder; });
		const bool	anyFolder		  = checkPredicate([](ResourceDirectory* dir) -> bool { return dir->isFolder; });
		const bool	anyEngineFolder	  = checkPredicate([this](ResourceDirectory* dir) -> bool { return dir == &m_linaAssets; });
		const bool	anyRootFolder	  = checkPredicate([this](ResourceDirectory* dir) -> bool { return dir == &m_editor->GetProjectManager().GetProjectData()->GetResourceRoot(); });
		const bool	anyEngineResource = checkPredicate([](ResourceDirectory* dir) -> bool { return dir->userData.directoryType == static_cast<uint32>(ResourceDirectoryType::EngineResource); });
		const bool	anyNonExternal	  = checkPredicate([](ResourceDirectory* dir) -> bool { return dir->resourceType != ResourceType::ExternalSource; });

		// Create
		if (anyFile || anyEngineFolder)
			createDisabled = true;

		// Rename
		if (size != 1 || anyEngineResource || anyEngineFolder || anyRootFolder)
			renameDisabled = true;

		// Delete
		if (size == 0 || anyEngineResource || anyEngineFolder || anyRootFolder)
			deleteDisabled = true;

		// Duplicate
		if (size == 0 || anyEngineResource || anyEngineFolder || anyRootFolder)
			duplicateDisabled = true;

		// Favourite
		if (size != 1 || anyEngineFolder || anyRootFolder)
			favDisabled = true;

		// Change src
		if (size != 1 || anyFolder || anyNonExternal || anyEngineResource)
			changeSrcDisabled = true;

		if (sid == 0)
		{
			outData.push_back(FileMenuItem::Data{
				.text		 = Locale::GetStr(LocaleStr::Rename),
				.altText	 = "F2",
				.headerIcon	 = ICON_EDIT_PEN,
				.hasDropdown = false,
				.isDisabled	 = renameDisabled,
				.userData	 = userData,
			});

			outData.push_back(FileMenuItem::Data{.isDivider = true});

			outData.push_back(FileMenuItem::Data{
				.text		 = Locale::GetStr(LocaleStr::Import),
				.headerIcon	 = ICON_IMPORT,
				.hasDropdown = false,
				.isDisabled	 = createDisabled,
				.userData	 = userData,
			});

			outData.push_back(FileMenuItem::Data{
				.text		 = Locale::GetStr(LocaleStr::ChangeSourceAsset),
				.headerIcon	 = ICON_FILE_PEN,
				.hasDropdown = false,
				.isDisabled	 = changeSrcDisabled,
				.userData	 = userData,
			});

			outData.push_back(FileMenuItem::Data{.isDivider = true});

			outData.push_back(FileMenuItem::Data{
				.text		  = Locale::GetStr(LocaleStr::Create),
				.dropdownIcon = ICON_ARROW_RIGHT,
				.hasDropdown  = true,
				.isDisabled	  = createDisabled,
				.userData	  = userData,
			});
			outData.push_back(FileMenuItem::Data{
				.text		= Locale::GetStr(LocaleStr::Delete),
				.altText	= "DEL",
				.headerIcon = ICON_TRASH,
				.isDisabled = deleteDisabled,
				.userData	= userData,
			});
			outData.push_back(FileMenuItem::Data{
				.text		= Locale::GetStr(LocaleStr::Duplicate),
				.altText	= "CTRL + D",
				.headerIcon = ICON_COPY,
				.isDisabled = duplicateDisabled,
				.userData	= userData,
			});
			outData.push_back(FileMenuItem::Data{.isDivider = true});

			const bool	 alreadyInFav = !selection.empty() && selection.front()->userData.isInFavourites;
			const String favText	  = alreadyInFav ? Locale::GetStr(LocaleStr::RemoveFromFavourites) : Locale::GetStr(LocaleStr::AddToFavourites);

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
				FileMenuItem::Data{.text = Locale::GetStr(LocaleStr::Sampler), .userData = userData},
				FileMenuItem::Data{.text = Locale::GetStr(LocaleStr::Material), .userData = userData},
				FileMenuItem::Data{.text = Locale::GetStr(LocaleStr::PhysicsMaterial), .userData = userData},
				FileMenuItem::Data{.text = Locale::GetStr(LocaleStr::World), .userData = userData},
				FileMenuItem::Data{.text = Locale::GetStr(LocaleStr::GUIWidget), .userData = userData},
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

		if (sid == TO_SID(Locale::GetStr(LocaleStr::ChangeSourceAsset)))
		{
			PlatformProcess::DialogProperties props;

			const String   extensions	 = ReflectionSystem::Get().Resolve(selection.front()->resourceTID)->GetProperty<String>("Extensions"_hs);
			Vector<String> extensionsVec = {};
			UtilStr::SeperateByChar(extensions, extensionsVec, ',');
			const Vector<String> files = PlatformProcess::OpenDialog({
				.title		   = Locale::GetStr(LocaleStr::Select),
				.primaryButton = Locale::GetStr(LocaleStr::Select),
				.extensions	   = extensionsVec,
				.mode		   = PlatformProcess::DialogMode::SelectFile,
			});

			if (!files.empty())
			{
				selection.front()->sourcePathRelativeToProject = FileSystem::GetRelative(FileSystem::GetFilePath(m_editor->GetProjectManager().GetProjectData()->GetPath()), files.front());
				m_editor->GetProjectManager().SaveProjectChanges();
			}
			return true;
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::Import)))
		{
			const Vector<String> files = PlatformProcess::OpenDialog({
				.title				   = Locale::GetStr(LocaleStr::Import),
				.primaryButton		   = Locale::GetStr(LocaleStr::Import),
				.extensionsDescription = "",
				.extensions			   = {"png", "jpg", "mp3", "glb", "gltf", "otf", "ttf", "linashader"},
				.mode				   = PlatformProcess::DialogMode::SelectFile,
				.multiSelection		   = true,
			});

			if (files.empty())
				return true;

			Vector<ResourcePipeline::ResourceImportDef> importDefinitions;
			for (const String& file : files)
				importDefinitions.push_back({.path = file});

			ResourceDirectory* directory = selection.front();

			m_importingResources.clear();

			EditorTask* task   = m_editor->GetTaskManager().CreateTask();
			task->ownerWindow  = GetWindow();
			task->useProgress  = true;
			task->progress	   = 0.0f;
			task->progressText = Locale::GetStr(LocaleStr::Working);
			task->title		   = Locale::GetStr(LocaleStr::ImportingResources);

			task->task = [task, importDefinitions, this, directory]() {
				const float totalSize = static_cast<float>(importDefinitions.size());

				Vector<ResourceDirectory*> resources =
					ResourcePipeline::ImportResources(m_editor->GetProjectManager().GetProjectData(), directory, importDefinitions, [this, task, totalSize](uint32 importedCount, const ResourcePipeline::ResourceImportDef& def, bool isComplete) {
						const float p	   = static_cast<float>(importedCount) / totalSize;
						task->progress	   = p;
						task->progressText = def.path;
					});

				m_editor->GetProjectManager().SaveProjectChanges();

				for (ResourceDirectory* dir : resources)
				{
					TextureAtlasImage* img = ThumbnailGenerator::GenerateThumbnail(m_editor->GetProjectManager().GetProjectData(), dir->resourceID, dir->resourceTID, m_editor->GetAtlasManager());
					m_importingResources.push_back(linatl::make_pair(dir, img));
				}
			};

			task->onComplete = [this]() {
				for (Pair<ResourceDirectory*, TextureAtlasImage*> pair : m_importingResources)
					m_editor->GetProjectManager().SetThumbnail(pair.first, pair.second);

				m_importingResources.clear();
				RefreshDirectory();
			};

			m_editor->GetTaskManager().AddTask(task);

			return true;
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::AddToFavourites)))
		{
			Vector<GUID> guids;
			guids.resize(selection.size());
			for (size_t i = 0; i < selection.size(); i++)
				guids[i] = selection[i]->guid;

			EditorActionResourceFav::Create(m_editor, guids, true);

			return true;
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::RemoveFromFavourites)))
		{
			Vector<GUID> guids;
			guids.resize(selection.size());
			for (size_t i = 0; i < selection.size(); i++)
				guids[i] = selection[i]->guid;
			EditorActionResourceFav::Create(m_editor, guids, false);

			return true;
		}

		ResourceDirectory* newCreated = nullptr;
		ResourceDirectory* front	  = selection.empty() ? &root : selection.front();

		if (sid == TO_SID(Locale::GetStr(LocaleStr::Folder)))
		{
			newCreated = m_editor->GetProjectManager().GetProjectData()->CreateResourceDirectory(front,
																								 {
																									 .name	   = "Folder",
																									 .isFolder = true,
																								 });
		}
		else if (sid == TO_SID(Locale::GetStr(LocaleStr::GUIWidget)))
		{
			newCreated = ResourcePipeline::SaveNewResource(m_editor->GetProjectManager().GetProjectData(), front, "GUIWidget", GetTypeID<GUIWidget>());
		}
		else if (sid == TO_SID(Locale::GetStr(LocaleStr::World)))
		{
			newCreated = ResourcePipeline::SaveNewResource(m_editor->GetProjectManager().GetProjectData(), front, "World", GetTypeID<EntityWorld>());
		}
		else if (sid == TO_SID(Locale::GetStr(LocaleStr::Material)))
		{
			newCreated = ResourcePipeline::SaveNewResource(m_editor->GetProjectManager().GetProjectData(), front, "Material", GetTypeID<Material>());
		}
		else if (sid == TO_SID(Locale::GetStr(LocaleStr::PhysicsMaterial)))
		{
			newCreated = ResourcePipeline::SaveNewResource(m_editor->GetProjectManager().GetProjectData(), front, "PhysicsMaterial", GetTypeID<PhysicsMaterial>());
		}
		else if (sid == TO_SID(Locale::GetStr(LocaleStr::Sampler)))
		{
			newCreated = ResourcePipeline::SaveNewResource(m_editor->GetProjectManager().GetProjectData(), front, "TextureSampler", GetTypeID<TextureSampler>());
		}
		else if (sid == TO_SID(Locale::GetStr(LocaleStr::OpaqueSurfaceShader)))
		{
			newCreated = ResourcePipeline::SaveNewResource(m_editor->GetProjectManager().GetProjectData(), front, "DeferredShader", GetTypeID<Shader>(), 0, 0);
		}
		else if (sid == TO_SID(Locale::GetStr(LocaleStr::TransparentSurfaceShader)))
		{
			newCreated = ResourcePipeline::SaveNewResource(m_editor->GetProjectManager().GetProjectData(), front, "ForwardShader", GetTypeID<Shader>(), 0, 1);
		}
		else if (sid == TO_SID(Locale::GetStr(LocaleStr::SkyShader)))
		{
			newCreated = ResourcePipeline::SaveNewResource(m_editor->GetProjectManager().GetProjectData(), front, "SkyShader", GetTypeID<Shader>(), 0, 2);
		}
		else if (sid == TO_SID(Locale::GetStr(LocaleStr::PostProcessShader)))
		{
			newCreated = ResourcePipeline::SaveNewResource(m_editor->GetProjectManager().GetProjectData(), front, "PostProcessShader", GetTypeID<Shader>(), 0, 3);
		}
		else
			return false;

		if (!newCreated->isFolder)
		{
			TextureAtlasImage* img = ThumbnailGenerator::GenerateThumbnail(m_editor->GetProjectManager().GetProjectData(), newCreated->resourceID, newCreated->resourceTID, m_editor->GetAtlasManager());
			m_editor->GetProjectManager().SetThumbnail(newCreated, img);
		}

		m_editor->GetProjectManager().SaveProjectChanges();

		RefreshDirectory();

		Widget* w = m_controller->GetItem(newCreated);

		// search str or filters can prevent it from being added
		if (w)
		{
			m_controller->MakeVisibleRecursively(w);
			m_controller->SelectItem(w, true, false);
		}

		return true;
	}

	void ResourceDirectoryBrowser::RequestRename(ResourceDirectory* dir)
	{
		Widget* parent = m_controller->GetItem(dir);
		Text*	text   = parent->GetWidgetOfType<Text>(parent);

		InputField* inp = m_manager->Allocate<InputField>();
		inp->GetFlags().Set(WF_USE_FIXED_SIZE_Y);
		inp->GetText()->GetProps().text = dir->name;
		inp->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		inp->SetSizeX(parent->GetSizeX());
		inp->SetPos(text->GetParent()->GetPos());
		inp->SetFixedSizeY(text->GetParent()->GetSizeY());
		inp->Initialize();

		inp->GetCallbacks().onEditEnded = [text, dir, inp, this]() {
			const String& str	  = inp->GetValueStr();
			text->GetProps().text = str;
			text->CalculateTextSize();
			text->GetWidgetManager()->AddToKillList(inp);
			EditorActionResourceRename::Create(m_editor, dir->guid, dir->name, str);
		};

		text->GetWidgetManager()->AddToForeground(inp);
		inp->StartEditing();
		inp->SelectAll();

		m_controller->SetFocus(true);
		m_manager->GrabControls(inp);
	}

	void ResourceDirectoryBrowser::RequestDelete(Vector<ResourceDirectory*> dirs)
	{
		const String text = Locale::GetStr(LocaleStr::AreYouSureYouWantToDeleteI) + " " + TO_STRING(dirs.size()) + " " + Locale::GetStr(LocaleStr::AreYouSureYouWantToDeleteII);

		Widget* lockRoot = m_editor->GetWindowPanelManager().LockAllForegrounds(m_lgxWindow, [](Widget* owner) -> Widget* { return CommonWidgets::BuildSimpleForegroundLockText(owner, Locale::GetStr(LocaleStr::WorkInProgressInAnotherWindow)); });

		Vector<CommonWidgets::GenericPopupButton> buttons = {
			{
				.title = Locale::GetStr(LocaleStr::Yes),
				.onPressed =
					[this, dirs]() {
						DeleteItems(dirs);
						m_editor->GetWindowPanelManager().UnlockAllForegrounds();
					},
			},
			{
				.title	   = Locale::GetStr(LocaleStr::No),
				.onPressed = [this]() { m_editor->GetWindowPanelManager().UnlockAllForegrounds(); },
			},
		};

		lockRoot->AddChild(CommonWidgets::BuildGenericPopupWithButtons(lockRoot, text, buttons));
	}

	void ResourceDirectoryBrowser::RequestDuplicate(Vector<ResourceDirectory*> dirs)
	{
		for (ResourceDirectory* item : dirs)
			ResourcePipeline::DuplicateResource(m_editor, item, item->parent);

		RefreshDirectory();
		m_editor->GetProjectManager().SaveProjectChanges();
	}

	void ResourceDirectoryBrowser::DeleteItems(Vector<ResourceDirectory*> dirs)
	{
		for (ResourceDirectory* item : dirs)
		{
			m_editor->GetProjectManager().GetProjectData()->DestroyResourceDirectory(item);
		}

		RefreshDirectory();
		m_editor->GetProjectManager().SaveProjectChanges();
	}

	void ResourceDirectoryBrowser::DropPayload(ResourceDirectory* target, Entity* entity)
	{
		if (target == &m_linaAssets)
			return;

		if (target == nullptr)
			target = &m_editor->GetProjectManager().GetProjectData()->GetResourceRoot();

		ResourceDirectory* edir = nullptr;
		if (!target->isFolder)
		{
			if (target->resourceTID != GetTypeID<EntityTemplate>())
				return;

			edir	   = target;
			edir->name = entity->GetName();
		}
		else
			edir = ResourcePipeline::SaveNewResource(m_editor->GetProjectManager().GetProjectData(), target, entity->GetName(), GetTypeID<EntityTemplate>());

		const String path	 = m_editor->GetProjectManager().GetProjectData()->GetResourcePath(edir->resourceID);
		IStream		 istream = Serialization::LoadFromFile(path.c_str());

		EntityTemplate tmp(0, "");
		tmp.LoadFromStream(istream);
		tmp.SetEntity(entity);
		tmp.SaveToFileAsBinary(path);
		RefreshDirectory();
	}

	void ResourceDirectoryBrowser::DropPayload(ResourceDirectory* target)
	{
		if (target == &m_linaAssets)
			return;

		if (target == nullptr)
			target = &m_editor->GetProjectManager().GetProjectData()->GetResourceRoot();

		if (!target->isFolder)
			return;

		if (CheckIfContainsEngineResource(m_payloadItems))
			return;

		Vector<GUID> previousParents;
		Vector<GUID> resources;
		const GUID	 targetGUID = target->guid;

		for (ResourceDirectory* carry : m_payloadItems)
		{
			if (carry->parent == target)
				continue;

			// wtf
			if (carry == target)
				continue;

			previousParents.push_back(carry->parent->guid);
			resources.push_back(carry->guid);
		}

		m_payloadItems.clear();

		if (!resources.empty())
			EditorActionResourceMove::Create(m_editor, resources, previousParents, targetGUID);
	}

	bool ResourceDirectoryBrowser::CheckIfContainsSearchStr(ResourceDirectory* dir)
	{
		if (UtilStr::ToLower(dir->name).find(m_searchStr) != String::npos)
			return true;

		for (ResourceDirectory* c : dir->children)
		{
			if (CheckIfContainsSearchStr(c))
				return true;
		}

		return false;
	}

	bool ResourceDirectoryBrowser::CheckIfContainsEngineResource(const Vector<ResourceDirectory*>& dirs)
	{
		ResourceDirectory* root = &m_editor->GetProjectManager().GetProjectData()->GetResourceRoot();

		for (ResourceDirectory* dir : dirs)
		{
			if (dir->userData.directoryType == static_cast<uint32>(ResourceDirectoryType::EngineResource))
				return true;
		}

		return false;
	}
	bool ResourceDirectoryBrowser::IsChildInFavs(ResourceDirectory* dir) const
	{
		for (ResourceDirectory* c : dir->children)
		{
			if (c->userData.isInFavourites)
				return true;

			if (c->isFolder && IsChildInFavs(c))
				return true;
		}

		return false;
	}
} // namespace Lina::Editor
