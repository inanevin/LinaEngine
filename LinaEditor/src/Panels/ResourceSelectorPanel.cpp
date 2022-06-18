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

#include "Panels/ResourceSelectorPanel.hpp"
#include "Core/Window.hpp"
#include "Core/ImGuiCommon.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "Core/ResourceManager.hpp"
#include "Resources/ResourceStorage.hpp"
#include "Core/GUILayer.hpp"
#include "Core/EditorApplication.hpp"
#include "Rendering/Texture.hpp"

namespace Lina::Editor
{

    constexpr float paddingX = 10;

    void ResourceSelectorPanel::Initialize(const char* id, const char* icon)
    {
        EditorPanel::Initialize(id, icon);
        m_window = Graphics::Window::Get();
        m_windowFlags |= ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize;
        m_root        = Resources::ResourceManager::Get()->GetRootFolder();
        m_storage     = Resources::ResourceStorage::Get();
        m_show        = false;
        m_firstLaunch = true;
    }

    void ResourceSelectorPanel::Draw()
    {

        if (m_show)
        {
            if (m_firstLaunch)
            {
                m_firstLaunch            = false;
                const ImVec2  size       = ImVec2(400 * GUILayer::Get()->GetDPIScale(), 600 * GUILayer::Get()->GetDPIScale());
                const Vector2 windowPos  = m_window->GetPos();
                const Vector2 windowSize = m_window->GetWorkSize();
                const ImVec2  pos        = ImVec2(windowPos.x - size.x / 2.0f + windowSize.x / 2.0f, windowPos.y - size.y / 2.0f + windowSize.y / 2.0f);
                ImGui::SetNextWindowSize(size);
                ImGui::SetNextWindowPos(pos);
            }

            if (Begin())
            {
                ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());
                WidgetsUtility::IncrementCursorPosY(2);
                ImGui::SetCursorPosX(paddingX);
                DrawTop();

                // Divide
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
                WidgetsUtility::HorizontalDivider(0.0f, 1.5f);
                ImGui::PopStyleColor();

                ImGui::SetCursorPosX(paddingX);
                WidgetsUtility::IncrementCursorPosY(8);
                DrawBottom();

                End();
            }
        }
    }

    void ResourceSelectorPanel::DrawTop()
    {
        const std::string selectText = "Select Resource: " + m_resourceStr;
        WidgetsUtility::PropertyLabel(selectText.c_str(), false);

        WidgetsUtility::PropertyLabel("Filter");

        // Search bar.
        InputTextCallback_UserData cb_user_data;
        cb_user_data.Str        = &m_searchFilter;
        const float filterWidth = 200.0f * GUILayer::Get()->GetDPIScale();

        ImGui::PushItemWidth(-paddingX);
        ImGui::InputTextWithHint("##resourceSelector_folderFilter", "search...", (char*)m_searchFilter.c_str(), m_searchFilter.capacity() + 1, ImGuiInputTextFlags_CallbackResize, InputTextCallback, &cb_user_data);
    }

    void ResourceSelectorPanel::DrawBottom()
    {

        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyleColorVec4(ImGuiCol_PopupBg));
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0f);
        ImGui::BeginChild("resourceSelector_bottom", ImVec2(-paddingX, -60 * GUILayer::Get()->GetDPIScale()), true);

        DrawFilesInFolder(m_root);

        ImGui::EndChild();
        ImGui::PopStyleVar();
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();

        if (m_selectedFile != nullptr)
        {
            ImGui::SetCursorPosX(paddingX);
            ImGui::Text("Full Path:");
            ImGui::SameLine();
            ImGui::PushTextWrapPos();
            ImGui::Text(m_selectedFile->m_fullPath.c_str());
            ImGui::PopTextWrapPos();
        }
    }

    void ResourceSelectorPanel::DrawFilesInFolder(Utility::Folder* folder)
    {
        for (auto* sub : folder->m_folders)
            DrawFilesInFolder(sub);

        for (auto* file : folder->m_files)
        {
            if (m_storage->GetTypeIDFromExtension(file->m_extension) != m_currentFileType)
                continue;

            const std::string fileNameLower  = Utility::ToLower(file->m_name);
            const std::string searchStrLower = Utility::ToLower(m_searchFilter);

            if (m_searchFilter.compare("") != 0 && fileNameLower.find(searchStrLower) != 0)
                continue;

            ImGui::SetCursorPosX(5);
            ImGui::Selectable(file->m_name.c_str(), m_selectedFile == file);

            if (ImGui::IsItemHovered())
            {
#pragma warning(disable : 4312)

                if (file->m_typeID == GetTypeID<Graphics::Model>() || file->m_typeID == GetTypeID<Graphics::Material>())
                {
                    uint32       textureID   = EditorApplication::Get()->GetSnapshotTexture(file->m_sid);
                    uint32       bgTextureID = Resources::ResourceStorage::Get()->GetResource<Graphics::Texture>("Resources/Editor/Textures/Checkered.png")->GetID();
                    const ImVec2 imageMin    = ImVec2(ImGui::GetMousePos().x, ImGui::GetMousePos().y);
                    const ImVec2 imageMax    = ImVec2(imageMin.x + 128 * GUILayer::Get()->GetDPIScale(), imageMin.y + 128 * GUILayer::Get()->GetDPIScale());
                    ImGui::GetForegroundDrawList()->AddImage((void*)bgTextureID, imageMin, imageMax, ImVec2(0, 1), ImVec2(1, 0));
                    ImGui::GetForegroundDrawList()->AddImage((void*)textureID, imageMin, imageMax, ImVec2(0, 1), ImVec2(1, 0));
                    ImGui::GetForegroundDrawList()->AddRect(imageMin, imageMax, ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), 0.0f, ImDrawFlags_None, 1.5f);
                }
                else if (file->m_typeID == GetTypeID<Graphics::Texture>())
                {
                    uint32       textureID   = Resources::ResourceStorage::Get()->GetResource<Graphics::Texture>(file->m_sid)->GetID();
                    uint32       bgTextureID = Resources::ResourceStorage::Get()->GetResource<Graphics::Texture>("Resources/Editor/Textures/Checkered.png")->GetID();
                    const ImVec2 imageMin    = ImVec2(ImGui::GetMousePos().x, ImGui::GetMousePos().y);
                    const ImVec2 imageMax    = ImVec2(imageMin.x + 128 * GUILayer::Get()->GetDPIScale(), imageMin.y + 128 * GUILayer::Get()->GetDPIScale());
                    ImGui::GetForegroundDrawList()->AddImage((void*)bgTextureID, imageMin, imageMax, ImVec2(0, 1), ImVec2(1, 0));
                    ImGui::GetForegroundDrawList()->AddImage((void*)textureID, imageMin, imageMax, ImVec2(0, 1), ImVec2(1, 0));
                    ImGui::GetForegroundDrawList()->AddRect(imageMin, imageMax, ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), 0.0f, ImDrawFlags_None, 1.5f);
                }
            }

            if (ImGui::IsItemClicked())
            {
                m_selectedFile = file;
            }

            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
                m_selectedFile     = file;
                m_selectedResource = StringID(m_selectedFile->m_fullPath.c_str()).value();
                Close();
            }
        }
    }

} // namespace Lina::Editor