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

#ifndef EditorShortcutManager_HPP
#define EditorShortcutManager_HPP

#include "Event/ISystemEventListener.hpp"
#include "Core/Common.hpp"
#include "Data/Vector.hpp"
#include "Core/EditorCommon.hpp"


namespace Lina::Editor
{
	class Editor;

	struct ShortcutEntry
	{
		Shortcut shortCut  = Shortcut::None;
		uint32	 holdKey1  = 0;
		uint32	 holdKey2  = 0;
		uint32	 actionKey = 0;
	};

	class EditorShortcutManager : public ISystemEventListener
	{
	public:
		EditorShortcutManager(Editor* editor);
		~EditorShortcutManager();
		virtual void OnSystemEvent(SystemEvent eventType, const Event& ev);

		void AddShortcut(Shortcut shortcut, uint32 holdKey1, uint32 holdKey2, uint32 actionKey);

		virtual Bitmask32 GetSystemEventMask()
		{
			return EVS_Key;
		}

	private:
		Vector<ShortcutEntry> m_shortcuts;
		Editor*				  m_editor = nullptr;
		Input*				  m_input  = nullptr;
	};
} // namespace Lina::Editor

#endif
