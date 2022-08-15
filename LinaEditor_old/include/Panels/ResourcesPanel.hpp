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
#include "Data/Map.hpp"

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
        String                 m_folderSearchFilter        = "";
        String                 m_fileSearchFilter          = "";
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
