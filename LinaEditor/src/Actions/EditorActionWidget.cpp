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

#include "Editor/Actions/EditorActionWidget.hpp"
#include "Editor/Editor.hpp"
#include "Editor/Widgets/World/WorldController.hpp"
#include "Editor/Widgets/Panel/PanelWidgetEditor.hpp"
#include "Core/Meta/ProjectData.hpp"

namespace Lina::Editor
{

	EditorActionWidgetSelection* EditorActionWidgetSelection::Create(Editor* editor, ResourceID id, const Vector<Widget*>& previous, const Vector<Widget*>& current)
	{
		EditorActionWidgetSelection* action = new EditorActionWidgetSelection();
		action->m_resourceID				= id;

		for (Widget* w : previous)
			action->m_previous.push_back(w->GetUniqueID());

		for (Widget* w : current)
			action->m_current.push_back(w->GetUniqueID());

		editor->GetEditorActionManager().AddToStack(action);

		return nullptr;
	}

	void EditorActionWidgetSelection::Execute(Editor* editor, ExecType type)
	{
		Panel* panel = editor->GetWindowPanelManager().FindPanelOfType(PanelType::WidgetEditor, m_resourceID);
		if (!panel)
			return;

		PanelWidgetEditor* widgetEditor = static_cast<PanelWidgetEditor*>(panel);
		GUIWidget*		   guiWidget	= static_cast<GUIWidget*>(widgetEditor->GetResource());
		Widget*			   root			= &guiWidget->GetRoot();

		if (type == ExecType::Undo)
		{
			Vector<Widget*> selection;

			for (uint32 id : m_previous)
			{
				if (id == root->GetUniqueID())
				{
					selection.push_back(root);
					continue;
				}

				Widget* w = root->FindChildWithUniqueID(id);

				if (w)
					selection.push_back(w);
			}

			widgetEditor->ChangeSelection(selection);
		}
		else if (type == ExecType::Redo)
		{
			Vector<Widget*> selection;

			for (uint32 id : m_current)
			{
				if (id == root->GetUniqueID())
				{
					selection.push_back(root);
					continue;
				}

				Widget* w = root->FindChildWithUniqueID(id);

				if (w)
					selection.push_back(w);
			}

			widgetEditor->ChangeSelection(selection);
		}

	}

	EditorActionWidgetChanged* EditorActionWidgetChanged::Create(Editor* editor, ResourceID id, Widget* widget, OStream& prevStream)
	{
		EditorActionWidgetChanged* action = new EditorActionWidgetChanged();
		action->m_prevStream.Create(prevStream.GetDataRaw(), prevStream.GetCurrentSize());
		OStream stream;
		widget->SaveToStream(stream);
		action->m_stream.Create(stream.GetDataRaw(), stream.GetCurrentSize());
		stream.Destroy();

		action->m_resourceID = id;
		editor->GetEditorActionManager().AddToStack(action);
		return action;
	}

	void EditorActionWidgetChanged::Execute(Editor* editor, ExecType type)
	{
		Panel* panel = editor->GetWindowPanelManager().FindPanelOfType(PanelType::WidgetEditor, m_resourceID);
		if (!panel)
			return;

		PanelWidgetEditor* widgetEditor = static_cast<PanelWidgetEditor*>(panel);

		if (type == ExecType::Undo)
			widgetEditor->SetStream(m_prevStream);
		else if (type == ExecType::Redo)
			widgetEditor->SetStream(m_stream);

		widgetEditor->RefreshBrowser();
		widgetEditor->StoreEditorActionBuffer();

		GUIWidget* w = static_cast<GUIWidget*>(widgetEditor->GetResource());
		w->SaveToFileAsBinary(editor->GetProjectManager().GetProjectData()->GetResourcePath(w->GetID()));
	}

} // namespace Lina::Editor
