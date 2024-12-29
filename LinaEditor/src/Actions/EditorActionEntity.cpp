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

#include "Editor/Actions/EditorActionEntity.hpp"
#include "Editor/Editor.hpp"
#include "Editor/Widgets/World/WorldController.hpp"
#include "Core/World/Entity.hpp"

namespace Lina::Editor
{

	EditorActionEntitySelection* EditorActionEntitySelection::Create(Editor* editor, uint64 m_worldId, const Vector<Entity*>& previousSelection, const Vector<Entity*>& currentSelection)
	{
		EditorActionEntitySelection* action = new EditorActionEntitySelection();

		action->m_prevSelected.reserve(previousSelection.size());
		action->m_selected.reserve(currentSelection.size());
		action->m_worldId = m_worldId;

		for (Entity* prev : previousSelection)
			action->m_prevSelected.push_back(prev->GetGUID());

		for (Entity* prev : currentSelection)
			action->m_selected.push_back(prev->GetGUID());

		editor->GetEditorActionManager().AddToStack(action);
		return action;
	}

	void EditorActionEntitySelection::Execute(Editor* editor, ExecType type)
	{
		WorldController* controller = editor->GetWorldManager().FindWorldController(m_worldId);
		if (!controller)
			return;

		if (type == ExecType::Undo)
		{
			controller->OnActionEntitySelection(m_prevSelected);
		}
		else if (type == ExecType::Redo)
		{
			controller->OnActionEntitySelection(m_selected);
		}
	}

} // namespace Lina::Editor
