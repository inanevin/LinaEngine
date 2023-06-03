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

#pragma once

#ifndef EditorCommandManager_HPP
#define EditorCommandManager_HPP

#include "Data/String.hpp"
#include "Data/Deque.hpp"

namespace Lina
{
	class Entity;
}

namespace Lina::Editor
{
	class Editor;
	class EditorCommand;
	class GUINode;

	class EditorCommandManager
	{
	public:
		EditorCommandManager(Editor* editor);
		~EditorCommandManager();

		void AddCommand(EditorCommand* cmd);
		void Undo();
		void OnReferenceDestroyed(void* ptr);

		inline Editor* GetEditor() const
		{
			return m_editor;
		}

	private:
		Editor*				  m_editor = nullptr;
		Deque<EditorCommand*> m_commandStack;
	};
} // namespace Lina::Editor

#endif
