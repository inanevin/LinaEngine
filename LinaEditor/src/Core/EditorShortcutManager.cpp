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

#include "Core/EditorShortcutManager.hpp"
#include "Core/Editor.hpp"
#include "System/ISystem.hpp"
#include "Input/Core/Input.hpp"
#include "Input/Core/InputMappings.hpp"

namespace Lina::Editor
{
	EditorShortcutManager::EditorShortcutManager(Editor* editor) : m_editor(editor)
	{
		m_input = m_editor->GetSystem()->CastSubsystem<Input>(SubsystemType::Input);
		m_editor->GetSystem()->AddListener(this);
	}

	EditorShortcutManager::~EditorShortcutManager()
	{
		m_editor->GetSystem()->RemoveListener(this);
	}

	void EditorShortcutManager::OnSystemEvent(SystemEvent eventType, const Event& ev)
	{
		if (eventType & EVS_Key)
		{
			const uint32	  key	 = ev.iParams[0];
			const InputAction action = static_cast<InputAction>(ev.iParams[1]);

			if (action != InputAction::Pressed)
				return;

			for (auto& sc : m_shortcuts)
			{
				if (key == sc.keySecondary && m_input->GetKey(sc.keyPrimary))
				{
					m_editor->OnShortcut(sc.shortCut, ev.pParams[0]);
					return;
				}
			}
		}
	}

} // namespace Lina::Editor
