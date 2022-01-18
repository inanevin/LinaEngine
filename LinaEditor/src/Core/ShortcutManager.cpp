/* 
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

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

#include "Core/ShortcutManager.hpp"
#include "Core/InputMappings.hpp"
#include "EventSystem/EventSystem.hpp"
#include "EventSystem/InputEvents.hpp"
#include "Core/EditorCommon.hpp"

namespace Lina::Editor
{
    void ShortcutManager::Initialize()
    {
        m_eventSystem = Event::EventSystem::Get();
        m_eventSystem->Connect<Event::EKeyCallback, &ShortcutManager::OnKeyCallback>(this);
        m_eventSystem->Connect<Event::EMouseButtonCallback, &ShortcutManager::OnMouseCallback>(this);

        RegisterShortcut(EShortcut{"duplicate", LINA_KEY_LCTRL, LINA_KEY_D});
        RegisterShortcut(EShortcut{"playmode", LINA_KEY_LCTRL, LINA_KEY_SPACE});
        RegisterShortcut(EShortcut{"create_empty", LINA_KEY_LCTRL, LINA_KEY_E});
        RegisterShortcut(EShortcut{"save", LINA_KEY_LCTRL, LINA_KEY_S});
        RegisterShortcut(EShortcut{"reload", LINA_KEY_LCTRL, LINA_KEY_R});
    }

    void ShortcutManager::RegisterShortcut(EShortcut& shortcut)
    {
        m_shortcuts.push_back(std::make_pair(shortcut, false));
    }

    void ShortcutManager::OnKeyCallback(const Event::EKeyCallback& ev)
    {
        for (auto& shortcut : m_shortcuts)
        {
            if (ev.m_action == Input::InputAction::Pressed)
            {
                if (shortcut.first.m_heldKey == ev.m_key)
                    shortcut.second = true;
                else if (shortcut.first.m_pressedKey == ev.m_key && shortcut.second)
                {
                    m_eventSystem->Trigger<EShortcut>(shortcut.first);
                }
            }
            else if (ev.m_action == Input::InputAction::Released)
            {
                if (shortcut.first.m_heldKey == ev.m_key && shortcut.second)
                    shortcut.second = false;
            }
        }
    }

    void ShortcutManager::OnMouseCallback(const Event::EMouseButtonCallback& ev)
    {
    }

} // namespace Lina::Editor