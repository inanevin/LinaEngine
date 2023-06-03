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

#include "Commands/EditorCommandManager.hpp"
#include "Commands/EditorCommandEntity.hpp"
#include "Data/CommonData.hpp"

namespace Lina::Editor
{

#define MAX_QUEUE_SIZE 15

	EditorCommandManager::EditorCommandManager(Editor* editor) : m_editor(editor)
	{
	}

	EditorCommandManager::~EditorCommandManager()
	{
		while (!m_commandStack.empty())
		{
			EditorCommand* cmd = m_commandStack.front();
			delete cmd;
			m_commandStack.pop_front();
		}
	}

	void EditorCommandManager::AddCommand(EditorCommand* cmd)
	{
		cmd->Execute(nullptr);
		m_commandStack.push_front(cmd);

		while (m_commandStack.size() > MAX_QUEUE_SIZE)
		{
			EditorCommand* cmd = m_commandStack.back();
			m_commandStack.pop_back();
			delete cmd;
		}
	}

	void EditorCommandManager::Undo()
	{
		if (m_commandStack.empty())
			return;

		auto cmd = m_commandStack.front();
		cmd->Undo();
		delete cmd;
		m_commandStack.pop_front();
	}

	void EditorCommandManager::OnReferenceDestroyed(void* ptr)
	{
		Deque<EditorCommand*> newStack;

		for (auto s : m_commandStack)
		{
			if (!s->OnReferenceDestroyed(ptr))
				newStack.push_front(s);
			else
				delete s;
		}

		m_commandStack = newStack;
	}

} // namespace Lina::Editor
