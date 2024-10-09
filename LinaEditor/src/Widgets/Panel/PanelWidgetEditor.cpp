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

#include "Editor/Widgets/Panel/PanelWidgetEditor.hpp"
#include "Editor/Widgets/Panel/PanelWidgetEditorProperties.hpp"
#include "Editor/Widgets/Layout/ItemController.hpp"
#include "Editor/Widgets/CommonWidgets.hpp"
#include "Editor/Editor.hpp"
#include "Editor/EditorLocale.hpp"
#include "Core/Resources/ResourceDirectory.hpp"
#include "Core/Graphics/Resource/GUIWidget.hpp"
#include "Core/GUI/Widgets/WidgetUtility.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/GUI/Widgets/Layout/ScrollArea.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/Primitives/Button.hpp"
#include "Core/GUI/Widgets/Primitives/Icon.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/Primitives/InputField.hpp"
#include "Core/Meta/ProjectData.hpp"
#include "Core/Platform/PlatformProcess.hpp"
#include "Editor/Widgets/Compound/WindowBar.hpp"
#include "Editor/Widgets/Compound/ResourceDirectoryBrowser.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina::Editor
{
	void PanelWidgetEditor::Construct()
	{
		m_editor = Editor::Get();

		DirectionalLayout* horizontal = m_manager->Allocate<DirectionalLayout>("Horizontal");
		horizontal->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		horizontal->SetAlignedPos(Vector2::Zero);
		horizontal->SetAlignedSize(Vector2::One);
		horizontal->GetProps().direction = DirectionOrientation::Horizontal;
		horizontal->GetProps().mode		 = DirectionalLayout::Mode::Bordered;
		AddChild(horizontal);

		DirectionalLayout* leftSide = m_manager->Allocate<DirectionalLayout>("Left");
		leftSide->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		leftSide->SetAlignedPos(Vector2(0.0f, 0.0f));
		leftSide->SetAlignedSize(Vector2(0.25f, 1.0f));
		leftSide->GetProps().mode	   = DirectionalLayout::Mode::Bordered;
		leftSide->GetProps().direction = DirectionOrientation::Vertical;
		horizontal->AddChild(leftSide);

		Widget* widgetsPanel = m_manager->Allocate<Widget>("WidgetsPanel");
		widgetsPanel->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		widgetsPanel->SetAlignedPos(Vector2(0.0f, 0.0f));
		widgetsPanel->SetAlignedSize(Vector2(1.0f, 0.5f));
		widgetsPanel->GetWidgetProps().childMargins = TBLR::Eq(Theme::GetDef().baseIndent);
		leftSide->AddChild(widgetsPanel);

		ItemController* itemControllerWidgets = m_manager->Allocate<ItemController>("ItemController");
		itemControllerWidgets->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		itemControllerWidgets->SetAlignedPos(Vector2::Zero);
		itemControllerWidgets->SetAlignedSize(Vector2::One);
		itemControllerWidgets->GetWidgetProps().drawBackground		 = true;
		itemControllerWidgets->GetWidgetProps().dropshadow.enabled	 = true;
		itemControllerWidgets->GetWidgetProps().dropshadow.isInner	 = true;
		itemControllerWidgets->GetWidgetProps().dropshadow.direction = Direction::Top;
		itemControllerWidgets->GetWidgetProps().dropshadow.color	 = Theme::GetDef().black;
		itemControllerWidgets->GetWidgetProps().dropshadow.color.w	 = 0.25f;
		itemControllerWidgets->GetWidgetProps().dropshadow.steps	 = 8;
		itemControllerWidgets->GetWidgetProps().outlineThickness	 = 0.0f;
		itemControllerWidgets->GetWidgetProps().colorBackground		 = Theme::GetDef().background0;
		itemControllerWidgets->GetWidgetProps().childMargins		 = {.top = Theme::GetDef().baseIndentInner, .bottom = Theme::GetDef().baseIndentInner};

		itemControllerWidgets->GetProps().onCreatePayload = [this](void* userdata) {
			Widget*		root   = m_editor->GetWindowPanelManager().GetPayloadRoot();
			WidgetInfo* inf	   = static_cast<WidgetInfo*>(userdata);
			Text*		t	   = root->GetWidgetManager()->Allocate<Text>();
			t->GetProps().text = inf->title;
			t->Initialize();
			m_payloadCarryTID = inf->tid;
			Editor::Get()->GetWindowPanelManager().CreatePayload(t, PayloadType::WidgetEditorWidget, t->GetSize());
		};

		itemControllerWidgets->GetProps().onCheckCanCreatePayload = [](void* userdata) -> bool { return userdata != nullptr; };
		m_widgetsController										  = itemControllerWidgets;
		widgetsPanel->AddChild(itemControllerWidgets);

		{
			ScrollArea* scroll = m_manager->Allocate<ScrollArea>("Scroll");
			scroll->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
			scroll->SetAlignedPos(Vector2::Zero);
			scroll->SetAlignedSize(Vector2::One);
			scroll->GetProps().direction = DirectionOrientation::Vertical;
			itemControllerWidgets->AddChild(scroll);

			DirectionalLayout* layout = m_manager->Allocate<DirectionalLayout>("VerticalLayout");
			layout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
			layout->SetAlignedPos(Vector2::Zero);
			layout->SetAlignedSize(Vector2::One);
			layout->GetProps().direction		  = DirectionOrientation::Vertical;
			layout->GetWidgetProps().clipChildren = true;
			scroll->AddChild(layout);
			m_widgetsLayout = layout;
		}

		DirectionalLayout* hierarchy = m_manager->Allocate<DirectionalLayout>("Hierarchy");
		hierarchy->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		hierarchy->SetAlignedPos(Vector2(0.0f, 0.5f));
		hierarchy->SetAlignedSize(Vector2(1.0f, 0.5f));
		hierarchy->GetWidgetProps().childMargins = TBLR::Eq(Theme::GetDef().baseIndent);
		hierarchy->GetProps().direction			 = DirectionOrientation::Vertical;
		hierarchy->GetWidgetProps().childPadding = Theme::GetDef().baseIndent;
		leftSide->AddChild(hierarchy);

		DirectionalLayout* buttons = m_manager->Allocate<DirectionalLayout>("Buttons");
		buttons->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		buttons->SetAlignedSizeX(1.0f);
		buttons->GetProps().direction = DirectionOrientation::Horizontal;
		buttons->GetProps().mode	  = DirectionalLayout::Mode::EqualSizes;
		buttons->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		buttons->GetWidgetProps().childPadding = Theme::GetDef().baseIndent;
		hierarchy->AddChild(buttons);

		Button* btnLoad = WidgetUtility::BuildIconTextButton(this, ICON_SAVE, Locale::GetStr(LocaleStr::Load));
		btnLoad->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y);
		btnLoad->SetAlignedPosY(0.5f);
		btnLoad->SetAnchorY(Anchor::Center);
		btnLoad->SetAlignedSizeY(1.0f);
		btnLoad->GetProps().onClicked = [this]() { CommonWidgets::ThrowResourceSelector(this, GetTypeID<GUIWidget>(), [this](ResourceDirectory* dir) { CheckSaveCurrent([this, dir]() { OpenWidget(dir->resourceID); }); }); };
		buttons->AddChild(btnLoad);

		Button* btnSave = WidgetUtility::BuildIconTextButton(this, ICON_SAVE, Locale::GetStr(LocaleStr::Save));
		btnSave->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y);
		btnSave->SetAlignedPosY(0.5f);
		btnSave->SetAnchorY(Anchor::Center);
		btnSave->SetAlignedSizeY(1.0f);
		btnSave->GetProps().onClicked = [this]() {
			if (m_currentWidget)
				m_editor->GetResourceManagerV2().SaveResource(m_editor->GetProjectManager().GetProjectData(), m_currentWidget);
		};
		buttons->AddChild(btnSave);

		Button* btnExport = WidgetUtility::BuildIconTextButton(this, ICON_EXPORT, Locale::GetStr(LocaleStr::Export));
		btnExport->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y);
		btnExport->SetAlignedPosY(0.5f);
		btnExport->SetAnchorY(Anchor::Center);
		btnExport->SetAlignedSizeY(1.0f);
		btnExport->GetProps().onClicked = [this]() {
			const String fullPath = PlatformProcess::SaveDialog({
				.title				   = Locale::GetStr(LocaleStr::Save),
				.primaryButton		   = Locale::GetStr(LocaleStr::Save),
				.extensionsDescription = "",
				.extensions			   = {"linaresource"},
				.mode				   = PlatformProcess::DialogMode::SelectFile,
			});
			m_currentWidget->SaveToFileAsBinary(fullPath);
		};
		buttons->AddChild(btnExport);

		ItemController* itemControllerHierarchy = m_manager->Allocate<ItemController>("ItemController");
		itemControllerHierarchy->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		itemControllerHierarchy->SetAlignedPosX(0.0f);
		itemControllerHierarchy->SetAlignedSize(Vector2(1.0f, 0.0f));
		itemControllerHierarchy->GetWidgetProps().drawBackground	   = true;
		itemControllerHierarchy->GetWidgetProps().dropshadow.enabled   = true;
		itemControllerHierarchy->GetWidgetProps().dropshadow.isInner   = true;
		itemControllerHierarchy->GetWidgetProps().dropshadow.color	   = Theme::GetDef().black;
		itemControllerHierarchy->GetWidgetProps().dropshadow.color.w   = 0.25f;
		itemControllerHierarchy->GetWidgetProps().dropshadow.steps	   = 8;
		itemControllerHierarchy->GetWidgetProps().dropshadow.direction = Direction::Top;
		itemControllerHierarchy->GetWidgetProps().outlineThickness	   = 0.0f;
		itemControllerHierarchy->GetWidgetProps().colorBackground	   = Theme::GetDef().background0;
		itemControllerHierarchy->GetWidgetProps().childMargins		   = {.top = Theme::GetDef().baseIndentInner, .bottom = Theme::GetDef().baseIndentInner};
		itemControllerHierarchy->GetContextMenu()->SetListener(this);
		itemControllerHierarchy->GetProps().payloadType = PayloadType::WidgetEditorWidget;

		itemControllerHierarchy->GetProps().onCreatePayload = [this](void* userdata) {
			Widget* root		 = m_editor->GetWindowPanelManager().GetPayloadRoot();
			Widget* pickedWidget = static_cast<Widget*>(userdata);
			Text*	t			 = root->GetWidgetManager()->Allocate<Text>();
			t->GetProps().text	 = pickedWidget->GetWidgetProps().debugName;
			t->Initialize();
			m_payloadCarryWidget = pickedWidget;
			Editor::Get()->GetWindowPanelManager().CreatePayload(t, PayloadType::WidgetEditorWidget, t->GetSize());
		};

		itemControllerHierarchy->GetProps().onCheckCanCreatePayload = [this](void* ud) -> bool {
			Widget* w = static_cast<Widget*>(ud);
			if (w == &m_currentWidget->GetRoot())
				return false;

			return true;
		};

		itemControllerHierarchy->GetProps().onPayloadAccepted = [this](void* userdata) {
			Widget* parent = userdata == nullptr ? &m_currentWidget->GetRoot() : static_cast<Widget*>(userdata);
			AddPayloadToWidget(parent);
		};
		m_hierarchyController = itemControllerHierarchy;
		hierarchy->AddChild(itemControllerHierarchy);

		itemControllerHierarchy->GetProps().onDelete = [itemControllerHierarchy, this]() {
			Vector<Widget*> selection = itemControllerHierarchy->GetSelectedUserData<Widget>();
			RequestDelete(selection);
		};

		itemControllerHierarchy->GetProps().onDuplicate = [itemControllerHierarchy, this]() {
			Vector<Widget*> selection = itemControllerHierarchy->GetSelectedUserData<Widget>();
			RequestDuplicate(selection);
		};

		itemControllerHierarchy->GetProps().onItemRenamed = [itemControllerHierarchy, this](void* item) { RequestRename(static_cast<Widget*>(item)); };

		itemControllerHierarchy->GetProps().onItemSelected = [this](void* userdata) {
			Widget* w = static_cast<Widget*>(userdata);
			m_propertiesArea->Refresh(w);
		};

		{
			ScrollArea* scroll = m_manager->Allocate<ScrollArea>("Scroll");
			scroll->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
			scroll->SetAlignedPos(Vector2::Zero);
			scroll->SetAlignedSize(Vector2::One);
			scroll->GetProps().direction = DirectionOrientation::Vertical;
			itemControllerHierarchy->AddChild(scroll);

			DirectionalLayout* layout = m_manager->Allocate<DirectionalLayout>("VerticalLayout");
			layout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
			layout->SetAlignedPos(Vector2::Zero);
			layout->SetAlignedSize(Vector2::One);
			layout->GetProps().direction		  = DirectionOrientation::Vertical;
			layout->GetWidgetProps().clipChildren = true;
			scroll->AddChild(layout);
			m_hierarchyLayout = layout;
		}

		Widget* gridArea = m_manager->Allocate<Widget>("Grid");
		gridArea->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		gridArea->SetAlignedPos(Vector2(0.25f, 0.0f));
		gridArea->SetAlignedSize(Vector2(0.5f, 1.0f));
		gridArea->GetWidgetProps().childMargins = TBLR::Eq(Theme::GetDef().baseIndent * 2);
		horizontal->AddChild(gridArea);
		m_gridParent = gridArea;

		PanelWidgetEditorProperties* propertiesArea = m_manager->Allocate<PanelWidgetEditorProperties>("Properties");
		propertiesArea->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		propertiesArea->SetAlignedPos(Vector2(0.75f, 0.0f));
		propertiesArea->SetAlignedSize(Vector2(0.25f, 1.0f));
		horizontal->AddChild(propertiesArea);
		m_propertiesArea = propertiesArea;

		m_leftSide	  = leftSide;
		m_leftSideTop = widgetsPanel;
		m_leftSideBot = hierarchy;
		m_middle	  = m_gridParent;
		m_rightSide	  = m_propertiesArea;
	}

	void PanelWidgetEditor::PreDestruct()
	{
		if (m_currentWidget == nullptr)
			return;

		CloseCurrent(false);
	}

	void PanelWidgetEditor::SaveLayoutToStream(OStream& stream)
	{
		stream << m_leftSide->GetAlignedSizeX();
		stream << m_leftSideTop->GetAlignedSizeY();
		stream << m_middle->GetAlignedSizeX();
		stream << m_lastOpenWidget;
	}

	void PanelWidgetEditor::LoadLayoutFromStream(IStream& stream)
	{
		float align0 = 0.0f, align1 = 0.0f, align2 = 0.0f;
		stream >> align0 >> align1 >> align2;

		m_leftSide->SetAlignedSizeX(align0);
		m_middle->SetAlignedPosX(align0);
		m_middle->SetAlignedSizeX(align2);
		m_rightSide->SetAlignedPosX(align0 + align2);
		m_rightSide->SetAlignedSizeX(1.0f - align0 - align2);
		m_leftSideTop->SetAlignedSizeY(align1);
		m_leftSideBot->SetAlignedPosY(align1);
		m_leftSideBot->SetAlignedSizeY(1.0f - align1);

		stream >> m_lastOpenWidget;
		OpenWidget(m_lastOpenWidget);
	}

	void PanelWidgetEditor::Initialize()
	{
		Widget::Initialize();
		RefreshWidgets();
	}

	void PanelWidgetEditor::Tick(float delta)
	{
	}

	void PanelWidgetEditor::Draw()
	{
		Widget::Draw();
		Vector<Widget*> selection = m_hierarchyController->GetSelectedUserData<Widget>();

		LinaVG::StyleOptions opts;
		opts.isFilled  = false;
		opts.thickness = Theme::GetDef().baseOutlineThickness * 2.0f;
		opts.color	   = Theme::GetDef().accentPrimary0.AsLVG4();

		for (auto* w : selection)
		{
			m_lvg->DrawRect(w->GetRect().pos.AsLVG(), w->GetRect().GetEnd().AsLVG(), opts, 0.0f, m_drawOrder + 1);
		}
	}

	void PanelWidgetEditor::RefreshWidgets()
	{
		m_widgetsLayout->DeallocAllChildren();
		m_widgetsLayout->RemoveAllChildren();
		m_widgetsController->ClearItems();
		m_categories.clear();

		const HashMap<TypeID, MetaType>& types = ReflectionSystem::Get().GetTypes();
		for (auto& [typeID, meta] : types)
		{
			if (meta.HasProperty<uint32>("WidgetIdent"_hs))
			{
				const String title	  = meta.GetProperty<String>("Title"_hs);
				const String category = meta.GetProperty<String>("Category"_hs);

				if (category.compare("Hidden") == 0)
					continue;

				WidgetInfo widgetInfo = {
					.title = title,
					.tid   = typeID,
				};

				auto it = linatl::find_if(m_categories.begin(), m_categories.end(), [&category](const CategoryInfo& inf) -> bool { return inf.title.compare(category) == 0; });

				if (it != m_categories.end())
				{
					it->widgets.push_back(widgetInfo);
				}
				else
				{
					m_categories.push_back({});
					CategoryInfo& categoryInfo = m_categories.back();
					categoryInfo.title		   = category;
					categoryInfo.widgets.push_back(widgetInfo);
				}
			}
		}

		for (CategoryInfo& category : m_categories)
		{
			Widget* cat = CommonWidgets::BuildDefaultFoldItem(this, nullptr, Theme::GetDef().baseIndent, "", Color::White, category.title, true, nullptr, true, true);
			m_widgetsController->AddItem(cat->GetChildren().front());
			m_widgetsLayout->AddChild(cat);

			for (WidgetInfo& wif : category.widgets)
			{
				Widget* wi = CommonWidgets::BuildDefaultListItem(this, &wif, Theme::GetDef().baseIndent * 2.0f, "", Color::White, wif.title, true);
				cat->AddChild(wi);
				m_widgetsController->AddItem(wi);
			}
		}
	}

	void PanelWidgetEditor::RefreshHierarchy()
	{
		m_hierarchyLayout->DeallocAllChildren();
		m_hierarchyLayout->RemoveAllChildren();
		m_hierarchyController->ClearItems();

		if (m_currentWidget == nullptr)
			return;

		m_editor->GetProjectManager().GetProjectData()->GetResourceRoot().FindResourceDirectory(m_currentWidget->GetID());

		Widget* sourceWidget = &m_currentWidget->GetRoot();
		Widget* rootInEditor = CommonWidgets::BuildDefaultFoldItem(this, sourceWidget, Theme::GetDef().baseIndent, "", Color::White, sourceWidget->GetWidgetProps().debugName, true, &sourceWidget->_fold, true);

		m_hierarchyController->AddItem(rootInEditor->GetChildren().front());
		m_hierarchyLayout->AddChild(rootInEditor);

		AddItemForWidget(rootInEditor, sourceWidget, Theme::GetDef().baseIndent * 2);
	}

	void PanelWidgetEditor::AddItemForWidget(Widget* rootInEditor, Widget* sourceWidget, float margin)
	{
		for (Widget* c : sourceWidget->GetChildren())
		{
			const std::type_info& typeInfo = typeid(c);
			const String		  typeName = typeInfo.name();
			Widget*				  item	   = CommonWidgets::BuildDefaultFoldItem(this, c, margin, "", Color::White, c->GetWidgetProps().debugName, !c->GetChildren().empty(), &c->_fold, false);

			Text* classType			   = m_manager->Allocate<Text>();
			classType->GetProps().text = "(" + ReflectionSystem::Get().Resolve(c->GetTID()).GetProperty<String>("Title"_hs) + ")";
			classType->GetFlags().Set(WF_POS_ALIGN_Y);
			classType->SetAlignedPosY(0.5f);
			classType->SetAnchorY(Anchor::Center);
			classType->Initialize();
			item->GetChildren().front()->AddChild(classType);

			m_hierarchyController->AddItem(item->GetChildren().front());
			rootInEditor->AddChild(item);
			AddItemForWidget(item, c, margin + Theme::GetDef().baseIndent);
		}
	}

	void PanelWidgetEditor::CheckSaveCurrent(Delegate<void()>&& onAct)
	{
		if (m_currentWidget == nullptr)
		{
			onAct();
			return;
		}

		// GenericPopup* p = CommonWidgets::ThrowGenericPopup(Locale::GetStr(LocaleStr::SaveChanges), Locale::GetStr(LocaleStr::SaveChangesDesc), //ICON_LINA_LOGO, this);
		// p->AddButton(GenericPopup::ButtonProps{
		//	.text = Locale::GetStr(LocaleStr::Yes),
		//	.onClicked =
		//		[onAct, this, p]() {
		//			CloseCurrent(true);
		//			onAct();
		//			m_manager->AddToKillList(p);
		//		},
		// });
		//
		// p->AddButton(GenericPopup::ButtonProps{
		//	.text = Locale::GetStr(LocaleStr::No),
		//	.onClicked =
		//		[onAct, this, p]() {
		//			CloseCurrent(false);
		//			onAct();
		//			m_manager->AddToKillList(p);
		//		},
		//});
		//
		// m_manager->AddToForeground(p);
	}

	void PanelWidgetEditor::OnFileMenuGetItems(FileMenu* filemenu, StringID sid, Vector<FileMenuItem::Data>& outData, void* userData)
	{
		if (m_currentWidget == nullptr)
			return;

		bool deleteDisabled	   = false;
		bool duplicateDisabled = false;
		bool renameDisabled	   = false;

		Vector<Widget*> selection = m_hierarchyController->GetSelectedUserData<Widget>();

		if (selection.empty())
			return;

		if (selection.size() > 1)
		{
			renameDisabled = true;
		}

		Widget* currentRoot = &m_currentWidget->GetRoot();

		auto it = linatl::find_if(selection.begin(), selection.end(), [currentRoot](Widget* w) -> bool { return w == currentRoot; });
		if (it != selection.end())
		{
			deleteDisabled	  = true;
			duplicateDisabled = true;
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

			outData.push_back(FileMenuItem::Data{
				.text		 = Locale::GetStr(LocaleStr::Refresh),
				.hasDropdown = false,
				.isDisabled	 = false,
				.userData	 = userData,
			});

			outData.push_back(FileMenuItem::Data{.isDivider = true});

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
		}
	}

	bool PanelWidgetEditor::OnFileMenuItemClicked(FileMenu* filemenu, StringID sid, void* userData)
	{
		if (m_currentWidget == nullptr)
			return false;

		Vector<Widget*> selection = m_hierarchyController->GetSelectedUserData<Widget>();

		if (sid == TO_SID(Locale::GetStr(LocaleStr::Rename)))
		{
			RequestRename(selection.front());
			return true;
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::Delete)))
		{
			RequestDelete(selection);
			return true;
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::Duplicate)))
		{
			RequestDuplicate(selection);
			return true;
		}

		if (sid == TO_SID(Locale::GetStr(LocaleStr::Refresh)))
		{
			for (Widget* w : selection)
				w->Initialize();
			return true;
		}

		return false;
	}

	void PanelWidgetEditor::RequestDelete(Vector<Widget*> widgets)
	{
		Vector<Widget*> selection	= m_hierarchyController->GetSelectedUserData<Widget>();
		Widget*			currentRoot = &m_currentWidget->GetRoot();

		if (selection.empty())
			return;

		auto it = linatl::find_if(selection.begin(), selection.end(), [currentRoot](Widget* w) -> bool { return w == currentRoot; });
		if (it != selection.end())
		{
			return;
		}

		Vector<Widget*> roots;
		for (Widget* w : selection)
		{
			auto it = linatl::find_if(selection.begin(), selection.end(), [w](Widget* selected) -> bool { return selected == w->GetParent(); });
			if (it == selection.end())
				roots.push_back(w);
		}

		for (Widget* r : roots)
		{
			r->GetParent()->RemoveChild(r);
			r->GetWidgetManager()->Deallocate(r);
		}

		RefreshHierarchy();
	}

	void PanelWidgetEditor::RequestDuplicate(Vector<Widget*> widgets)
	{
		Vector<Widget*> selection	= m_hierarchyController->GetSelectedUserData<Widget>();
		Widget*			currentRoot = &m_currentWidget->GetRoot();

		if (selection.empty())
			return;

		auto it = linatl::find_if(selection.begin(), selection.end(), [currentRoot](Widget* w) -> bool { return w == currentRoot; });
		if (it != selection.end())
		{
			return;
		}

		Vector<Widget*> roots;
		for (Widget* w : selection)
		{
			auto it = linatl::find_if(selection.begin(), selection.end(), [w](Widget* selected) -> bool { return selected == w->GetParent(); });
			if (it == selection.end())
				roots.push_back(w);
		}

		for (Widget* r : roots)
		{
			Widget* copy = r->GetWidgetManager()->Allocate(r->GetTID());
			OStream stream;
			r->SaveToStream(stream);
			IStream istream;
			istream.Create(stream.GetDataRaw(), stream.GetCurrentSize());
			copy->LoadFromStream(istream);
			stream.Destroy();
			istream.Destroy();
			r->GetParent()->AddChild(copy);
		}

		RefreshHierarchy();
	}

	void PanelWidgetEditor::RequestRename(Widget* w)
	{
		Widget* parent = m_hierarchyController->GetItem(w);
		Text*	text   = parent->GetWidgetOfType<Text>(parent);

		InputField* inp = m_manager->Allocate<InputField>();
		inp->GetFlags().Set(WF_USE_FIXED_SIZE_Y);
		inp->GetText()->GetProps().text = text->GetProps().text;
		inp->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		inp->SetSizeX(parent->GetSizeX());
		inp->SetPos(text->GetParent()->GetPos());
		inp->SetFixedSizeY(text->GetParent()->GetSizeY());
		inp->Initialize();

		inp->GetProps().onEditEnd = [text, inp, w, parent, this](const String& str) {
			text->GetProps().text = str;
			text->CalculateTextSize();
			w->GetWidgetProps().debugName = str;
			text->GetWidgetManager()->AddToKillList(inp);
		};

		text->GetWidgetManager()->AddToForeground(inp);
		inp->StartEditing();
		inp->SelectAll();

		m_hierarchyController->SetFocus(true);
		m_manager->GrabControls(inp);
	}

	void PanelWidgetEditor::OpenWidget(ResourceID id)
	{
		m_propertiesArea->Refresh(nullptr);
		m_currentWidget = m_editor->GetResourceManagerV2().OpenResource<GUIWidget>(m_editor->GetProjectManager().GetProjectData(), id, (void*)m_manager);

		RefreshHierarchy();

		if (m_currentWidget == nullptr)
			return;

		m_lastOpenWidget = id;

		Widget* root = &m_currentWidget->GetRoot();
		m_gridParent->AddChild(root);
		root->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		root->SetAlignedPos(Vector2::Zero);
		root->SetAlignedSize(Vector2::One);
		m_propertiesArea->Refresh(root);
	}

	void PanelWidgetEditor::CloseCurrent(bool save)
	{
		if (save)
			m_editor->GetResourceManagerV2().CloseResource(m_editor->GetProjectManager().GetProjectData(), m_currentWidget, true);
		else
			m_editor->GetResourceManagerV2().CloseResource(m_editor->GetProjectManager().GetProjectData(), m_currentWidget, false);

		m_gridParent->RemoveChild(&m_currentWidget->GetRoot());
	}

	bool PanelWidgetEditor::OnMouse(uint32 button, LinaGX::InputAction act)
	{
		if (act != LinaGX::InputAction::Pressed)
			return false;

		if (m_currentWidget == nullptr)
			return false;

		if (!m_currentWidget->GetRoot().GetIsHovered())
			return false;

		Widget* deepest = m_currentWidget->GetRoot().FindDeepestHovered();
		m_hierarchyController->SelectItem(m_hierarchyController->GetItem(deepest), !m_lgxWindow->GetInput()->GetKey(LINAGX_KEY_LCTRL), false, true);
		return true;
	}

	void PanelWidgetEditor::AddPayloadToWidget(Widget* target)
	{
		if (m_payloadCarryWidget != nullptr)
		{
			if (target == m_payloadCarryWidget)
				return;

			m_payloadCarryWidget->GetParent()->RemoveChild(m_payloadCarryWidget);
			target->AddChild(m_payloadCarryWidget);
			m_payloadCarryWidget->GetParent()->Initialize();
			m_payloadCarryWidget = nullptr;
			RefreshHierarchy();
			return;
		}

		Widget* w = m_manager->Allocate(m_payloadCarryTID);
		w->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		w->SetAlignedPos(Vector2::Zero);
		w->SetAlignedSizeX(1.0f);
		w->SetFixedSizeY(Theme::GetDef().baseItemHeight);

		if (m_payloadCarryTID == GetTypeID<Text>())
		{
			w->GetFlags() = 0;
			w->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
			static_cast<Text*>(w)->GetProps().text = "Text";
			static_cast<Text*>(w)->CalculateTextSize();
		}
		else if (m_payloadCarryTID == GetTypeID<Icon>())
		{
			w->GetFlags() = 0;
			w->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
			static_cast<Icon*>(w)->GetProps().icon = ICON_LINA_LOGO;
			static_cast<Icon*>(w)->CalculateIconSize();
		}

		w->Initialize();
		target->AddChild(w);
		RefreshHierarchy();
		m_hierarchyController->SelectItem(m_hierarchyController->GetItem(w), true, true, true);
	}
} // namespace Lina::Editor
