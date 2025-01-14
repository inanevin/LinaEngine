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

#include "Editor/Widgets/Compound/EntityBrowser.hpp"
#include "Editor/Widgets/World/WorldController.hpp"
#include "Editor/Widgets/World/WorldDisplayer.hpp"
#include "Editor/Widgets/Layout/ItemController.hpp"
#include "Editor/Widgets/Panel/PanelWorld.hpp"
#include "Editor/Editor.hpp"
#include "Editor/EditorLocale.hpp"
#include "Editor/Actions/EditorActionEntity.hpp"
#include "Editor/World/WorldUtility.hpp"
#include "Core/World/Components/CompLight.hpp"
#include "Core/GUI/Widgets/Layout/FoldLayout.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/GUI/Widgets/Layout/ScrollArea.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Editor/Widgets/CommonWidgets.hpp"
#include "Core/GUI/Widgets/Primitives/InputField.hpp"

namespace Lina::Editor
{
	void EntityBrowser::Construct()
	{
		m_editor = Editor::Get();

		DirectionalLayout* vertical = m_manager->Allocate<DirectionalLayout>("Vertical");
		vertical->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		vertical->SetAlignedPos(Vector2::Zero);
		vertical->SetAlignedSize(Vector2::One);
		vertical->GetProps().direction				   = DirectionOrientation::Vertical;
		vertical->GetWidgetProps().childPadding		   = Theme::GetDef().baseIndent;
		vertical->GetWidgetProps().childMargins.top	   = Theme::GetDef().baseIndent;
		vertical->GetWidgetProps().childMargins.bottom = Theme::GetDef().baseIndent;
		AddChild(vertical);

		DirectionalLayout* header = m_manager->Allocate<DirectionalLayout>("Header");
		header->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		header->SetAlignedPosX(0.0f);
		header->SetAlignedSizeX(1.0f);
		header->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		header->GetWidgetProps().childPadding		= Theme::GetDef().baseIndent;
		header->GetWidgetProps().childMargins.left	= Theme::GetDef().baseIndent;
		header->GetWidgetProps().childMargins.right = Theme::GetDef().baseIndent;
		vertical->AddChild(header);

		InputField* searchField = m_manager->Allocate<InputField>("SearchField");
		searchField->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		searchField->SetAlignedPosY(0.0f);
		searchField->SetAlignedSize(Vector2(0.0f, 1.0f));
		searchField->GetProps().usePlaceHolder	= true;
		searchField->GetProps().placeHolderText = Locale::GetStr(LocaleStr::Search);
		searchField->GetProps().placeHolderIcon = ICON_SEARCH;
        searchField->GetCallbacks().onEdited = [searchField, this](){
            m_searchStr = searchField->GetText()->GetProps().text;
            RefreshEntities();
        };
		header->AddChild(searchField);

		ScrollArea* scroll = m_manager->Allocate<ScrollArea>("Scroll");
		scroll->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		scroll->SetAlignedPosX(0.0f);
		scroll->SetAlignedSizeX(1.0f);
		scroll->SetAlignedSizeY(0.0f);
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

		controller->GetProps().onItemSelected = [this](void* ud) {
			const Vector<Entity*> selection = m_controller->GetSelectedUserData<Entity>();

			if (m_world)
				EditorActionEntitySelection::Create(m_editor, m_world->GetID(), selection, true, true, "EntityList"_hs);
		};

		controller->GetProps().onItemUnselected = [this](void* ud) {
			const Vector<Entity*> selection = m_controller->GetSelectedUserData<Entity>();
			if (m_world)
				EditorActionEntitySelection::Create(m_editor, m_world->GetID(), selection, true, true, "EntityList"_hs);
		};

		controller->GetProps().onItemRenamed = [this](void* ud) {
			Entity* e = static_cast<Entity*>(ud);
			RequestRename(e);
		};
		controller->GetProps().onDelete = [this]() {
			const Vector<Entity*> selection = m_controller->GetSelectedUserData<Entity>();
			EditorActionEntitySelection::Create(m_editor, m_world->GetID(), selection, false, true);
			EditorActionEntityDelete::Create(m_editor, m_world, selection);
			EditorActionCollective::Create(m_editor, 2);
		};

		controller->GetProps().onInteract = [this]() {
			Panel* p = m_editor->GetWindowPanelManager().FindPanelOfType(PanelType::World, 0);
			if (!p)
				return;

			static_cast<PanelWorld*>(p)->GetDisplayer()->GetController()->FocusSelected();
		};

		controller->GetProps().onDuplicate = [this]() {
			const Vector<Entity*> selection = m_controller->GetSelectedUserData<Entity>();
			Vector<Entity*>		  entities;
			WorldUtility::DuplicateEntities(m_editor, m_world, selection, entities);
			EditorActionEntitiesCreated::Create(m_editor, m_world, entities);
			EditorActionEntitySelection::Create(m_editor, m_world->GetID(), entities, true, true);
			EditorActionCollective::Create(m_editor, 2);
		};

		controller->GetProps().payloadType			   = PayloadType::EntitySelectable;
		controller->GetProps().onCheckCanCreatePayload = [](void* ud) { return true; };
		controller->GetProps().onCreatePayload		   = [this]() {
			Widget*			root	 = m_editor->GetWindowPanelManager().GetPayloadRoot();
			Vector<Entity*> payloads = m_controller->GetSelectedUserData<Entity>();

			Text* t			   = root->GetWidgetManager()->Allocate<Text>();
			t->GetProps().text = payloads.size() == 1 ? payloads.front()->GetName() : Locale::GetStr(LocaleStr::MultipleItems);
			t->Initialize();

			m_payloadItems = payloads;
			Editor::Get()->GetWindowPanelManager().CreatePayload(t, PayloadType::EntitySelectable, t->GetSize());
		};
		controller->GetProps().onPayloadAccepted = [this](void* ud) { DropPayload(static_cast<Entity*>(ud)); };

		controller->GetContextMenu()->SetListener(this);

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
	}

	void EntityBrowser::RefreshEntities()
	{
		m_layout->DeallocAllChildren();
		m_layout->RemoveAllChildren();
		m_controller->ClearItems();
		m_itemCtr = 0;

		if (m_world == nullptr)
			return;

		Vector<Entity*> roots;
		roots.reserve(m_world->GetActiveEntityCount());
		m_world->ViewEntities([&](Entity* e, uint32 idx) -> bool {
			if (e->GetParent() == nullptr)
				roots.push_back(e);
			return false;
		});

		for (Entity* root : roots)
		{
			if (!m_searchStr.empty() && !ContainsSearchStrRecursive(root))
				continue;

			AddItem(m_layout, root, Theme::GetDef().baseIndent);
		}

		m_controller->GatherItems(m_layout);
		OnEntitySelectionChanged(m_selectedEntities, true);
	}

	void EntityBrowser::SetWorld(EntityWorld* w)
	{
		m_world = w;
		RefreshEntities();
	}

	void EntityBrowser::OnEntitySelectionChanged(const Vector<Entity*>& entities, bool applySelection)
	{
		m_selectedEntities = entities;
		if (!applySelection)
			return;

		m_controller->UnselectAll();
		for (Entity* e : entities)
			m_controller->SelectItem(m_controller->GetItem(e), false, false, true);
	}

	void EntityBrowser::DropPayload(Entity* e)
	{
		EditorActionEntityParenting::Create(m_editor, m_world, m_payloadItems, e);
		m_payloadItems.clear();
	}

	void EntityBrowser::AddItem(Widget* parent, Entity* e, float margin)
	{
		const Vector<Entity*>& children = e->GetChildren();

		const CommonWidgets::EntityItemProperties props = {
			.icon		 = ICON_CUBE,
			.iconColor	 = Theme::GetDef().foreground0,
			.title		 = e->GetName(),
			.hasChildren = !children.empty(),
			.margin		 = margin,
			.unfoldValue = &e->GetInterfaceUserData().unfolded,
			.userData	 = e,
		};

		FoldLayout* layout = CommonWidgets::BuildEntityItem(this, props);
		parent->AddChild(layout);

		if (m_itemCtr % 2 == 0)
		{
			layout->GetWidgetProps().drawBackground	  = true;
			layout->GetWidgetProps().outlineThickness = 0.0f;
			layout->GetWidgetProps().rounding		  = 0.0f;
			layout->GetWidgetProps().colorBackground  = Theme::GetDef().background2;
		}

		m_itemCtr++;

		for (Entity* c : children)
		{
			if (!m_searchStr.empty() && !ContainsSearchStrRecursive(c))
				continue;
			AddItem(layout, c, margin + Theme::GetDef().baseIndent);
		}
	}

	void EntityBrowser::RequestRename(Entity* e)
	{
		Widget* parent = m_controller->GetItem(e);
		Text*	text   = parent->GetWidgetOfType<Text>(parent);

		InputField* inp = m_manager->Allocate<InputField>("");
		inp->GetFlags().Set(WF_USE_FIXED_SIZE_Y);
		inp->GetText()->GetProps().text = e->GetName();
		inp->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		inp->SetSizeX(parent->GetSizeX());
		inp->SetPos(text->GetParent()->GetPos());
		inp->SetFixedSizeY(text->GetParent()->GetSizeY());
		inp->Initialize();

        inp->GetCallbacks().onEditEnded = [text, e, inp, this](){
            const String& str = inp->GetValueStr();
            text->GetProps().text = str;
            text->CalculateTextSize();
            text->GetWidgetManager()->AddToKillList(inp);
            const String stored = e->GetName();
            e->SetName(str);
            EditorActionEntityNames::Create(m_editor, m_world->GetID(), {e}, {stored});
        };
	
		text->GetWidgetManager()->AddToForeground(inp);
		inp->StartEditing();
		inp->SelectAll();

		m_controller->SetFocus(true);
		m_manager->GrabControls(inp);
	}

	bool EntityBrowser::ContainsSearchStrRecursive(Entity* e)
	{
		if (e->GetName().find(m_searchStr) != String::npos)
			return true;

		for (Entity* c : e->GetChildren())
		{
			if (ContainsSearchStrRecursive(c))
				return true;
		}

		return false;
	}

	bool EntityBrowser::OnFileMenuItemClicked(FileMenu* filemenu, StringID sid, void* userData)
	{
		const Vector<Entity*> selection = m_controller->GetSelectedUserData<Entity>();

		if (sid == TO_SID(Locale::GetStr(LocaleStr::Rename)))
		{
			Entity* e = selection.front();
			RequestRename(e);
			return true;
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::Delete)))
		{
			EditorActionEntitySelection::Create(m_editor, m_world->GetID(), selection, false, true);
			EditorActionEntityDelete::Create(m_editor, m_world, selection);
			EditorActionCollective::Create(m_editor, 2);
			return true;
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::Duplicate)))
		{
			Vector<Entity*> entities;
			WorldUtility::DuplicateEntities(m_editor, m_world, selection, entities);
			EditorActionEntitiesCreated::Create(m_editor, m_world, entities);
			EditorActionEntitySelection::Create(m_editor, m_world->GetID(), entities, true, true);
			EditorActionCollective::Create(m_editor, 2);
			return true;
		}
        
        const Vector3 cameraPos = m_world->GetWorldCamera().GetPosition() + m_world->GetWorldCamera().GetRotation().GetForward() * 1.0f;
        const Vector3 createPos = selection.empty() ? cameraPos : selection.at(0)->GetPosition();

		if (sid == TO_SID(Locale::GetStr(LocaleStr::PointLight)))
		{
            Entity* e = m_world->CreateEntity(m_world->ConsumeEntityGUID(), Locale::GetStr(LocaleStr::PointLight));
            e->SetPosition(createPos);
            CompLight* comp = m_world->AddComponent<CompLight>(e);
            comp->SetType(LightType::Point);
            
            EditorActionEntitiesCreated::Create(m_editor, m_world, {e});
            EditorActionEntitySelection::Create(m_editor, m_world->GetID(), {e}, true, true);
            EditorActionCollective::Create(m_editor, 2);
			return true;
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::SpotLight)))
		{
            Entity* e = m_world->CreateEntity(m_world->ConsumeEntityGUID(), Locale::GetStr(LocaleStr::SpotLight));
            e->SetPosition(createPos);
            CompLight* comp = m_world->AddComponent<CompLight>(e);
            comp->SetType(LightType::Spot);
            
            EditorActionEntitiesCreated::Create(m_editor, m_world, {e});
            EditorActionEntitySelection::Create(m_editor, m_world->GetID(), {e}, true, true);
            EditorActionCollective::Create(m_editor, 2);
			return true;
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::DirectionalLight)))
		{

            Entity* e = m_world->CreateEntity(m_world->ConsumeEntityGUID(), Locale::GetStr(LocaleStr::DirectionalLight));
            e->SetPosition(createPos);
            CompLight* comp = m_world->AddComponent<CompLight>(e);
            comp->SetType(LightType::Directional);
            
            EditorActionEntitiesCreated::Create(m_editor, m_world, {e});
            EditorActionEntitySelection::Create(m_editor, m_world->GetID(), {e}, true, true);
            EditorActionCollective::Create(m_editor, 2);
			return true;
		}

		
		return false;
	}

	void EntityBrowser::OnFileMenuGetItems(FileMenu* filemenu, StringID sid, Vector<FileMenuItem::Data>& outData, void* userData)
	{
        if(!m_world)
            return;
        
		const Vector<Entity*> selection = m_controller->GetSelectedUserData<Entity>();

		bool basicActionsDisabled = false;

		if (selection.empty())
		{
			basicActionsDisabled = true;
		}

		if (sid == 0)
		{
			outData.push_back(FileMenuItem::Data{
				.text		 = Locale::GetStr(LocaleStr::Rename),
				.altText	 = "F2",
				.headerIcon	 = ICON_EDIT_PEN,
				.hasDropdown = false,
				.isDisabled	 = basicActionsDisabled,
				.userData	 = userData,
			});

			outData.push_back(FileMenuItem::Data{.isDivider = true});

			outData.push_back(FileMenuItem::Data{
				.text		  = Locale::GetStr(LocaleStr::Create),
				.dropdownIcon = ICON_CHEVRON_RIGHT,
				.hasDropdown  = true,
				.isDisabled	  = false,
				.userData	  = userData,
			});

			outData.push_back(FileMenuItem::Data{
				.text		= Locale::GetStr(LocaleStr::Delete),
				.altText	= "DEL",
				.headerIcon = ICON_TRASH,
				.isDisabled = basicActionsDisabled,
				.userData	= userData,
			});

			outData.push_back(FileMenuItem::Data{
				.text		= Locale::GetStr(LocaleStr::Duplicate),
				.altText	= "CTRL + D",
				.headerIcon = ICON_COPY,
				.isDisabled = basicActionsDisabled,
				.userData	= userData,
			});

			outData.push_back(FileMenuItem::Data{.isDivider = true});

			outData.push_back(FileMenuItem::Data{
				.text		  = Locale::GetStr(LocaleStr::Display),
				.dropdownIcon = ICON_ARROW_RIGHT,
				.hasDropdown  = true,
				.isDisabled	  = basicActionsDisabled,
				.userData	  = userData,
			});
		}
		else if (sid == TO_SID(Locale::GetStr(LocaleStr::Create)))
		{
			outData = {
				FileMenuItem::Data{.text = Locale::GetStr(LocaleStr::PointLight), .userData = userData},
				FileMenuItem::Data{.text = Locale::GetStr(LocaleStr::SpotLight), .userData = userData},
				FileMenuItem::Data{.text = Locale::GetStr(LocaleStr::DirectionalLight), .userData = userData},
			};
		}
		else if (sid == TO_SID(Locale::GetStr(LocaleStr::Display)))
		{
			outData = {

			};
		}
	}

} // namespace Lina::Editor
