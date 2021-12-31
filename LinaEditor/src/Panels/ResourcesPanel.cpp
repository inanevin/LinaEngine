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
#include "imgui/imgui.h"

#include <filesystem>

namespace Lina::Editor
{
    Menu*               m_leftPaneMenu           = nullptr;
    MenuBarElement*     m_showEditorFoldersMB    = nullptr;
    MenuBarElement*     m_showEngineFoldersMB    = nullptr;
    float               m_fileNodeStartCursorPos = 16.0f;
    std::vector<TypeID> m_resourceTypesToDraw;

    struct InputTextCallback_UserData
    {
        std::string*           Str;
        ImGuiInputTextCallback ChainCallback;
        void*                  ChainCallbackUserData;
    };

    ResourcesPanel::~ResourcesPanel()
    {
        delete m_leftPaneMenu;
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
        Event::EventSystem::Get()->Connect<Event::EKeyCallback, &ResourcesPanel::OnKeyCallback>(this);

        m_rootFolder = Resources::ResourceManager::Get()->GetRootFolder();
    }

    static int InputTextCallback(ImGuiInputTextCallbackData* data)
    {
        InputTextCallback_UserData* user_data = (InputTextCallback_UserData*)data->UserData;
        if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
        {
            // Resize string callback
            // If for some reason we refuse the new length (BufTextLen) and/or capacity (BufSize) we need to set them back to what we want.
            std::string* str = user_data->Str;
            IM_ASSERT(data->Buf == str->c_str());
            str->resize(data->BufTextLen);
            data->Buf = (char*)str->c_str();
        }
        else if (user_data->ChainCallback)
        {
            // Forward to user callback, if any
            data->UserData = user_data->ChainCallbackUserData;
            return user_data->ChainCallback(data);
        }
        return 0;
    }

    void ResourcesPanel::Draw()
    {
        if (m_show)
        {
            Begin();

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
            m_leftPaneFocused = ImGui::IsWindowFocused();

            if (ImGui::IsWindowHovered())
                anyChildHovered = true;

            ImGui::EndChild();

            ImGui::SameLine();
            WidgetsUtility::IncrementCursorPosX(-ImGui::GetStyle().ItemSpacing.x + 2);

            ImGui::BeginChild("resources_rightPane");
            DrawRightPane();
            m_rightPaneFocused = ImGui::IsWindowFocused();

            if (ImGui::IsWindowHovered())
                anyChildHovered = true;

            ImGui::EndChild();

            HandleLeftPaneResize(anyChildHovered || isWindowHovered);

            End();
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

        const ImVec2 childPos  = ImGui::GetWindowPos();
        const ImVec2 childSize = ImGui::GetWindowSize();

        WidgetsUtility::IncrementCursorPosY(4);
        DrawFolderMenu(m_rootFolder, true);

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

        // Draw the selected folders contents.
        if (m_selectedFolder != nullptr)
        {
            ImGui::SetCursorPosX(m_fileNodeStartCursorPos);
            WidgetsUtility::IncrementCursorPosY(8);
            DrawContents(m_selectedFolder);
        }

        ImGui::EndChild();
    }

    void ResourcesPanel::DrawFolderMenu(Utility::Folder* folder, bool performFilterCheck)
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
            folder->m_isOpen = WidgetsUtility::DrawTreeFolder(folder, m_selectedFolder);

            if (ImGui::IsItemClicked())
                m_selectedFolder = folder;
        }

        if (folder->m_isOpen && folder->m_folders.size() > 0)
        {
            for (auto& f : folder->m_folders)
                DrawFolderMenu(f, shouldSubsPerformCheck);

            if (!dontDraw)
                ImGui::TreePop();
        }

        return;

        // static ImGuiTreeNodeFlags folderFlagsNotSelected = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
        // static ImGuiTreeNodeFlags folderFlagsSelected = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Selected;
        // static ImGuiTreeNodeFlags fileNodeFlagsNotSelected = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth;
        // static ImGuiTreeNodeFlags fileNodeFlagsSelected = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Selected;
        //
        // ImGuiTreeNodeFlags folderFlags = m_selectedFolder == &folder ? folderFlagsSelected : folderFlagsNotSelected;
        //
        // std::string id = "##" + folder.m_name;
        // bool nodeOpen = ImGui::TreeNodeEx(id.c_str(), folderFlags);
        // ImGui::SameLine();  WidgetsUtility::IncrementCursorPosY(5);
        // WidgetsUtility::Icon(ICON_FA_FOLDER, 0.7f, ImVec4(0.8f, 0.8f, 0.8f, 1.0f));
        // ImGui::SameLine(); WidgetsUtility::IncrementCursorPosX(3); WidgetsUtility::IncrementCursorPosY(-5);
        // ImGui::Text(folder.m_name.c_str());
        //
        //// Click
        // if (ImGui::IsItemClicked())
        //{
        //	m_selectedFile = nullptr;
        //	m_selectedFolder = &folder;
        //}
        //
        //
        // if (nodeOpen)
        //{
        //	//for (auto& f : folder.m_folders)
        //	//{
        //	//	DrawFolderMenu(f);
        //	//
        //	//}
        //	ImGui::TreePop();
        //
        //}
    }

    void ResourcesPanel::DrawContents(Utility::Folder* folder)
    {
        // Deselect the right panel upon empty click.
        if (!ImGui::IsAnyItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            DeselectNodes(false);

        // First draw the folders.
        for (auto* subfolder : folder->m_folders)
        {
            WidgetsUtility::DrawResourceNode(-1, subfolder == m_selectedSubfolder, subfolder->m_fullPath, m_fileNodeStartCursorPos);

            if (ImGui::IsItemClicked())
            {
                m_selectedSubfolder = subfolder;
            }

            if (ImGui::IsItemHovered())
            {
                if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) || Input::InputEngineBackend::Get()->GetKeyDown(LINA_KEY_RETURN))
                {
                    DeselectNodes(false);
                    m_selectedFolder = subfolder;
                }
            }
        }
        // Then draw the files.
        for (auto* file : folder->m_files)
        {

            TypeID tid = m_storage->GetTypeIDFromExtension(file->m_extension);
            if (!m_storage->IsTypeRegistered(tid))
                continue;

            WidgetsUtility::DrawResourceNode(tid, file == m_selectedFile, file->m_fullPath, m_fileNodeStartCursorPos);

            if (ImGui::IsItemClicked())
                m_selectedFile = file;
        }
    }

    void ResourcesPanel::DrawFile(Utility::File& file)
    {
    }

    void ResourcesPanel::OnMenuBarElementClicked(const EMenuBarElementClicked& ev)
    {
        if (ev.m_item == static_cast<uint8>(MenuBarElementType::Resources_ShowEngineFolders))
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
        else if (ev.m_item == static_cast<uint8>(MenuBarElementType::Resources_ShowEditorFolders))
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
    }

    void ResourcesPanel::OnKeyCallback(const Event::EKeyCallback& ev)
    {
        if (m_leftPaneFocused && m_selectedFolder != nullptr && ev.m_key == LINA_KEY_F2)
        {
            LINA_TRACE("Left rename");
        }
        if (m_rightPaneFocused && ev.m_key == LINA_KEY_F2)
        {
            if (m_selectedFile != nullptr)
            {
                LINA_TRACE("Right file rename");
            }
            else if (m_selectedSubfolder != nullptr)
            {
                LINA_TRACE("Right sf rename");
            }
        }
    }

    void ResourcesPanel::DeselectNodes(bool deselectAll)
    {
        m_selectedFile      = nullptr;
        m_selectedSubfolder = nullptr;

        if (deselectAll)
            m_selectedFolder = nullptr;
    }

    /* void ResourcesPanel::DrawContextMenu()
    {
        std::string rootPath = s_hoveredFolder == nullptr ? "Resources" : s_hoveredFolder->m_path;

        // Handle Right click popup.
        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::BeginMenu("Create"))
            {
                // Build a folder.
                if (ImGui::MenuItem("Folder"))
                {
                    std::string folderPath = rootPath + "/NewFolder" + std::to_string(++s_itemIDCounter);
                    EditorUtility::CreateFolderInPath(folderPath);
                    EditorFolder folder;
                    folder.m_path = folderPath;
                    folder.m_name = "NewFolder" + std::to_string(s_itemIDCounter);
                    folder.m_id = s_itemIDCounter;

                    if (s_hoveredFolder != nullptr)
                        s_hoveredFolder->m_subFolders[folder.m_id] = folder;
                    else
                        m_resourceFolders[0].m_subFolders[folder.m_id] = folder;

                }

                ImGui::Separator();

                // Build a material.
                if (ImGui::MenuItem("Material"))
                {
                    std::string name = "NewMaterial" + std::to_string(++s_itemIDCounter) + ".mat";
                    std::string materialPath = rootPath + "/" + name;

                    EditorFile file;
                    file.m_path = materialPath;
                    file.m_pathToFolder = rootPath + "/";
                    file.m_name = name;
                    file.m_extension = "mat";
                    file.m_type = FileType::Material;
                    file.m_id = ++s_itemIDCounter;

                    Graphics::Material& m = Graphics::Material::CreateMaterial(Graphics::RenderEngineBackend::Get()->GetDefaultShader(), file.m_path);
                    Graphics::Material::SaveMaterialData(m, materialPath);

                    if (s_hoveredFolder != nullptr)
                        s_hoveredFolder->m_files[file.m_id] = file;
                    else
                        m_resourceFolders[0].m_files[file.m_id] = file;
                }

                ImGui::EndMenu();
            }

            ImGui::EndPopup();
        }
    }

    void ResourcesPanel::DrawFolder(EditorFolder& folder, bool isRoot)
    {
        static ImGuiTreeNodeFlags folderFlagsNotSelected = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
        static ImGuiTreeNodeFlags folderFlagsSelected = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Selected;
        static ImGuiTreeNodeFlags fileNodeFlagsNotSelected = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth;
        static ImGuiTreeNodeFlags fileNodeFlagsSelected = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Selected;


        WidgetsUtility::IncrementCursorPosY(11);
        WidgetsUtility::ItemSpacingX(0);

        if (!isRoot)
            WidgetsUtility::IncrementCursorPosY(-11);


        // Draw folders.
        for (std::map<int, EditorFolder>::iterator it = folder.m_subFolders.begin(); it != folder.m_subFolders.end();)
        {
            // Skip drawing if internal folders.
            if (!m_drawInternals)
            {
                if (it->second.m_parent != nullptr && it->second.m_parent->m_name.compare(ROOT_NAME) == 0)
                {
                    if (it->second.m_name.compare("engine") == 0 || it->second.m_name.compare("editor") == 0)
                    {
                        it++;
                        continue;
                    }
                }
            }



            WidgetsUtility::IncrementCursorPosX(4);

            if (it->second.m_markedForErase)
            {
                // Delete directory.
                EditorUtility::DeleteDirectory(it->second.m_path);

                // Nullout reference.
                if (s_hoveredFolder == &it->second)
                    s_hoveredFolder = nullptr;

                // Unload the contained resources & erase.
                UnloadFileResourcesInFolder(it->second);
                folder.m_subFolders.erase(it++);
                continue;
            }

            ImGuiTreeNodeFlags folderFlags = (it->second).m_id == s_selectedItem ? folderFlagsSelected : folderFlagsNotSelected;

            if (it->second.m_markedForForceOpen)
            {
                ImGui::SetNextItemOpen(true);
                it->second.m_markedForForceOpen = false;
            }

            std::string id = "##" + (it->second).m_name;
            bool nodeOpen = ImGui::TreeNodeEx(id.c_str(), folderFlags);
            ImGui::SameLine();  WidgetsUtility::IncrementCursorPosY(5);
            // col = , ImVec4(0.9f, 0.83f, 0.0f, 1.0f)
            WidgetsUtility::Icon(ICON_FA_FOLDER, 0.7f);
            ImGui::SameLine(); WidgetsUtility::IncrementCursorPosX(3); WidgetsUtility::IncrementCursorPosY(-5);
            ImGui::Text((it->second).m_name.c_str());

            // Click
            if (ImGui::IsItemClicked())
            {
                s_selectedItem = (it->second).m_id;
                s_selectedFile = nullptr;
                s_selectedFolder = &(it->second);
            }

            // Hover.
            if (ImGui::IsWindowHovered() && ImGui::IsItemHovered())
                s_hoveredFolder = &(it->second);

            if (nodeOpen)
            {
                DrawFolder(it->second);
                ImGui::TreePop();
            }
            ++it;

        }
        WidgetsUtility::PopStyleVar();


        // Draw files.
        for (std::map<int, EditorFile>::iterator it = folder.m_files.begin(); it != folder.m_files.end();)
        {
            WidgetsUtility::IncrementCursorPosX(-9);
            if (it->second.m_markedForErase)
            {
                // Delete directory.
                EditorUtility::DeleteDirectory(it->second.m_path);

                // Unload the resources & erase.
                UnloadFileResource(it->second);
                folder.m_files.erase(it++);
                continue;
            }

            ImGuiTreeNodeFlags fileFlags = it->second.m_id == s_selectedItem ? fileNodeFlagsSelected : fileNodeFlagsNotSelected;

            // Highlight.
            if (it->second.m_markedForHighlight)
            {
                it->second.m_markedForHighlight = false;
                s_usedFileNameColor = s_highlightColor;
                s_highlightColorSet = true;
                s_colorLerpItemID = it->second.m_id;
                s_colorLerpTimestamp = ImGui::GetTime();
            }

            if (s_highlightColorSet)
            {
                float t = ImGui::GetTime() - s_colorLerpTimestamp;
                float remapped = Math::Remap(t, 0.0f, s_colorLerpDuration, 0.0f, 1.0f);

                s_usedFileNameColor.x = Math::Lerp(s_highlightColor.x, s_fileNameColor.x, remapped);
                s_usedFileNameColor.y = Math::Lerp(s_highlightColor.y, s_fileNameColor.y, remapped);
                s_usedFileNameColor.z = Math::Lerp(s_highlightColor.z, s_fileNameColor.z, remapped);
                s_usedFileNameColor.w = Math::Lerp(s_highlightColor.w, s_fileNameColor.w, remapped);

                if (t > s_colorLerpDuration - 0.1f)
                {
                    s_highlightColorSet = false;
                    s_usedFileNameColor = s_fileNameColor;
                }
            }

            if (it->second.m_id == s_colorLerpItemID)
                ImGui::PushStyleColor(ImGuiCol_Text, s_usedFileNameColor);
            else
                ImGui::PushStyleColor(ImGuiCol_Text, s_fileNameColor);

            // Node
            bool nodeOpen = ImGui::TreeNodeEx(it->second.m_name.c_str(), fileFlags);

            // Drag drop
            if (it->second.m_type == FileType::Texture2D)
            {
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
                {
                    // Set payload to carry the texture;
                    StringIDType id = Graphics::Texture::GetTexture(it->second.m_path).GetSID();
                    ImGui::SetDragDropPayload(RESOURCES_MOVETEXTURE_ID, &id, sizeof(StringIDType));

                    // Display preview
                    ImGui::Text("Assign ");
                    ImGui::EndDragDropSource();
                }
            }
            if (it->second.m_type == FileType::HDRI)
            {
                // TODO: HDRI Support & auto capturing.
            }
            else if (it->second.m_type == FileType::Material)
            {
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
                {
                    // Set payload to carry the texture;
                    StringIDType id = Graphics::Material::GetMaterial(it->second.m_path).GetID();
                    ImGui::SetDragDropPayload(RESOURCES_MOVEMATERIAL_ID, &id, sizeof(StringIDType));

                    // Display preview
                    ImGui::Text("Assign ");
                    ImGui::EndDragDropSource();
                }
            }
            else if (it->second.m_type == FileType::Model)
            {
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
                {
                    // Set payload to carry the texture;
                    StringIDType id = Graphics::Model::GetModel(it->second.m_path).GetID();
                    ImGui::SetDragDropPayload(RESOURCES_MOVEMESH_ID, &id, sizeof(StringIDType));

                    // Display preview
                    ImGui::Text("Assign ");
                    ImGui::EndDragDropSource();
                }
            }
            else if (it->second.m_type == FileType::Shader)
            {
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
                {
                    // Set payload to carry the texture;
                    StringIDType id = Graphics::Shader::GetShader(it->second.m_path).GetSID();
                    ImGui::SetDragDropPayload(RESOURCES_MOVESHADER_ID, &id, sizeof(uint32));

                    // Display preview
                    ImGui::Text("Assign ");
                    ImGui::EndDragDropSource();
                }
            }


            ImGui::PopStyleColor();

            // Click.
            if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
            {
                s_selectedItem = it->second.m_id;
                s_selectedFolder = nullptr;
                s_selectedFile = &it->second;

                // Notify properties panel of file selection.
                if (it->second.m_type == FileType::Texture2D)
                    Event::EventSystem::Get()->Trigger<ETextureSelected>(ETextureSelected{ &Graphics::Texture::GetTexture(it->second.m_path) });
                else if (it->second.m_type == FileType::Model)
                    Event::EventSystem::Get()->Trigger<EModelSelected>(EModelSelected{ &Graphics::Model::GetModel(it->second.m_path) });
                else if (it->second.m_type == FileType::Material)
                    Event::EventSystem::Get()->Trigger<EMaterialSelected>(EMaterialSelected{ &it->second, &Graphics::Material::GetMaterial(it->second.m_path) });
            }

            if (nodeOpen)
                ImGui::TreePop();
            ++it;

        }

        // Deselect.
        if (!ImGui::IsAnyItemHovered() && ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            Event::EventSystem::Get()->Trigger<EEntityUnselected>(EEntityUnselected{});
            s_selectedItem = -1;
        }

        // Delete item.
        if (ImGui::IsKeyPressed(Input::InputCode::Delete) && s_selectedItem != -1 && ImGui::IsWindowFocused())
        {
            if (s_selectedFolder != nullptr)
                s_selectedFolder->m_markedForErase = true;
            if (s_selectedFile != nullptr)
                s_selectedFile->m_markedForErase = true;

            // Deselect
            Event::EventSystem::Get()->Trigger<EEntityUnselected>(EEntityUnselected{});
            s_selectedItem = -1;
        }

        if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered())
            s_hoveredFolder = nullptr;
    }

    void ResourcesPanel::LoadFolderResources(EditorFolder& folder)
    {
        // Load files.
        for (std::map<int, EditorFile>::iterator it = folder.m_files.begin(); it != folder.m_files.end(); ++it)
        {
            EditorFile& file = it->second;

            if (file.m_type == FileType::Texture2D)
            {
                bool textureExists = Graphics::Texture::TextureExists(file.m_path);

                if (!textureExists)
                {
                    //Graphics::SamplerParameters samplerParams;
                    //std::string samplerParamsPath = file.m_pathToFolder + EditorUtility::RemoveExtensionFromFilename(file.m_name) + ".linaimagedata";
                    //
                    //if (Utility::FileExists(samplerParamsPath))
                    //	samplerParams = Graphics::Texture::LoadAssetData(samplerParamsPath);
                    //
                    //Graphics::Texture::CreateTexture2D(file.m_path, samplerParams, false, false, samplerParamsPath);
                    //
                    //Graphics::Texture::SaveAssetData(samplerParamsPath, samplerParams);
                }
            }
            if (file.m_type == FileType::HDRI)
            {
                bool textureExists = Graphics::Texture::TextureExists(file.m_path);

                if (!textureExists)
                    Graphics::Texture::CreateTextureHDRI(file.m_path);
            }
            if (file.m_type == FileType::Material)
            {
                //bool materialExists = Graphics::Material::MaterialExists(file.m_path);
                //if (!materialExists)
                //	Graphics::Material::LoadMaterialFromFile(file.m_path);

            }
            else if (file.m_type == FileType::Model)
            {
                bool meshExists = Graphics::Model::ModelExists(file.m_path);

                if (!meshExists)
                {
                    //Graphics::ModelAssetData meshParams;
                    //std::string meshParamsPath = file.m_pathToFolder + EditorUtility::RemoveExtensionFromFilename(file.m_name) + ".linamodeldata";
                    //
                    //if (Utility::FileExists(meshParamsPath))
                    //	meshParams = Graphics::Model::LoadAssetData(meshParamsPath);
                    //
                    //Graphics::Model::CreateModel(file.m_path, meshParams, -1, meshParamsPath);

                    //Graphics::Model::SaveAssetData(meshParamsPath, meshParams);
                }
            }
        }

        // Recursively load subfolders.
        for (std::map<int, EditorFolder>::iterator it = folder.m_subFolders.begin(); it != folder.m_subFolders.end(); ++it)
            LoadFolderResources(it->second);
    }

    void ResourcesPanel::LoadFolderDependencies(EditorFolder& folder)
    {
        Graphics::RenderEngineBackend* renderEngine = Graphics::RenderEngineBackend::Get();

        // Load files.
        for (std::map<int, EditorFile>::iterator it = folder.m_files.begin(); it != folder.m_files.end(); ++it)
        {
            EditorFile& file = it->second;

            // SKIP FOR NOW BC WE NEED TO MAKE SURE WE HANDLE BOTH ENGINE CREATION & EDITOR CREATION
            if (file.m_type == FileType::Texture2D)
            {

            }
            if (file.m_type == FileType::Material)
            {
                // Graphics::Material& mat = Graphics::Material::GetMaterial(file.m_path);
                // mat.PostLoadMaterialData();
            }
            else if (file.m_type == FileType::Model)
            {

            }
        }

        // Recursively load subfolders.
        for (std::map<int, EditorFolder>::iterator it = folder.m_subFolders.begin(); it != folder.m_subFolders.end(); ++it)
            LoadFolderDependencies(it->second);
    }

    void ResourcesPanel::UnloadFileResource(EditorFile& file)
    {
        if (file.m_type == FileType::Texture2D)
            Graphics::Texture::UnloadTextureResource(Graphics::Texture::GetTexture(file.m_path).GetSID());
        else if(file.m_type == FileType::HDRI)
            Graphics::Texture::UnloadTextureResource(Graphics::Texture::GetTexture(file.m_path).GetSID());
        else if (file.m_type == FileType::Model)
            Graphics::Model::UnloadModel(Graphics::Model::GetModel(file.m_path).GetID());
        else if (file.m_type == FileType::Material)
            Graphics::Material::UnloadMaterialResource(Graphics::Material::GetMaterial(file.m_path).GetID());
    }

    void ResourcesPanel::UnloadFileResourcesInFolder(EditorFolder& folder)
    {
        for (std::map<int, EditorFile>::iterator it = folder.m_files.begin(); it != folder.m_files.end(); ++it)
            UnloadFileResource(it->second);
    }

    bool ResourcesPanel::ExpandFileResource(EditorFolder& folder, const std::string& path, FileType type)
    {
        for (auto& file : folder.m_files)
        {
            if (type == FileType::Unknown || (type != FileType::Unknown && file.second.m_type == type))
            {
                if (file.second.m_path.compare(path) == 0)
                {
                    file.second.m_markedForHighlight = true;
                    folder.m_markedForForceOpen = true;

                    EditorFolder* parent = folder.m_parent;
                    while (parent != nullptr)
                    {
                        parent->m_markedForForceOpen = true;

                        parent = parent->m_parent;
                    }

                    return true;
                }
            }
        }

        for (auto& subfolder : folder.m_subFolders)
        {
            if (ExpandFileResource(subfolder.second, path, type))
                break;
        }
    }

    FileType ResourcesPanel::GetFileType(std::string& extension)
    {
        if (extension.compare("jpg") == 0 || extension.compare("jpeg") == 0 || extension.compare("png") == 0 || extension.compare("tga") == 0)
            return FileType::Texture2D;
        else if (extension.compare("hdr") == 0)
            return FileType::HDRI;
        else if (extension.compare("ttf") == 0)
            return FileType::Font;
        else if (extension.compare("obj") == 0 || extension.compare("fbx") == 0 || extension.compare("3ds") == 0)
            return FileType::Model;
        else if (extension.compare("mat") == 0)
            return FileType::Material;
        else if (extension.compare("glsl") == 0)
            return FileType::Shader;
        else
            return FileType::Unknown;
    }

    void ResourcesPanel::MaterialTextureSelected(EMaterialTextureSelected ev)
    {
        ExpandFileResource(m_resourceFolders[0], ev.m_texture->GetPath(), FileType::Texture2D);
    }

    void ResourcesPanel::TextureReimported(ETextureReimported ev)
    {
        VerifyMaterialFiles(m_resourceFolders[0], ev);
    }

    bool ResourcesPanel::VerifyMaterialFiles(EditorFolder& folder, ETextureReimported ev)
    {
        // Iterate the files in this folder first & see if there is a match.
        for (auto file : folder.m_files)
        {
            if (file.second.m_type == FileType::Material)
            {
                Graphics::Material& mat = Graphics::Material::GetMaterial(file.second.m_path);
                for (auto sampler : mat.m_sampler2Ds)
                {
                    if (sampler.second.m_boundTexture == ev.m_selected)
                    {
                        mat.SetTexture(sampler.first, ev.m_reimported, sampler.second.m_bindMode);
                        return true;
                    }
                }

            }
        }

        // Iterate subfolders.
        for (auto folder : folder.m_subFolders)
        {
            if (VerifyMaterialFiles(folder.second, ev))
                return true;
        }

        return false;
    }
    */
} // namespace Lina::Editor
