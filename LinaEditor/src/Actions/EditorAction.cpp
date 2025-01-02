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

#include "Editor/Actions/EditorAction.hpp"
#include "Editor/Editor.hpp"

namespace Lina::Editor
{
	EditorActionCollective::~EditorActionCollective()
	{
		for (EditorAction* act : m_actions)
			delete act;
		m_actions.clear();
	}

	EditorActionCollective* EditorActionCollective::Create(Editor* editor, uint32 lastActions)
	{
		EditorActionCollective* collective = new EditorActionCollective();
		editor->GetEditorActionManager().PopLastActions(lastActions, collective->m_actions);
		editor->GetEditorActionManager().AddToStack(collective);
		return collective;
	}

	void EditorActionCollective::Execute(Editor* editor, ExecType type)
	{
		if (type == ExecType::Undo)
		{
			for (EditorAction* act : m_actions)
				act->Execute(editor, ExecType::Undo);
		}
		else if (type == ExecType::Redo)
		{
			const int32 sz = static_cast<int32>(m_actions.size());
			for (int32 i = sz - 1; i > -1; i--)
				m_actions.at(i)->Execute(editor, ExecType::Redo);
		}
	}
} // namespace Lina::Editor
