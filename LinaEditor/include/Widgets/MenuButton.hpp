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

/*
Class: Menu

Menu button elements are used for creating a custom menu barç

Timestamp: 10/8/2020 9:02:33 PM
*/

#pragma once

#ifndef MenuButton_HPP
#define MenuButton_HPP

#include "Math/Color.hpp"
#include <vector>

namespace Lina::Editor
{

    enum class MenuBarElementType
    {
        None           = 0,
        NewProject     = 1,
        LoadProject    = 2,
        SaveProject    = 3,
        PackageProject = 4,

        Edit = 10,
        View = 20,

        SaveLevel = 30,
        LoadLevel = 31,
        NewLevel  = 32,

        EntitiesPanel       = 40,
        HeaderPanel         = 11,
        LogPanel            = 42,
        PropertiesPanel     = 44,
        ResourcesPanel      = 45,
        LevelPanel          = 46,
        ProfilerPanel       = 47,
        GlobalSettingsPanel = 48,
        SystemsPanel        = 49,
        ImGuiPanel          = 50,
        TextEditorPanel     = 51,

        DebugViewShadows = 62,
        DebugViewNormal  = 63,

        Empty    = 80,
        Cube     = 81,
        Sphere   = 82,
        Capsule  = 83,
        Cylinder = 84,
        Plane    = 85,
        Quad     = 86,

        PLight = 100,
        SLight = 101,
        DLight = 102,

        Github  = 110,
        Website = 111,

        Resources_ShowEditorFolders        = 120,
        Resources_ShowEngineFolders        = 121,
        Resources_CreateNewFolder          = 122,
        Resources_CreateNewMaterial        = 123,
        Resources_CreateNewPhysicsMaterial = 124,
        Resources_Rescan                   = 125,
    };

    class MenuBarElement
    {

    public:
        MenuBarElement(const char* icon, const char* title, const char* tooltip, int groupID, MenuBarElementType elem = MenuBarElementType::None, bool ownsChildren = true, bool tooltipIsIcon = false)
            : m_icon(icon), m_title(title), m_tooltip(tooltip), m_groupID(groupID), m_type(elem), m_ownsChildren(ownsChildren), m_tooltipIsIcon(tooltipIsIcon){};
        ~MenuBarElement();

        void AddChild(MenuBarElement* child);
        void Draw();

        int         m_groupID       = -1;
        const char* m_title         = "";
        const char* m_tooltip       = "";
        bool        m_tooltipIsIcon = false;

    private:
        bool                         m_ownsChildren = true;
        MenuBarElementType           m_type         = MenuBarElementType::None;
        const char*                  m_icon         = "";
        std::vector<MenuBarElement*> m_children;
    };

    class Menu
    {
    public:
        Menu(const char* title)
            : m_title(title){};
        ~Menu();

        void AddElement(MenuBarElement* elem);
        void Draw();

        const char* m_title = "";

    private:
        std::vector<MenuBarElement*> m_elements;
    };

} // namespace Lina::Editor

#endif
