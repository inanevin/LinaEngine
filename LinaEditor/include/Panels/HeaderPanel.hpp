/*
Class: HeaderPanel

Draws the top header and its components like title, lina icon, window buttons, menubar etc.

Timestamp: 10/8/2020 1:39:03 PM
*/

#pragma once

#ifndef HeaderPanel_HPP
#define HeaderPanel_HPP

#include "Panels/EditorPanel.hpp"
#include "Data/Vector.hpp"

namespace Lina::Editor
{
    class Menu;
    class MenuBarElement;
    enum class MenuBarElementType;

    class HeaderPanel : public EditorPanel
    {

    public:
        HeaderPanel() = default;
        virtual ~HeaderPanel();

        virtual void Initialize(const char* id, const char* icon) override;
        virtual void Draw() override;

        static Vector<MenuBarElement*>& GetCreateEntityElements()
        {
            return s_createEntityElements;
        }

    private:
        void DrawMenuBarChild();

    private:
        static Vector<MenuBarElement*> s_createEntityElements;
        String                         m_title = "";
        Vector<Menu*>            m_menuButtons;
    };
} // namespace Lina::Editor

#endif
