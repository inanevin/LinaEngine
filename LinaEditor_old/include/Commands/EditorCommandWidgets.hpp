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

#ifndef EditorCommandWidgets_HPP
#define EditorCommandWidgets_HPP

#include "EditorCommand.hpp"
#include "Event/IGameEventListener.hpp"
#include "Data/String.hpp"

namespace Lina::Editor
{
	class GUINode;

	class EditorCommandTextEdit : public EditorCommand
	{
	public:
		EditorCommandTextEdit(Editor* editor, GUINode* node, const String& oldTitle, const String& newTitle) : EditorCommand(editor), m_node(node), m_oldTitle(oldTitle), m_title(newTitle){};
		virtual ~EditorCommandTextEdit(){};

		virtual void Execute(void* userData) override;
		virtual void Undo() override;
		virtual bool OnReferenceDestroyed(void* ref) override;

	private:
		GUINode* m_node		= nullptr;
		String	 m_oldTitle = "";
		String	 m_title	= "";
	};
} // namespace Lina::Editor

#endif
