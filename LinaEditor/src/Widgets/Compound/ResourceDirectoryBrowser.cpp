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
#include "Core/Meta/ProjectData.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/GUI/Widgets/Primitives/InputField.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/Layout/ScrollArea.hpp"
#include "Core/Graphics/Resource/GUIWidget.hpp"
#include "Core/Graphics/Resource/Shader.hpp"
#include "Core/Graphics/Resource/Material.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Core/Physics/PhysicsMaterial.hpp"

namespace Lina::Editor
{
	void ResourceDirectoryBrowser::Construct()
	{
		m_editor = Editor::Get();

		// GetWidgetProps().childMargins = TBLR::Eq(Theme::GetDef().baseIndent);

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
		AddChild(controller);

		ScrollArea* scroll = m_manager->Allocate<ScrollArea>("Scroll");
		scroll->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		scroll->SetAlignedPos(Vector2::Zero);
		scroll->SetAlignedSize(Vector2::One);
		scroll->GetProps().direction = DirectionOrientation::Vertical;
		controller->AddChild(scroll);

		DirectionalLayout* layout = m_manager->Allocate<DirectionalLayout>("VerticalLayout");
		layout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		layout->SetAlignedPos(Vector2::Zero);
		layout->SetAlignedSize(Vector2::One);
		layout->GetProps().direction		  = DirectionOrientation::Vertical;
		layout->GetWidgetProps().clipChildren = true;
		scroll->AddChild(layout);

		m_controller = controller;
		m_layout	 = layout;

		controller->GetProps().onItemRenamed = [this](void* ud) { RequestRename(static_cast<ResourceDirectory*>(ud)); };

		controller->GetProps().onDelete = [this]() {
			Vector<ResourceDirectory*> selection = m_controller->GetSelectedUserData<ResourceDirectory>();
			RequestDelete(selection);
		};

		controller->GetProps().onInteract = []() {

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
			if (child->isFolder)
			{
				Widget* w = CommonWidgets::BuildDefaultFoldItem(this, child, margin, ICON_FOLDER, Theme::GetDef().foreground0, child->name, !child->children.empty(), &child->unfolded);
				m_controller->GetItem(dir)->GetParent()->AddChild(w);
				m_controller->AddItem(w->GetChildren().front());
				AddItemForDirectory(child, margin + Theme::GetDef().baseIndent);
			}
			else
			{
				Widget* w = CommonWidgets::BuildTexturedListItem(this, child, margin, m_editor->GetAtlasManager().GetImageFromAtlas("ProjectIcons"_hs, "FileShaderSmall"_hs), child->name, true);
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
		}

		if (m_controller->GetSelectedItems().size() > 1)
		{
			createDisabled = true;
			renameDisabled = true;
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
			return false;

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
		{
			// TODO: duplicate the resources.
			item->parent->AddChild(item->Duplicate());
		}

		RefreshDirectory();
		m_editor->GetProjectManager().SaveProjectChanges();
	}

	void ResourceDirectoryBrowser::DeleteItems(Vector<ResourceDirectory*> dirs)
	{
		for (ResourceDirectory* item : dirs)
		{
			// TODO: delete the resource.
			item->parent->DestroyChild(item);
		}
		RefreshDirectory();
		m_editor->GetProjectManager().SaveProjectChanges();
	}
} // namespace Lina::Editor
