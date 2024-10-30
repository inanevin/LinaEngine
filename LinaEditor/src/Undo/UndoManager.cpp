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

#include "Editor/Undo/UndoManager.hpp"
#include "Editor/Undo/UndoAction.hpp"

namespace Lina::Editor
{
	void UndoManager::Initialize(Editor* editor)
	{
		m_editor = editor;
	}

	void UndoManager::Shutdown()
	{
		while (!m_undoStack.empty())
		{
			UndoAction* action = m_undoStack.top();
			m_undoStack.pop();
			delete action;
		}
	}
	void UndoManager::AddToStack(UndoAction* action)
	{
		m_undoStack.push(action);
		action->Execute(m_editor);
	}

	void UndoManager::Undo()
	{
		if (m_undoStack.empty())
			return;

		UndoAction* action = m_undoStack.top();
		m_undoStack.pop();

		action->Undo(m_editor);
		LINA_TRACE("Editor -> undo action!");
		delete action;
	}
} // namespace Lina::Editor
