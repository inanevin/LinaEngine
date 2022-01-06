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

#include "Panels/ResourcesPanel.hpp"
#include "Core/CommonResources.hpp"
#include "EventSystem/InputEvents.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "Core/Application.hpp"
#include "Core/EditorApplication.hpp"
#include "Core/EditorCommon.hpp"
#include "Core/GUILayer.hpp"
#include "Core/InputMappings.hpp"
#include "Core/RenderEngineBackend.hpp"
#include "IconsFontAwesome5.h"
#include "Math/Math.hpp"
#include "Panels/PropertiesPanel.hpp"
#include "Rendering/Shader.hpp"
#include "Utility/EditorUtility.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "Widgets/MenuButton.hpp"
#include "Widgets/Snackbar.hpp"
#include "Core/ImGuiCommon.hpp"
#include "imgui/imgui.h"

#include <filesystem>

namespace Lina::Editor
{
    Menu*               m_leftPaneMenu        = nullptr;
    MenuBarElement*     m_showEditorFoldersMB = nullptr;
    MenuBarElement*     m_showEngineFoldersMB = nullptr;
    std::vector<TypeID> m_resourceTypesToDraw;

    ResourcesPanel::~ResourcesPanel()
    {
        delete m_leftPaneMenu;
        delete m_contextMenu;
    }

    void ResourcesPanel::Initialize(const char* id, const char* icon)
    {
        EditorPanel::Initialize(id, icon);

        m_leftPaneWidth    = 280.0f;
        m_leftPaneMinWidth = 200.0f;
        m_leftPaneMaxWidth = 500.0f;

        m_storage             = Resources::ResourceStorage::Get();
        m_leftPaneMenu        = new Menu("");
        m_showEditorFoldersMB = new MenuBarElement("", "Show Editor Folders", ICON_FA_CHECK, 0, MenuBarElementType::Resources_ShowEditorFolders, true, true);
        m_showEngineFoldersMB = new MenuBarElement("", "Show Engine Folders", ICON_FA_CHECK, 0, MenuBarElementType::Resources_ShowEngineFolders, true, true);
        m_leftPaneMenu->AddElement(m_showEditorFoldersMB);
        m_leftPaneMenu->AddElement(m_showEngineFoldersMB);
        Event::EventSystem::Get()->Connect<EMenuBarElementClicked, &ResourcesPanel::OnMenuBarElementClicked>(this);
        m_rootFolder = Resources::ResourceManager::Get()->GetRootFolder();

        MenuBarElement* rescan        = new MenuBarElement("", "Rescan", "", 0, MenuBarElementType::Resources_Rescan);
        MenuBarElement* createElement = new MenuBarElement("", "Create", "", 1);
        createElement->AddChild(new MenuBarElement("", "Folder", "", 0, MenuBarElementType::Resources_CreateNewFolder));
        createElement->AddChild(new MenuBarElement("", "Material", "", 1, MenuBarElementType::Resources_CreateNewMaterial));
        createElement->AddChild(new MenuBarElement("", "PhysicsMaterial", "", 2, MenuBarElementType::Resources_CreateNewPhysicsMaterial));
        m_contextMenu = new Menu("res_leftPane_context");
        m_contextMenu->AddElement(rescan);
        m_contextMenu->AddElement(createElement);
    }

    void ResourcesPanel::Draw()
    {
        if (m_show)
        {
            if (Begin())
            {

                const ImVec2 windowPos       = ImGui::GetWindowPos();
                const ImVec2 windowSize      = ImGui::GetWindowSize();
                const ImVec2 cursorPos       = ImGui::GetCursorScreenPos();
                const ImVec2 leftPaneSize    = ImVec2(m_leftPaneWidth, 0);
                const ImVec2 rightPanePos    = ImVec2(cursorPos.x + leftPaneSize.x, cursorPos.y);
                const ImVec2 rightPaneSize   = ImVec2(windowSize.x - leftPaneSize.x, 0);
                bool         anyChildHovered = false;
                bool         isWindowHovered = ImGui::IsWindowHovered();

                ImGui::SetNextWindowPos(ImVec2(cursorPos.x, cursorPos.y));
                ImGui::BeginChild("resources_leftPane", leftPaneSize);
                DrawLeftPane();
                m_leftPaneFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows);

                if (ImGui::IsWindowHovered())
                    anyChildHovered = true;

                ImGui::EndChild();

                ImGui::SameLine();
                WidgetsUtility::IncrementCursorPosX(-ImGui::GetStyle().ItemSpacing.x + 2);

                ImGui::BeginChild("resources_rightPane");
                DrawRightPane();
                m_rightPaneFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows);

                if (ImGui::IsWindowHovered())
                    anyChildHovered = true;

                ImGui::EndChild();
                WidgetsUtility::VerticalResizeDivider(anyChildHovered || isWindowHovered, &m_resizeDividerPressedPos, &m_leftPaneWidth, m_leftPaneMinWidth, m_leftPaneMaxWidth, &m_lockWindowPos, &m_draggingChildWindowBorder);
                //HandleLeftPaneResize(anyChildHovered || isWindowHovered);

                End();
            }
        }
    }

    void ResourcesPanel::HandleLeftPaneResize(bool canResize)
    {
        const ImVec2 windowPos     = ImGui::GetWindowPos();
        const ImVec2 windowSize    = ImGui::GetWindowSize();
        const ImVec2 borderLineMin = ImVec2(windowPos.x + m_leftPaneWidth, windowPos.y - 1);
        const ImVec2 borderLineMax = ImVec2(borderLineMin.x, borderLineMin.y + windowSize.y);
        ImGui::GetWindowDrawList()->AddLine(borderLineMin, borderLineMax, ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f)), 1.5f);

        const ImVec2   hoverRectMin     = ImVec2(borderLineMin.x - 10, borderLineMin.y);
        const ImVec2   hoverRectMax     = ImVec2(borderLineMin.x + 10, borderLineMax.y);
        static float   pressedPos       = 0.0f;
        static Vector2 windowPosOnPress = Vector2::Zero;

        if (canResize)
        {
            bool canDrag = false;

            if (ImGui::IsMouseHoveringRect(hoverRectMin, hoverRectMax))
            {
                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
                canDrag         = true;
                m_lockWindowPos = true;
            }
            else
                m_lockWindowPos = false;

            if (canDrag)
            {

                if (Input::InputEngineBackend::Get()->GetMouseButtonDown(LINA_MOUSE_1))
                {
                    pressedPos                  = m_leftPaneWidth;
                    m_draggingChildWindowBorder = true;
                }
            }
        }
        else
            m_lockWindowPos = false;

        if (m_draggingChildWindowBorder && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
        {

            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
            const float delta  = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left).x;
            float       newPos = pressedPos + delta;

            if (newPos < m_leftPaneMinWidth)
                newPos = m_leftPaneMinWidth;
            else if (newPos > m_leftPaneMaxWidth)
                newPos = m_leftPaneMaxWidth;

            m_leftPaneWidth = newPos;
        }

        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
            m_draggingChildWindowBorder = false;
    }

    void ResourcesPanel::DrawLeftPane()
    {
        const float topBarSize    = 30;
        const float spaceFromLeft = 10;
        const float childRounding = 3.0f;

        // Search bar & other utilities.
        ImGui::BeginChild("resources_leftPane_topBar", ImVec2(0, topBarSize));

        // Settings Icon
        WidgetsUtility::IncrementCursorPosY(8);
        WidgetsUtility::IncrementCursorPosX(spaceFromLeft);
        if (WidgetsUtility::IconButton(ICON_FA_COG))
        {
            ImGui::OpenPopup("ResourcesLeftPaneSettings");
        }

        WidgetsUtility::PushPopupStyle();
        if (ImGui::BeginPopup("ResourcesLeftPaneSettings"))
        {
            m_leftPaneMenu->Draw();
            ImGui::EndPopup();
        }
        WidgetsUtility::PopPopupStyle();

        ImGui::SameLine();

        // Search bar.
        InputTextCallback_UserData cb_user_data;
        cb_user_data.Str        = &m_folderSearchFilter;
        const float filterWidth = 200.0f;
        WidgetsUtility::IncrementCursorPosY(-3);
        ImGui::PushItemWidth(-spaceFromLeft);
        ImGui::InputTextWithHint("##resources_folderFilter", "search...", (char*)m_folderSearchFilter.c_str(), m_folderSearchFilter.capacity() + 1, ImGuiInputTextFlags_CallbackResize, InputTextCallback, &cb_user_data);
        ImGui::EndChild();

        // Resource folders.
        WidgetsUtility::IncrementCursorPosX(spaceFromLeft);
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyleColorVec4(ImGuiCol_PopupBg));
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, childRounding);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0f);
        ImGui::BeginChild("resources_leftPane_bottom", ImVec2(-spaceFromLeft, -20), true);
        DrawContextMenu();

        const ImVec2 childPos  = ImGui::GetWindowPos();
        const ImVec2 childSize = ImGui::GetWindowSize();

        WidgetsUtility::IncrementCursorPosY(4);
        DrawFolderHierarchy(m_rootFolder, true);

        if (m_selectedFolder != nullptr && m_leftPaneFocused && Input::InputEngineBackend::Get()->GetKeyDown(LINA_KEY_DELETE))
        {
            if (WidgetsUtility::IsProtectedDirectory(m_selectedFolder))
                Snackbar::PushSnackbar(LogLevel::Warn, "The Root, Engine, Editor and Sandbox folders can not be deleted!");
            else
            {
                Utility::DeleteFolder(m_selectedFolder);
                DeselectNodes(true);
            }
        }

        // Deselect folder
        if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered() && Input::InputEngineBackend::Get()->GetMouseButtonDown(LINA_MOUSE_1))
            DeselectNodes(true);

        ImGui::EndChild();
        ImGui::PopStyleVar();
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
    }

    void ResourcesPanel::DrawRightPane()
    {
        const float                topBarSize    = 40;
        const float                spaceFromLeft = 10;
        const float                filterWidth   = 180.0f;
        InputTextCallback_UserData cb_user_data;
        cb_user_data.Str = &m_fileSearchFilter;

        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyleColorVec4(ImGuiCol_TableHeaderBg));
        ImGui::BeginChild("resources_rightPane_topBar", ImVec2(0, topBarSize), true);
        ImGui::SetCursorPosY(topBarSize / 2.0f - ImGui::GetFrameHeight() / 2.0f);
        ImGui::SetCursorPosX(8);

        // Selected folder hierarchy.
        const float cursorYBeforeIcon = ImGui::GetCursorPosY();
        WidgetsUtility::IncrementCursorPosY(2.9f);
        WidgetsUtility::IconSmall(ICON_FA_FOLDER_OPEN);
        ImGui::SameLine();
        ImGui::SetCursorPosY(cursorYBeforeIcon);

        if (m_selectedFolder != nullptr)
        {
            std::vector<Utility::Folder*> hierarchy;
            Utility::GetFolderHierarchToRoot(m_selectedFolder, hierarchy);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
            ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_TableHeaderBg));

            for (int i = (int)hierarchy.size() - 1; i > -1; i--)
            {
                auto* folder = hierarchy[i];

                // Switch the selected folder.
                if (WidgetsUtility::Button(folder->m_name.c_str()))
                {
                    DeselectNodes(false);
                    m_selectedFolder = folder;
                }
                ImGui::SameLine();
                WidgetsUtility::IconSmall(ICON_FA_CARET_RIGHT);
                ImGui::SameLine();
            }

            WidgetsUtility::Button(m_selectedFolder->m_name.c_str());
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();
            ImGui::SameLine();
        }

        // Settings cog.
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - filterWidth - spaceFromLeft - ImGui::GetStyle().ItemSpacing.x * 3);
        if (WidgetsUtility::IconButton(ICON_FA_COG))
        {
            ImGui::OpenPopup("ResourcesRightTopSettings");
        }

        WidgetsUtility::PushPopupStyle();
        if (ImGui::BeginPopup("ResourcesRightTopSettings"))
        {
            WidgetsUtility::PropertyLabel("Item Size");
            WidgetsUtility::IncrementCursorPosX(16);
            ImGui::SetNextItemWidth(100);
            ImGui::SliderFloat("##itemSizes", &m_nodeSizes, 0.5f, 1.5f);

            ImGui::EndPopup();
        }
        WidgetsUtility::PopPopupStyle();
        ImGui::SameLine();

        // Search bar.
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - filterWidth - spaceFromLeft);
        ImGui::PushItemWidth(-spaceFromLeft);
        ImGui::InputTextWithHint("##resources_fileFilter", "search...", (char*)m_fileSearchFilter.c_str(), m_fileSearchFilter.capacity() + 1, ImGuiInputTextFlags_CallbackResize, InputTextCallback, &cb_user_data);

        ImGui::EndChild();
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
        WidgetsUtility::HorizontalDivider(-4.0f, 1.5f);
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();

        ImGui::BeginChild("resources_rightPane_bottom");
        DrawContextMenu();

        // Draw the selected folders contents.
        if (m_selectedFolder != nullptr)
        {
            ImGui::SetCursorPosX(16);
            WidgetsUtility::IncrementCursorPosY(8);
            DrawContents(m_selectedFolder);
        }

        ImGui::EndChild();
    }

    void ResourcesPanel::DrawFolderHierarchy(Utility::Folder* folder, bool performFilterCheck)
    {
        bool dontDraw               = false;
        bool shouldSubsPerformCheck = performFilterCheck;

        if (!m_showEngineFolders && folder->m_fullPath.compare("Resources/Engine") == 0)
            return;

        if (!m_showEditorFolders && folder->m_fullPath.compare("Resources/Editor") == 0)
            return;

        // Here
        if (performFilterCheck)
        {
            const bool containSearchFilter = Utility::FolderContainsFilter(folder, m_folderSearchFilter);

            if (containSearchFilter)
            {
                dontDraw               = false;
                shouldSubsPerformCheck = false;
            }
            else
            {
                const bool subfoldersContain = Utility::SubfoldersContainFilter(folder, m_folderSearchFilter);

                if (subfoldersContain)
                {
                    dontDraw               = false;
                    shouldSubsPerformCheck = true;
                }
                else
                {
                    dontDraw = true;
                }
            }
        }

        ImVec4 childBG = ImGui::GetStyleColorVec4(ImGuiCol_ChildBg);

        if (!dontDraw)
        {
            ImGui::SetNextItemOpen(folder->m_isOpen);

            folder->m_isOpen = WidgetsUtility::DrawTreeFolder(folder, m_selectedFolder, m_leftPaneFocused);

            if (ImGui::IsItemClicked())
            {

                m_selectedFolder = folder;
            }
        }

        if (folder->m_isOpen && folder->m_folders.size() > 0)
        {
            for (auto& f : folder->m_folders)
                DrawFolderHierarchy(f, shouldSubsPerformCheck);

            if (!dontDraw)
                ImGui::TreePop();
        }

        return;
    }

    void ResourcesPanel::DrawContents(Utility::Folder* folder)
    {
        // Deselect the right panel upon empty click.
        if (!ImGui::IsAnyItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            DeselectNodes(false);

        // First draw the folders.
        for (auto* subfolder : folder->m_folders)
        {
            bool renamedItem = false;
            WidgetsUtility::DrawResourceNode(subfolder, subfolder == m_selectedSubfolder, &renamedItem, m_nodeSizes, m_rightPaneFocused);

            if (ImGui::IsItemClicked())
            {
                m_selectedSubfolder = subfolder;
            }

            // Double clicking or pressing enter on a subfolder will select that sub folder from the left-pane, unless the subfolder is being renamed.
            if (ImGui::IsItemHovered())
            {
                if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) || Input::InputEngineBackend::Get()->GetKeyDown(LINA_KEY_RETURN))
                {
                    if (!renamedItem)
                    {
                        DeselectNodes(false);
                        m_selectedFolder = subfolder;

                        // Open the fold-out of the parent folder if closed.
                        if (!m_selectedFolder->m_parent->m_isOpen)
                            m_selectedFolder->m_parent->m_isOpen = true;
                    }
                }
            }
        }
        // Then draw the files.
        for (auto* file : folder->m_files)
        {

            TypeID tid = file->m_typeID;
            if (!m_storage->IsTypeRegistered(tid))
                continue;
            bool renamedItem = false;

            WidgetsUtility::DrawResourceNode(file, file == m_selectedFile, &renamedItem, m_nodeSizes, m_rightPaneFocused);

            if (ImGui::IsItemClicked())
            {
                if (m_selectedFile != nullptr && m_selectedFile != file)
                    m_selectedFile->m_isRenaming = false;

                m_selectedFile = file;
            }

            if (ImGui::IsItemHovered())
            {
                if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) || Input::InputEngineBackend::Get()->GetKeyDown(LINA_KEY_RETURN))
                {
                    if (file->m_typeID == GetTypeID<Graphics::Texture>())
                    {
                        auto& texturePanel = GUILayer::Get()->GetTexturePanel();
                        texturePanel.SetTargetTexture(m_storage->GetResource<Graphics::Texture>(file->m_sid));

                        texturePanel.Open();
                        if (!texturePanel.IsMaximized())
                            texturePanel.ToggleMaximize();
                    }
                    else if (file->m_typeID == GetTypeID<Graphics::Model>())
                    {
                        auto& modelPanel = GUILayer::Get()->GetModelPanel();
                        modelPanel.SetTargetModel(m_storage->GetResource<Graphics::Model>(file->m_sid));

                        modelPanel.Open();
                        if (!modelPanel.IsMaximized())
                            modelPanel.ToggleMaximize();
                    }

                }
            }
        }

        if (m_rightPaneFocused && Input::InputEngineBackend::Get()->GetKeyDown(LINA_KEY_DELETE))
        {
            if (m_selectedSubfolder != nullptr)
            {
                if (WidgetsUtility::IsProtectedDirectory(m_selectedSubfolder))
                    Snackbar::PushSnackbar(LogLevel::Warn, "The Root, Engine, Editor and Sandbox folders can not be deleted!");
                else
                    Utility::DeleteFolder(m_selectedSubfolder);
            }
            else if (m_selectedFile != nullptr)
            {
                if (WidgetsUtility::IsProtectedDirectory(m_selectedFile))
                    Snackbar::PushSnackbar(LogLevel::Warn, "The Root, Engine, Editor and Sandbox folders can not be deleted!");
                else
                    Utility::DeleteResourceFile(m_selectedFile);
            }
        }
    }

    void ResourcesPanel::DrawFile(Utility::File& file)
    {
    }

    void ResourcesPanel::OnMenuBarElementClicked(const EMenuBarElementClicked& ev)
    {
        const MenuBarElementType type = static_cast<MenuBarElementType>(ev.m_item);

        if (type == MenuBarElementType::Resources_ShowEngineFolders)
        {
            m_showEngineFolders              = !m_showEngineFolders;
            m_showEngineFoldersMB->m_tooltip = m_showEngineFolders ? ICON_FA_CHECK : "";

            // If we are disabling viewing the engine folders and the current
            // selected folder is the engine folder, or a subfolder underneath,
            // deselect it and the file.
            if (!m_showEngineFolders && m_selectedFolder != nullptr)
            {
                Utility::Folder* f                     = m_selectedFolder;
                bool             currentIsEngineFolder = m_selectedFolder->m_fullPath.compare("Resources/Engine") == 0;

                if (!currentIsEngineFolder)
                {
                    while (f->m_parent != nullptr)
                    {
                        if (f->m_parent->m_fullPath.compare("Resources/Engine") == 0)
                        {
                            currentIsEngineFolder = true;
                            break;
                        }
                        f = f->m_parent;
                    }
                }

                if (currentIsEngineFolder)
                    DeselectNodes(true);
            }
        }
        else if (type == MenuBarElementType::Resources_ShowEditorFolders)
        {
            m_showEditorFolders              = !m_showEditorFolders;
            m_showEditorFoldersMB->m_tooltip = m_showEditorFolders ? ICON_FA_CHECK : "";

            // If we are disabling viewing the editor  folders and the current
            // selected folder is the editor folder, or a subfolder underneath,
            // deselect it and the file.
            if (!m_showEditorFolders && m_selectedFolder != nullptr)
            {
                Utility::Folder* f                     = m_selectedFolder;
                bool             currentIsEditorFolder = m_selectedFolder->m_fullPath.compare("Resources/Editor") == 0;

                if (!currentIsEditorFolder)
                {
                    while (f->m_parent != nullptr)
                    {
                        if (f->m_parent->m_fullPath.compare("Resources/Editor") == 0)
                        {
                            currentIsEditorFolder = true;
                            break;
                        }
                        f = f->m_parent;
                    }
                }

                if (currentIsEditorFolder)
                    DeselectNodes(true);
            }
        }
        else if (type == MenuBarElementType::Resources_CreateNewFolder)
        {
            if (ContextMenuCanAddAsset())
            {
                // Get Resources/Sandbox if no selected folder is available.
                Utility::Folder* parent = m_selectedFolder == nullptr ? m_rootFolder->m_folders[2] : m_selectedFolder;
                Utility::CreateNewSubfolder(parent);
            }
        }
        else if (type == MenuBarElementType::Resources_CreateNewMaterial)
        {
        }
        else if (type == MenuBarElementType::Resources_CreateNewPhysicsMaterial)
        {
        }
        else if (type == MenuBarElementType::Resources_Rescan)
        {
            if (m_selectedFolder != nullptr)
                Utility::ScanFolder(m_selectedFolder, true, nullptr, true);
        }
    }

    void ResourcesPanel::DeselectNodes(bool deselectAll)
    {
        if (m_selectedFile != nullptr)
            m_selectedFile->m_isRenaming = false;

        if (m_selectedSubfolder != nullptr)
            m_selectedSubfolder->m_isRenaming = false;

        m_selectedFile      = nullptr;
        m_selectedSubfolder = nullptr;

        if (m_selectedFolder != nullptr)
            m_selectedFolder->m_isRenaming = false;

        if (deselectAll)
            m_selectedFolder = nullptr;
    }

    void ResourcesPanel::DrawContextMenu()
    {
        WidgetsUtility::PushPopupStyle();

        // Handle Right Click.
        if (ImGui::BeginPopupContextWindow())
        {
            m_contextMenu->Draw();
            ImGui::EndPopup();
        }
        WidgetsUtility::PopPopupStyle();
    }

    bool ResourcesPanel::ContextMenuCanAddAsset()
    {
        if (m_selectedFolder != nullptr && m_selectedFolder->m_fullPath.find("Resources/Sandbox") == std::string::npos)
        {
            Snackbar::PushSnackbar(LogLevel::Warn, "New assets and folders can only be added under the Sandbox directory!");
            return false;
        }

        return true;
    }

} // namespace Lina::Editor
