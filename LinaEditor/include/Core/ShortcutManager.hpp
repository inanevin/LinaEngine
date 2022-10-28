/*
Class: ShortcutManager



Timestamp: 12/28/2021 7:57:22 PM
*/

#pragma once

#ifndef ShortcutManager_HPP
#define ShortcutManager_HPP

// Headers here.
#include "Data/Vector.hpp"
#include "Core/EditorCommon.hpp"

namespace Lina
{
    namespace Event
    {
        struct EKeyCallback;
        struct EMouseButtonCallback;
    } // namespace Event
} // namespace Lina

namespace Lina::Editor
{

    class ShortcutManager
    {

    public:
        ShortcutManager()  = default;
        ~ShortcutManager() = default;
        void Initialize();
        void RegisterShortcut(EShortcut shortcut);

    private:
        void OnKeyCallback(const Event::EKeyCallback& ev);
        void OnMouseCallback(const Event::EMouseButtonCallback& ev);

    private:
        Vector<std::pair<EShortcut, bool>> m_shortcuts;
    };
} // namespace Lina::Editor

#endif
