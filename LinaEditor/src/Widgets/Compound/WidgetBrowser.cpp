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

#include "Editor/Widgets/Compound/WidgetBrowser.hpp"
#include "Editor/Widgets/World/WorldController.hpp"
#include "Editor/Widgets/World/WorldDisplayer.hpp"
#include "Editor/Widgets/Layout/ItemController.hpp"
#include "Editor/Widgets/Panel/PanelWidgetEditor.hpp"
#include "Editor/Editor.hpp"
#include "Editor/EditorLocale.hpp"
#include "Editor/Actions/EditorActionWidget.hpp"
#include "Editor/World/EditorWorldUtility.hpp"
#include "Core/World/WorldUtility.hpp"
#include "Core/World/Components/CompLight.hpp"
#include "Core/GUI/Widgets/Layout/FoldLayout.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/GUI/Widgets/Layout/ScrollArea.hpp"
#include "Editor/Widgets/CommonWidgets.hpp"
#include "Core/GUI/Widgets/Primitives/InputField.hpp"

namespace Lina::Editor
{
	void WidgetBrowser::Construct()
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
		searchField->GetCallbacks().onEdited	= [searchField, this]() {
			   m_searchStr = searchField->GetText()->GetProps().text;
			   RefreshWidgets();
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
			const Vector<Widget*> selection = m_controller->GetSelectedUserData<Widget>();
			m_panelWidgetEditor->SetSelectedWidgets(selection);
		};

		controller->GetProps().onItemUnselected = [this](void* ud) {
			const Vector<Widget*> selection = m_controller->GetSelectedUserData<Widget>();
			m_panelWidgetEditor->SetSelectedWidgets(selection);
		};

		controller->GetProps().onItemRenamed = [this](void* ud) {
			Widget* w = static_cast<Widget*>(ud);
			RequestRename(w);
		};

		controller->GetProps().onDelete = [this]() {
			const Vector<Widget*> selection = m_controller->GetSelectedUserData<Widget>();
			m_panelWidgetEditor->Delete(selection);
		};

		controller->GetProps().onDuplicate = [this]() {
			const Vector<Widget*> selection = m_controller->GetSelectedUserData<Widget>();
			m_panelWidgetEditor->Duplicate(selection);
		};

		controller->GetProps().payloadTypes			   = {PayloadType::WidgetEditorWidget};
		controller->GetProps().onCheckCanCreatePayload = [](void* ud) { return true; };
		controller->GetProps().onCreatePayload		   = [this]() {
			Widget*			root	 = m_editor->GetWindowPanelManager().GetPayloadRoot();
			Vector<Widget*> payloads = m_controller->GetSelectedUserData<Widget>();

			Text* t			   = root->GetWidgetManager()->Allocate<Text>();
			t->GetProps().text = payloads.size() == 1 ? payloads.front()->GetWidgetProps().debugName : Locale::GetStr(LocaleStr::MultipleItems);
			t->Initialize();
			t->SetUserData(payloads.at(0));

			m_payloadItems = payloads;
			Editor::Get()->GetWindowPanelManager().CreatePayload(t, PayloadType::WidgetEditorWidget, t->GetSize());
		};
		controller->GetProps().onPayloadAccepted = [this](void* ud, void* payloadUserData, PayloadType type) { DropPayload(static_cast<Widget*>(ud)); };

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

	void WidgetBrowser::SetRoot(Widget* root)
	{
		m_root = root;
		RefreshWidgets();
	}

	void WidgetBrowser::RefreshWidgets()
	{
		m_layout->DeallocAllChildren();
		m_layout->RemoveAllChildren();
		m_controller->ClearItems();
		m_itemCtr = 0;

		if (!m_searchStr.empty() && !ContainsSearchStrRecursive(m_root))
			return;

		AddItem(m_layout, m_root, Theme::GetDef().baseIndent);
		m_controller->GatherItems(m_layout);
	}

	void WidgetBrowser::DropPayload(Widget* w)
	{
		m_panelWidgetEditor->Parent(m_payloadItems, w);
		m_payloadItems.clear();
	}

	void WidgetBrowser::AddItem(Widget* parent, Widget* w, float margin)
	{
		const Vector<Widget*>& children = w->GetChildren();

		const CommonWidgets::EntityItemProperties props = {
			.icon		 = ICON_CUBE,
			.iconColor	 = Theme::GetDef().foreground0,
			.title		 = w->GetWidgetProps().debugName,
			.hasChildren = !children.empty(),
			.margin		 = margin,
			.unfoldValue = &w->_fold,
			.userData	 = w,
		};

		FoldLayout* layout = CommonWidgets::BuildEntityItem(this, props);
		parent->AddChild(layout);

		// if (m_itemCtr % 2 == 0)
		// {
		// 	layout->GetWidgetProps().drawBackground	  = true;
		// 	layout->GetWidgetProps().outlineThickness = 0.0f;
		// 	layout->GetWidgetProps().rounding		  = 0.0f;
		// 	layout->GetWidgetProps().colorBackground  = Theme::GetDef().background2;
		// }
		//
		// m_itemCtr++;

		for (Widget* c : children)
		{
			if (!m_searchStr.empty() && !ContainsSearchStrRecursive(c))
				continue;
			AddItem(layout, c, margin + Theme::GetDef().baseIndent);
		}
	}

	void WidgetBrowser::RequestRename(Widget* w)
	{
		Widget* parent = m_controller->GetItem(w);
		Text*	text   = parent->GetWidgetOfType<Text>(parent);

		InputField* inp = m_manager->Allocate<InputField>("");
		inp->GetFlags().Set(WF_USE_FIXED_SIZE_Y);
		inp->GetText()->GetProps().text = w->GetWidgetProps().debugName;
		inp->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		inp->SetSizeX(parent->GetSizeX());
		inp->SetPos(text->GetParent()->GetPos());
		inp->SetFixedSizeY(text->GetParent()->GetSizeY());
		inp->Initialize();

		inp->GetCallbacks().onEditEnded = [text, w, inp, this]() {
			const String& str	  = inp->GetValueStr();
			text->GetProps().text = str;
			text->CalculateTextSize();
			text->GetWidgetManager()->AddToKillList(inp);
			const String stored = w->GetWidgetProps().debugName;
			m_panelWidgetEditor->Rename(w, str);
		};

		text->GetWidgetManager()->AddToForeground(inp);
		inp->StartEditing();
		inp->SelectAll();

		m_controller->SetFocus(true);
		m_manager->GrabControls(inp);
	}

	bool WidgetBrowser::ContainsSearchStrRecursive(Widget* w)
	{
		if (w->GetWidgetProps().debugName.find(m_searchStr) != String::npos)
			return true;

		for (Widget* c : w->GetChildren())
		{
			if (ContainsSearchStrRecursive(c))
				return true;
		}

		return false;
	}

	bool WidgetBrowser::OnFileMenuItemClicked(FileMenu* filemenu, StringID sid, void* userData)
	{
		const Vector<Widget*> selection = m_controller->GetSelectedUserData<Widget>();

		if (sid == TO_SID(Locale::GetStr(LocaleStr::Rename)))
		{
			Widget* w = selection.front();
			RequestRename(w);
			return true;
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::Delete)))
		{
			m_panelWidgetEditor->Delete(selection);
			return true;
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::Duplicate)))
		{
			m_panelWidgetEditor->Duplicate(selection);
			return true;
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::Widget)))
		{
			Widget* w = m_manager->Allocate<Widget>("Widget");
			w->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
			w->SetAlignedPos(Vector2::Zero);
			w->SetAlignedSize(Vector2::One);
			m_panelWidgetEditor->AddNew(w, selection.empty() ? nullptr : selection.front());
			return true;
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::Text)))
		{
			Text* w = m_manager->Allocate<Text>("Text");
			w->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
			w->SetAlignedPos(Vector2::Zero);
			w->UpdateTextAndCalcSize("Text");
			w->GetProps().font = ENGINE_FONT_ROBOTO_ID;
			m_panelWidgetEditor->AddNew(w, selection.empty() ? nullptr : selection.front());
			return true;
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::DirectionalLayout)))
		{
			DirectionalLayout* w = m_manager->Allocate<DirectionalLayout>("Layout");
			w->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
			w->SetAlignedPos(Vector2::Zero);
			m_panelWidgetEditor->AddNew(w, selection.empty() ? nullptr : selection.front());
			return true;
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::Button)))
		{
			Button* w = m_manager->Allocate<Button>("Button");
			w->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
			w->SetAlignedPos(Vector2::Zero);
			m_panelWidgetEditor->AddNew(w, selection.front());
			return true;
		}

		/*
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

		if (sid == TO_SID(Locale::GetStr(LocaleStr::Entity)))
		{
			Entity* e = m_world->CreateEntity(m_world->ConsumeEntityGUID(), Locale::GetStr(LocaleStr::PointLight));
			e->SetPosition(createPos);
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
		*/

		return false;
	}

	void WidgetBrowser::OnFileMenuGetItems(FileMenu* filemenu, StringID sid, Vector<FileMenuItem::Data>& outData, void* userData)
	{
		const Vector<Widget*> selection = m_controller->GetSelectedUserData<Widget>();

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
		}
		else if (sid == TO_SID(Locale::GetStr(LocaleStr::Create)))
		{
			outData = {

				FileMenuItem::Data{.text = Locale::GetStr(LocaleStr::Widget), .userData = userData},
				FileMenuItem::Data{.text = Locale::GetStr(LocaleStr::Text), .userData = userData},
				FileMenuItem::Data{.text = Locale::GetStr(LocaleStr::DirectionalLayout), .userData = userData},
				FileMenuItem::Data{.text = Locale::GetStr(LocaleStr::Button), .userData = userData},
			};
		}
		else if (sid == TO_SID(Locale::GetStr(LocaleStr::Display)))
		{
			outData = {

			};
		}
	}

} // namespace Lina::Editor
