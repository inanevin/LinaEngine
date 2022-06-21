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