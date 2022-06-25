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
        const String selectText = "Select Resource: " + m_resourceStr;
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

            const String fileNameLower  = Utility::ToLower(file->name);
            const String searchStrLower = Utility::ToLower(m_searchFilter);

            if (m_searchFilter.compare("") != 0 && fileNameLower.find(searchStrLower) != 0)
                continue;

            ImGui::SetCursorPosX(5);
            ImGui::Selectable(file->name.c_str(), m_selectedFile == file);

            if (ImGui::IsItemHovered())
            {
#pragma warning(disable : 4312)

                if (file->typeID == GetTypeID<Graphics::Model>() || file->typeID == GetTypeID<Graphics::Material>())
                {
                    uint32       textureID   = EditorApplication::Get()->GetSnapshotTexture(file->sid);
                    uint32       bgTextureID = Resources::ResourceStorage::Get()->GetResource<Graphics::Texture>("Resources/Editor/Textures/Checkered.png")->GetID();
                    const ImVec2 imageMin    = ImVec2(ImGui::GetMousePos().x, ImGui::GetMousePos().y);
                    const ImVec2 imageMax    = ImVec2(imageMin.x + 128 * GUILayer::Get()->GetDPIScale(), imageMin.y + 128 * GUILayer::Get()->GetDPIScale());
                    ImGui::GetForegroundDrawList()->AddImage((void*)bgTextureID, imageMin, imageMax, ImVec2(0, 1), ImVec2(1, 0));
                    ImGui::GetForegroundDrawList()->AddImage((void*)textureID, imageMin, imageMax, ImVec2(0, 1), ImVec2(1, 0));
                    ImGui::GetForegroundDrawList()->AddRect(imageMin, imageMax, ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), 0.0f, ImDrawFlags_None, 1.5f);
                }
                else if (file->typeID == GetTypeID<Graphics::Texture>())
                {
                    uint32       textureID   = Resources::ResourceStorage::Get()->GetResource<Graphics::Texture>(file->sid)->GetID();
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