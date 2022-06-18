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

/*
Class: ResourcesPanel

Displays the project folders & files, creates functionality to edit, add, remove them.

Timestamp: 6/5/2020 12:54:52 AM
*/

#pragma once

#ifndef ResourcesPanel_HPP
#define ResourcesPanel_HPP

#include "Core/EditorCommon.hpp"
#include "Panels/EditorPanel.hpp"
#include "Data/Vector.hpp"
#include <map>

namespace Lina
{
    namespace Event
    {
        struct EKeyCallback;
    }

    namespace Resources
    {
        class ResourceStorage;
    }

    namespace Utility
    {
        struct Folder;
        struct File;
    } // namespace Utility
} // namespace Lina

namespace Lina::Editor
{

    struct EMenuBarElementClicked;
    class Menu;
    class PropertiesPanel;

    class ResourcesPanel : public EditorPanel
    {

    public:
        ResourcesPanel() = default;
        virtual ~ResourcesPanel();

        virtual void Initialize(const char* id, const char* icon) override;
        virtual void Draw() override;

    private:
        void HandleLeftPaneResize(bool canResize);

        void DrawLeftPane();
        void DrawRightPane();

        void DrawFolderHierarchy(Utility::Folder* folder, bool performFilterCheck);
        void DrawContents(Utility::Folder* folder);
        void DrawFile(Utility::File& file);

    private:
        void OnMenuBarElementClicked(const EMenuBarElementClicked& ev);
        void DeselectNodes(bool deselectAll);
        void DrawContextMenu();
        bool ContextMenuCanAddAsset();

    private:
        std::string                 m_folderSearchFilter        = "";
        std::string                 m_fileSearchFilter          = "";
        bool                        m_draggingChildWindowBorder = false;
        bool                        m_showEditorFolders         = true;
        bool                        m_showEngineFolders         = true;
        bool                        m_leftPaneFocused           = false;
        bool                        m_rightPaneFocused          = false;
        float                       m_leftPaneWidth             = 280.0f;
        float                       m_leftPaneMinWidth          = 200.0f;
        float                       m_leftPaneMaxWidth          = 500.0f;
        float                       m_nodeSizes                 = 1.0f;
        Utility::Folder*            m_rootFolder                = nullptr;
        Utility::Folder*            m_selectedFolder            = nullptr;
        Utility::File*              m_selectedFile              = nullptr;
        Utility::Folder*            m_selectedSubfolder         = nullptr;
        Resources::ResourceStorage* m_storage                   = nullptr;
        Menu*                       m_contextMenu               = nullptr;
        float                       m_resizeDividerPressedPos   = 0.0f;
    };
} // namespace Lina::Editor

#endif
