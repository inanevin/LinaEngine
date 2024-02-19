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
#include "Graphics/Core/LGXWrapper.hpp"
#include "LinaGX/Core/InputMappings.hpp"

namespace Lina::Editor
{
	EditorShortcutManager::EditorShortcutManager(Editor* editor) : m_editor(editor)
	{
		m_lgxWrapper = m_editor->GetSystem()->CastSubsystem<LGXWrapper>(SubsystemType::LGXWrapper);
		m_editor->GetSystem()->AddListener(this);

		AddShortcut(Shortcut::CTRL_A, LINAGX_KEY_LCTRL, 0, LINAGX_KEY_A);
		AddShortcut(Shortcut::CTRL_D, LINAGX_KEY_LCTRL, 0, LINAGX_KEY_D);
		AddShortcut(Shortcut::CTRL_S, LINAGX_KEY_LCTRL, 0, LINAGX_KEY_S);
		AddShortcut(Shortcut::CTRL_T, LINAGX_KEY_LCTRL, 0, LINAGX_KEY_T);
		AddShortcut(Shortcut::CTRL_W, LINAGX_KEY_LCTRL, 0, LINAGX_KEY_W);
		AddShortcut(Shortcut::CTRL_Z, LINAGX_KEY_LCTRL, 0, LINAGX_KEY_Z);
		AddShortcut(Shortcut::CTRL_X, LINAGX_KEY_LCTRL, 0, LINAGX_KEY_X);
		AddShortcut(Shortcut::CTRL_C, LINAGX_KEY_LCTRL, 0, LINAGX_KEY_C);
		AddShortcut(Shortcut::CTRL_V, LINAGX_KEY_LCTRL, 0, LINAGX_KEY_V);
		AddShortcut(Shortcut::CTRL_SHIFT_S, LINAGX_KEY_LCTRL, LINAGX_KEY_LSHIFT, LINAGX_KEY_S);
	}

	EditorShortcutManager::~EditorShortcutManager()
	{
		m_shortcuts.clear();
		m_editor->GetSystem()->RemoveListener(this);
	}

	void EditorShortcutManager::OnSystemEvent(SystemEvent eventType, const Event& ev)
	{
		if (eventType & EVS_Key)
		{
			const uint32			  key	 = ev.iParams[0];
			const LinaGX::InputAction action = static_cast<LinaGX::InputAction>(ev.iParams[1]);

			if (action != LinaGX::InputAction::Pressed)
				return;

			Vector<ShortcutEntry*> eligibleEntries;

			for (auto& sc : m_shortcuts)
			{
				if (key == sc.actionKey)
				{
					if (sc.holdKey2 != 0)
					{
						if (!m_lgxWrapper->GetInput()->GetKey(sc.holdKey1) || !m_lgxWrapper->GetInput()->GetKey(sc.holdKey2))
							continue;
					}
					else
					{
						if (!m_lgxWrapper->GetInput()->GetKey(sc.holdKey1))
							continue;
					}

					eligibleEntries.push_back(&sc);
				}
			}

			if (!eligibleEntries.empty())
			{
				// Prioritize 3 keys.
				for (auto e : eligibleEntries)
				{
					if (e->holdKey2 != 0)
					{
						m_editor->OnShortcut(e->shortCut, static_cast<LinaGX::Window*>(ev.pParams[0]));
						return;
					}
				}

				m_editor->OnShortcut(eligibleEntries[0]->shortCut, static_cast<LinaGX::Window*>(ev.pParams[0]));
			}
		}
	}

	void EditorShortcutManager::AddShortcut(Shortcut shortcut, uint32 holdKey1, uint32 holdKey2, uint32 actionKey)
	{
		ShortcutEntry entry = ShortcutEntry{
			.shortCut  = shortcut,
			.holdKey1  = holdKey1,
			.holdKey2  = holdKey2,
			.actionKey = actionKey,
		};

		m_shortcuts.push_back(entry);
	}

} // namespace Lina::Editor
