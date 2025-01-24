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
#include "Editor/Editor.hpp"
#include "Editor/Actions/EditorActionWidget.hpp"
#include "Editor/Widgets/CommonWidgets.hpp"
#include "Editor/Widgets/Compound/WidgetBrowser.hpp"
#include "Editor/Widgets/Layout/ItemController.hpp"
#include "Editor/Actions/EditorActionResources.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/Application.hpp"
#include "Core/Meta/ProjectData.hpp"
#include "Common/Math/Math.hpp"

namespace Lina::Editor
{

	namespace
	{
		bool ParentInList(const Vector<Widget*>& list, Widget* w)
		{
			Widget* parent = w->GetParent();
			auto	it	   = linatl::find_if(list.begin(), list.end(), [parent](Widget* widg) -> bool { return parent == widg; });
			if (it != list.end())
				return true;

			if (parent)
				return ParentInList(list, parent);

			return false;
		}

		Vector<Widget*> GetRoots(const Vector<Widget*>& widgets)
		{
			Vector<Widget*> roots;

			for (Widget* w : widgets)
			{
				if (ParentInList(widgets, w))
					continue;

				roots.push_back(w);
			}

			return roots;
		}

		Vector<Widget*> RemoveRoot(const Vector<Widget*>& widgets, Widget* root)
		{
			Vector<Widget*> retVal;
			retVal.reserve(widgets.size());
			for (Widget* w : widgets)
			{
				if (w == root)
					continue;
				retVal.push_back(w);
			}
			return retVal;
		}

		void FindMaxUniqueID(Widget* root, uint32& id)
		{
			id = Math::Max(root->GetUniqueID(), id);
			for (Widget* c : root->GetChildren())
			{
				FindMaxUniqueID(c, id);
			}
		}

	} // namespace

	void PanelWidgetEditor::Construct()
	{
		PanelResourceViewer::Construct();

		m_horizontal->DeallocAllChildren();
		m_horizontal->RemoveAllChildren();

		m_browser = m_manager->Allocate<WidgetBrowser>("Browser");
		m_browser->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		m_browser->SetAlignedPos(Vector2::Zero);
		m_browser->SetAlignedSize(Vector2(0.3f, 1.0f));
		m_browser->SetWidgetEditor(this);
		m_horizontal->AddChild(m_browser);
		AddResourceBGAndInspector(0.3f);

		m_root = m_manager->Allocate<Widget>("Root");
		m_root->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		m_root->SetAlignedPos(Vector2::Zero);
		m_root->SetAlignedSize(Vector2::One);
		m_resourceBG->AddChild(m_root);
	}

	void PanelWidgetEditor::Initialize()
	{
		PanelResourceViewer::Initialize();

		if (!m_resource)
			return;

		FetchRootFromResource();
		RefreshResources();

		m_uniqueIDCounter = 0;
		FindMaxUniqueID(m_root, m_uniqueIDCounter);

		m_browser->SetRoot(m_root);
		m_browser->RefreshWidgets();

		StoreEditorActionBuffer();
		RebuildContents();
	}

	void PanelWidgetEditor::Draw()
	{
		LinaVG::StyleOptions opts;
		opts.isFilled = false;
		opts.color	  = Theme::GetDef().foreground0.AsLVG4();

		for (Widget* w : m_selectedWidgets)
		{
			m_lvg->DrawRect(w->GetPos().AsLVG(), w->GetRect().GetEnd().AsLVG(), opts, 0.0f, m_drawOrder + 1);
		}
	}

	void PanelWidgetEditor::StoreEditorActionBuffer()
	{
		GUIWidget* guiWidget = static_cast<GUIWidget*>(m_resource);
		OStream	   stream;
		m_root->SaveToStream(stream);
		m_storedStream.Destroy();
		m_storedStream.WriteRaw(stream.GetDataRaw(), stream.GetCurrentSize());
		stream.Destroy();
	}

	void PanelWidgetEditor::RebuildContents()
	{
		m_inspector->DeallocAllChildren();
		m_inspector->RemoveAllChildren();
		if (m_selectedWidgets.empty())
			return;

		GUIWidget* guiWidget = static_cast<GUIWidget*>(m_resource);
		Widget*	   target	 = m_selectedWidgets.at(0);

		DirectionalLayout* panelBase = m_manager->Allocate<DirectionalLayout>();
		panelBase->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_Y_TOTAL_CHILDREN);
		panelBase->SetAlignedSize(Vector2::One);
		panelBase->SetAlignedPosX(0.0f);
		panelBase->GetWidgetProps().childPadding = m_inspector->GetWidgetProps().childPadding;
		panelBase->GetProps().direction			 = DirectionOrientation::Vertical;
		panelBase->GetCallbacks().onEditEnded	 = [this]() {
			   GUIWidget* guiWidget = static_cast<GUIWidget*>(m_resource);
			   EditorActionWidgetChanged::Create(m_editor, m_resourceSpace, m_root, m_storedStream);
		};
		panelBase->GetCallbacks().onEditStarted = [this]() { StoreEditorActionBuffer(); };
		m_inspector->AddChild(panelBase);
		CommonWidgets::BuildClassReflection(panelBase, target, ReflectionSystem::Get().Resolve<Widget>());

		if (target->GetTID() != GetTypeID<Widget>())
		{
			m_inspector->AddChild(CommonWidgets::BuildSeperator(m_inspector));
			DirectionalLayout* panelSpec = m_manager->Allocate<DirectionalLayout>();
			panelSpec->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_Y_TOTAL_CHILDREN);
			panelSpec->SetAlignedSize(Vector2::One);
			panelSpec->SetAlignedPosX(0.0f);
			panelSpec->GetWidgetProps().childPadding = m_inspector->GetWidgetProps().childPadding;
			panelSpec->GetProps().direction			 = DirectionOrientation::Vertical;
			panelSpec->GetCallbacks().onEditEnded	 = [this]() {
				   GUIWidget* guiWidget = static_cast<GUIWidget*>(m_resource);
				   EditorActionWidgetChanged::Create(m_editor, m_resourceSpace, m_root, m_storedStream);
				   RefreshResources();
			};
			panelSpec->GetCallbacks().onEditStarted = [this]() { StoreEditorActionBuffer(); };
			m_inspector->AddChild(panelSpec);
			CommonWidgets::BuildClassReflection(panelSpec, target, ReflectionSystem::Get().Resolve(target->GetTID()));
		}
	}

	void PanelWidgetEditor::SetSelectedWidgets(const Vector<Widget*>& selection)
	{
		const Vector<Widget*> prev = m_selectedWidgets;
		m_selectedWidgets		   = selection;
		RebuildContents();
	}

	void PanelWidgetEditor::ChangeSelection(const Vector<Widget*>& selection)
	{
		m_selectedWidgets = selection;
		RebuildContents();
		m_browser->GetController()->UnselectAll();
		for (Widget* w : m_selectedWidgets)
		{
			Widget* item = m_browser->GetController()->GetItem(w);
			if (!item)
				continue;
			m_browser->GetController()->SelectItem(item, false, false, true);
		}
	}

	void PanelWidgetEditor::Duplicate(const Vector<Widget*>& widgets)
	{
		GUIWidget*			  guiWidget	  = static_cast<GUIWidget*>(m_resource);
		const Vector<Widget*> toDuplicate = RemoveRoot(widgets, m_root);

		if (toDuplicate.empty())
			return;

		StoreEditorActionBuffer();

		Vector<Widget*> duplicatedWidgets;
		for (Widget* w : toDuplicate)
		{
			Widget* parent = w->GetParent();
			OStream stream;
			w->SaveToStream(stream);

			IStream istream;
			istream.Create(stream.GetDataRaw(), stream.GetCurrentSize());
			stream.Destroy();

			Widget* duplicated = m_manager->Allocate(w->GetTID(), w->GetWidgetProps().debugName);
			duplicated->LoadFromStream(istream);
			istream.Destroy();
			parent->AddChild(duplicated);
			duplicatedWidgets.push_back(duplicated);
			duplicated->SetUniqueID(++m_uniqueIDCounter);
		}

		RefreshBrowser();
		EditorActionWidgetChanged::Create(m_editor, m_resourceSpace, m_root, m_storedStream);
		EditorActionWidgetSelection::Create(m_editor, m_resourceSpace, duplicatedWidgets);
		EditorActionCollective::Create(m_editor, 2, true);
	}

	void PanelWidgetEditor::Delete(const Vector<Widget*>& widgets)
	{
		GUIWidget*			  guiWidget = static_cast<GUIWidget*>(m_resource);
		const Vector<Widget*> selection = RemoveRoot(widgets, m_root);
		const Vector<Widget*> toDelete	= GetRoots(selection);

		if (toDelete.empty())
			return;

		StoreEditorActionBuffer();

		for (Widget* w : toDelete)
		{
			Widget* parent = w->GetParent();
			parent->RemoveChild(w);
			m_manager->Deallocate(w);
		}

		RefreshBrowser();
		EditorActionWidgetChanged::Create(m_editor, m_resourceSpace, m_root, m_storedStream);
		EditorActionWidgetSelection::Create(m_editor, m_resourceSpace, {m_root});
		EditorActionCollective::Create(m_editor, 2, true);
	}

	void PanelWidgetEditor::Parent(const Vector<Widget*>& widgets, Widget* parent)
	{
		GUIWidget*			  guiWidget = static_cast<GUIWidget*>(m_resource);
		const Vector<Widget*> toParent	= RemoveRoot(widgets, m_root);

		if (toParent.empty())
			return;

		StoreEditorActionBuffer();

		for (Widget* w : toParent)
		{
			if (w == parent)
				continue;

			w->GetParent()->RemoveChild(w);
			parent->AddChild(w);
		}

		RefreshBrowser();
		EditorActionWidgetChanged::Create(m_editor, m_resourceSpace, m_root, m_storedStream);
		EditorActionWidgetSelection::Create(m_editor, m_resourceSpace, widgets);
		EditorActionCollective::Create(m_editor, 2, true);
	}

	void PanelWidgetEditor::RefreshBrowser()
	{
		m_browser->RefreshWidgets();
	}

	void PanelWidgetEditor::AddNew(Widget* w, Widget* parent)
	{
		GUIWidget* guiWidget = static_cast<GUIWidget*>(m_resource);

		if (parent == nullptr)
			parent = m_root;

		parent->AddChild(w);
		w->SetUniqueID(++m_uniqueIDCounter);
		RefreshBrowser();

		EditorActionWidgetChanged::Create(m_editor, m_resourceSpace, m_root, m_storedStream);
		EditorActionWidgetSelection::Create(m_editor, m_resourceSpace, {w});
		EditorActionCollective::Create(m_editor, 2, true);
		RefreshResources();
		m_browser->GetController()->SelectItem(m_browser->GetController()->GetItem(w), true, true, true);
	}

	void PanelWidgetEditor::Rename(Widget* w, const String& name)
	{
		GUIWidget* guiWidget		  = static_cast<GUIWidget*>(m_resource);
		w->GetWidgetProps().debugName = name;
		EditorActionWidgetChanged::Create(m_editor, m_resourceSpace, m_root, m_storedStream);
	}

	void PanelWidgetEditor::RefreshResources()
	{
		HashSet<ResourceID> refs;
		m_root->CollectResourceReferencesRecursive(refs);
		m_editor->GetApp()->GetResourceManager().LoadResourcesFromProject(m_editor->GetProjectManager().GetProjectData(), refs, NULL, m_resourceSpace);
	}

	void PanelWidgetEditor::FetchRootFromResource()
	{
		GUIWidget*		 guiWidget	= static_cast<GUIWidget*>(m_resource);
		const RawStream& rootStream = guiWidget->GetStream();

		if (!rootStream.IsEmpty())
		{
			IStream stream;
			stream.Create(rootStream.GetRaw(), rootStream.GetSize());
			m_root->LoadFromStream(stream);
			stream.Destroy();
		}
	}

} // namespace Lina::Editor
