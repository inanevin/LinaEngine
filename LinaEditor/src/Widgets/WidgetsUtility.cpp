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
#include "Widgets/WidgetsUtility.hpp"
#include "Rendering/Texture.hpp"
#include "Core/CommonPhysics.hpp"
#include "Core/CustomFontIcons.hpp"
#include "Core/EditorCommon.hpp"
#include "Core/GUILayer.hpp"
#include "Core/InputBackend.hpp"
#include "Core/PhysicsCommon.hpp"
#include "Core/WindowBackend.hpp"
#include "Drawers/ComponentDrawer.hpp"
#include "EventSystem/EventSystem.hpp"
#include "IconsFontAwesome5.h"
#include "Math/Math.hpp"
#include "Math/Quaternion.hpp"
#include "Physics/PhysicsMaterial.hpp"
#include "Resources/ResourceStorage.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/Model.hpp"
#include "Rendering/Shader.hpp"
#include "Rendering/ShaderInclude.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "Widgets/MenuButton.hpp"
#include "Memory/Memory.hpp"
#include "Core/PhysicsBackend.hpp"
#include "Core/ImGuiCommon.hpp"
#include "Core/Engine.hpp"
#include "Widgets/Snackbar.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "Core/EditorApplication.hpp"
#include "ECS/Components/LightComponent.hpp"

namespace Lina::Editor
{

    struct MovableChildData
    {
        ImVec2 m_pressPosition      = ImVec2(0, 0);
        ImVec2 m_position           = ImVec2(0, 0);
        bool   m_isDragging         = false;
        bool   m_initialPositionSet = false;
    };

    static bool                                          s_isDraggingWidgetInput = false;
    static bool                                          s_mouseReleased         = true;
    static std::string                                   s_draggedInput          = "";
    static float                                         s_valueOnDragStart      = 0.0f;
    static int                                           s_valueOnDragStartInt   = 0;
    static std::map<TypeID, bool>                        m_classFoldoutMap;
    static std::map<TypeID, std::map<const char*, bool>> m_headerFoldoutMap;
    static std::map<std::string, bool>                   m_idFoldoutMap;
    static bool                                          m_shouldShowCameraOptions;
    static float                                         m_editorCameraSpeed           = 1.0f;
    static float                                         m_editorCameraSpeedMultiplier = 1.0f;
    static std::map<std::string, MovableChildData>       m_movableChildData;
    static std::map<StringIDType, uint32>                m_resourcePreviewTextures;
    static float                                         m_editorCameraAspectBeforeSnapshot   = 0.0f;
    static Vector3                                       m_editorCameraLocationBeforeSnapshot = Vector3::Zero;
    static Quaternion                                    m_editorCameraRotationBeforeSnapshot = Quaternion();
    const std::string                                    editorSnapshotLightName              = "##snapshot_light##_lina##";

    void WidgetsUtility::Tooltip(const char* tooltip)
    {
        if (s_isDraggingWidgetInput)
            return;

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        PushPopupStyle();
        ImGui::BeginTooltip();
        ImGui::Text(tooltip);
        ImGui::EndTooltip();
        PopPopupStyle();
        ImGui::PopStyleColor();
    }

    bool WidgetsUtility::ColorsEqual(ImVec4 col1, ImVec4 col2)
    {
        return (col1.x == col2.x && col1.y == col2.y && col1.z == col2.z && col1.w == col2.w);
    }

    bool WidgetsUtility::IsProtectedDirectory(Utility::DirectoryItem* item)
    {
        return item->m_fullPath.compare("Resources/") == 0 || item->m_fullPath.find("Resources/Engine") != std::string::npos ||
               item->m_fullPath.compare("Resources/Sandbox") == 0 || item->m_fullPath.find("Resources/Editor") != std::string::npos;
    }

    bool WidgetsUtility::DrawTreeFolder(Utility::Folder* folder, Utility::Folder*& selectedFolder, bool canRename)
    {
        FramePaddingX(GUILayer::Get()->GetDefaultFramePadding().x);
        const ImVec2             iconArrowMin  = ImVec2(ImGui::GetCursorScreenPos().x + 10, ImGui::GetCursorScreenPos().y);
        const ImVec2             iconFolderMin = ImVec2(ImGui::GetCursorScreenPos().x + 22, ImGui::GetCursorScreenPos().y);
        const ImGuiTreeNodeFlags parent        = ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnDoubleClick;
        const ImGuiTreeNodeFlags leaf          = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth;
        const bool               hasChildren   = folder->m_folders.size() > 0;
        ImGuiTreeNodeFlags       flags         = hasChildren ? parent : leaf;

        static char str0[128] = "Hello, world!";

        if (folder == selectedFolder)
        {
            flags |= ImGuiTreeNodeFlags_Selected;

            if (canRename && Input::InputEngineBackend::Get()->GetKeyDown(LINA_KEY_F2))
            {
                if (IsProtectedDirectory(folder))
                {
                    Snackbar::PushSnackbar(LogLevel::Warn, "The Root, Engine, Editor and Sandbox folders can not be renamed!");
                }
                else
                {
                    Memory::memset(str0, 0, 128);
                    Memory::memcpy(str0, folder->m_name.c_str(), folder->m_name.capacity() + 1);
                    folder->m_isRenaming = true;
                }
            }
        }

        // Tree node.
        IncrementCursorPosX(4);

        bool node = false;

        if (canRename && folder->m_isRenaming)
        {
            IncrementCursorPosX(34);

            ImVec4 lighterFrameBG = ImGui::GetStyleColorVec4(ImGuiCol_PopupBg);
            lighterFrameBG.x += 0.1f;
            lighterFrameBG.y += 0.1f;
            lighterFrameBG.z += 0.1f;
            ImGui::PushStyleColor(ImGuiCol_FrameBg, lighterFrameBG);
            const std::string inputLabel = "##_A_" + folder->m_name;
            if (ImGui::InputText(inputLabel.c_str(), str0, IM_ARRAYSIZE(str0), ImGuiInputTextFlags_EnterReturnsTrue))
            {
                folder->m_isRenaming = false;
                Utility::ChangeFolderName(folder, str0);
            }
            ImGui::PopStyleColor();

            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsItemClicked())
                folder->m_isRenaming = false;
        }
        else
        {
            node = ImGui::TreeNodeEx((void*)&folder->m_fullPath, flags, folder->m_name.c_str());
        }

        if (hasChildren)
        {
            PushIconFontSmall();
            const char* arrow = folder->m_isOpen ? ICON_FA_CARET_DOWN : ICON_FA_CARET_RIGHT;
            ImGui::GetWindowDrawList()->AddText(iconArrowMin, ImGui::GetColorU32(ImGui::GetStyleColorVec4(ImGuiCol_Text)), arrow);
            ImGui::PopFont();
        }

        // Folder icon
        const ImVec4 folderColor = ImVec4(1.0f, 1.0f, 1.0f, 0.65f);
        PushIconFontSmall();
        ImGui::PushStyleColor(ImGuiCol_Text, folderColor);
        ImGui::GetWindowDrawList()->AddText(iconFolderMin, ImGui::GetColorU32(ImGui::GetStyleColorVec4(ImGuiCol_Text)), ICON_FA_FOLDER);
        ImGui::PopStyleColor();
        ImGui::PopFont();

        PopStyleVar();
        return node;
    }

    void WidgetsUtility::SaveEditorCameraBeforeSnapshot(float aspectRatio)
    {
        auto& editorCamSystem                = EditorApplication::Get()->GetCameraSystem();
        auto& data                           = ECS::Registry::Get()->get<ECS::EntityDataComponent>(editorCamSystem.GetEditorCamera());
        m_editorCameraLocationBeforeSnapshot = data.GetLocation();
        m_editorCameraRotationBeforeSnapshot = data.GetRotation();

        // Handle aspect resizing.
        auto* renderEngine                 = Graphics::RenderEngineBackend::Get();
        auto* cameraSystem                 = renderEngine->GetCameraSystem();
        m_editorCameraAspectBeforeSnapshot = cameraSystem->GetAspectRatio();

        if (cameraSystem->GetAspectRatio() != aspectRatio)
            cameraSystem->SetAspectRatio(aspectRatio);

        auto*       reg       = ECS::Registry::Get();
        ECS::Entity ent       = reg->CreateEntity(editorSnapshotLightName);
        auto&       lightData = reg->get<ECS::EntityDataComponent>(ent);
        lightData.SetLocation(Vector3(5, 8, 0));
        reg->emplace<ECS::DirectionalLightComponent>(ent);
    }

    void WidgetsUtility::SetEditorCameraForSnapshot()
    {
        auto& editorCamSystem = EditorApplication::Get()->GetCameraSystem();
        auto& data            = ECS::Registry::Get()->get<ECS::EntityDataComponent>(editorCamSystem.GetEditorCamera());
        data.SetLocation(Vector3(0, 1, -4));
        data.SetRotation(Quaternion::LookAt(data.GetLocation(), Vector3::Zero, Vector3::Up));
    }

    void WidgetsUtility::ResetEditorCamera()
    {
        auto* reg = ECS::Registry::Get();
        reg->DestroyEntity(reg->GetEntity(editorSnapshotLightName));
        // Reset aspect resizing.
        auto* renderEngine = Graphics::RenderEngineBackend::Get();
        auto* cameraSystem = renderEngine->GetCameraSystem();
        if (cameraSystem->GetAspectRatio() != m_editorCameraAspectBeforeSnapshot)
            cameraSystem->SetAspectRatio(m_editorCameraAspectBeforeSnapshot);

        // Reset camera transformation.
        auto& camSystem = EditorApplication::Get()->GetCameraSystem();
        auto& data      = ECS::Registry::Get()->get<ECS::EntityDataComponent>(camSystem.GetEditorCamera());
        data.SetLocation(m_editorCameraLocationBeforeSnapshot);
        data.SetRotation(m_editorCameraRotationBeforeSnapshot);
    }

    void WidgetsUtility::DrawResourceNode(Utility::DirectoryItem* item, bool selected, bool* renamedItem, float sizeMultiplier, bool canRename)
    {
#pragma warning(disable : 4312)
        auto*              storage            = Resources::ResourceStorage::Get();
        const ImVec2       cursorPosScreen    = ImGui::GetCursorScreenPos();
        const ImVec2       cursorPos          = ImGui::GetCursorPos();
        const float        fileTypeRectHeight = 5;
        const ImVec2       imageSize          = ImVec2(70 * sizeMultiplier, 70 * sizeMultiplier);
        const ImVec2       totalSize          = ImVec2(imageSize.x + 30, 40 + imageSize.y);
        const float        windowWidth        = ImGui::GetWindowWidth();
        const ImVec2       itemRectMin        = ImVec2(cursorPosScreen.x, cursorPosScreen.y);
        const ImVec2       itemRectMax        = ImVec2(cursorPosScreen.x + totalSize.x, cursorPosScreen.y + totalSize.y);
        const bool         hovered            = ImGui::IsMouseHoveringRect(itemRectMin, itemRectMax);
        ImVec4             childBG            = selected ? ImGui::GetStyleColorVec4(ImGuiCol_FolderActive) : hovered ? ImGui::GetStyleColorVec4(ImGuiCol_FolderHovered)
                                                                                                                     : ImGui::GetStyleColorVec4(ImGuiCol_ChildBg);
        const std::string  fullPath           = item->m_fullPath;
        const bool         isFolder           = item->m_typeID == 0;
        const StringIDType sid                = item->m_sid;

        ImGui::BeginChild(fullPath.c_str(), totalSize, false, ImGuiWindowFlags_NoScrollbar);

        // Background.
        ImGui::GetWindowDrawList()->AddRectFilled(cursorPosScreen, ImVec2(cursorPosScreen.x + totalSize.x, cursorPosScreen.y + totalSize.y), ImGui::ColorConvertFloat4ToU32(childBG), 2.0f);

        uint32 textureID = 0;

        if (isFolder)
            textureID = storage->GetResource<Graphics::Texture>("Resources/Editor/Textures/Folder.png")->GetID();
        else
        {
            if (item->m_typeID == GetTypeID<Graphics::Texture>())
                textureID = storage->GetResource<Graphics::Texture>(fullPath)->GetID();
            else if (item->m_typeID == GetTypeID<Graphics::Model>() || item->m_typeID == GetTypeID<Graphics::Material>())
            {
                if (m_resourcePreviewTextures.find(item->m_sid) == m_resourcePreviewTextures.end())
                    m_resourcePreviewTextures[item->m_sid] = EditorApplication::Get()->GetSnapshotTexture(item->m_sid);

                textureID = m_resourcePreviewTextures[item->m_sid];
            }
            else if (item->m_typeID == GetTypeID<Graphics::Shader>() || item->m_typeID == GetTypeID<Graphics::ShaderInclude>())
                textureID = storage->GetResource<Graphics::Texture>("Resources/Editor/Textures/FileIcon_Shader.png")->GetID();
            else if (item->m_typeID == GetTypeID<Audio::Audio>())
                textureID = storage->GetResource<Graphics::Texture>("Resources/Editor/Textures/FileIcon_Audio.png")->GetID();
            else if (item->m_typeID == GetTypeID<Physics::PhysicsMaterial>())
                textureID = storage->GetResource<Graphics::Texture>("Resources/Editor/Textures/FileIcon_PhyMat.png")->GetID();
        }

        // Prepare border sizes from incremented cursor.
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2.0f - imageSize.x / 2.0f);
        WidgetsUtility::IncrementCursorPosY(5);
        const ImVec2 borderMin = ImGui::GetCursorScreenPos();
        const ImVec2 borderMax = ImVec2(borderMin.x + imageSize.x, borderMin.y + imageSize.y);

        // Add checkered background to cover transparents.
        if (!isFolder)
            ImGui::GetWindowDrawList()->AddImage((void*)storage->GetResource<Graphics::Texture>("Resources/Editor/Textures/Checkered.png")->GetID(), borderMin, borderMax, ImVec2(0, 1), ImVec2(1, 0));

        // Add the actual resource image.
        ImGui::Image((void*)textureID, imageSize, ImVec2(0, 1), ImVec2(1, 0));

        if (ImGui::IsItemHovered())
        {
            if (item->m_typeID == GetTypeID<Graphics::Model>())
                Tooltip("Model");
            else if (item->m_typeID == GetTypeID<Graphics::Material>())
                Tooltip("Material");
            else if (item->m_typeID == GetTypeID<Graphics::Texture>())
                Tooltip("Texture");
            else if (item->m_typeID == GetTypeID<Graphics::Shader>())
                Tooltip("Shader");
            else if (item->m_typeID == GetTypeID<Graphics::ShaderInclude>())
                Tooltip("Shader Include");
            else if (item->m_typeID == GetTypeID<Audio::Audio>())
                Tooltip("Audio");
            else if (item->m_typeID == GetTypeID<Physics::PhysicsMaterial>())
                Tooltip("Physics Material");
        }

        if (item->m_typeID == GetTypeID<Graphics::Model>())
        {
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
            {
                ImGui::SetDragDropPayload(RESOURCES_MOVEMODEL_ID, &sid, sizeof(StringIDType));

                // Display preview
                ImGui::Text(fullPath.c_str());
                ImGui::EndDragDropSource();
            }
        }

        // Draw a small colored rect indicating resource type.
        if (!isFolder)
        {
            const ImVec2 extRectMin   = ImVec2(borderMin.x, borderMax.y - fileTypeRectHeight);
            const ImVec2 extRectMax   = ImVec2(extRectMin.x + imageSize.x, extRectMin.y + fileTypeRectHeight);
            const Color  extColorLina = storage->GetTypeColor(item->m_typeID);
            const ImVec4 extColor     = ImVec4(extColorLina.r, extColorLina.g, extColorLina.b, extColorLina.a);
            ImGui::GetWindowDrawList()->AddRectFilled(extRectMin, extRectMax, ImGui::ColorConvertFloat4ToU32(extColor), 2.0f, ImDrawFlags_RoundCornersTop);
        }

        // Add image border afterwards, should overlay the image.
        if (!isFolder)
            ImGui::GetWindowDrawList()->AddRect(borderMin, borderMax, ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f)), 0.0f, 0, 1.5f);

        const std::string name           = Utility::GetFileWithoutExtension(Utility::GetFileNameOnly(fullPath));
        const float       textSize       = ImGui::CalcTextSize(name.c_str()).x;
        const bool        shouldWrapText = textSize >= imageSize.x + 3;

        static char str0[128] = "Hello, world!";

        if (selected && canRename && Input::InputEngineBackend::Get()->GetKeyDown(LINA_KEY_F2))
        {

            if (IsProtectedDirectory(item))
            {
                Snackbar::PushSnackbar(LogLevel::Warn, "The Root, Engine, Editor and Sandbox folders can not be renamed!");
            }
            else
            {
                item->m_isRenaming = true;
                Memory::memset(str0, 0, 128);
                Memory::memcpy(str0, item->m_name.c_str(), item->m_name.capacity() + 1);
            }
        }

        if (item->m_isRenaming)
        {
            const float inputTextWidth = ImGui::GetWindowWidth() * 0.7f;
            ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2.0f - inputTextWidth / 2.0f);

            ImGui::PushItemWidth(inputTextWidth);
            const std::string inputLabel = "##_A_" + item->m_fullPath;
            if (ImGui::InputText(inputLabel.c_str(), str0, IM_ARRAYSIZE(str0), ImGuiInputTextFlags_EnterReturnsTrue))
            {
                item->m_isRenaming = false;

                if (!isFolder)
                {
                    // If we have an asset data associated with the file, change it's name as well.
                    auto* parent = item->m_parent;
                    if (parent != nullptr)
                    {
                        for (auto* subfile : parent->m_files)
                        {
                            if (subfile != item && subfile->m_name.compare(item->m_name) == 0)
                                Utility::ChangeFileName(subfile, str0);
                        }
                    }

                    Utility::ChangeFileName(static_cast<Utility::File*>(item), str0);
                }
                else
                    Utility::ChangeFolderName(static_cast<Utility::Folder*>(item), str0);

                *renamedItem = true;
            }

            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsItemClicked())
                item->m_isRenaming = false;
        }
        else
        {

            ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2.0f - textSize / 2.0f);

            if (shouldWrapText)
                ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + totalSize.x);

            ImGui::Text(name.c_str());

            if (shouldWrapText)
                ImGui::PopTextWrapPos();
        }

        ImGui::EndChild();

        if (cursorPos.x + (totalSize.x + ImGui::GetStyle().ItemSpacing.x) * 2.0f < windowWidth)
            ImGui::SameLine();
        else
            ImGui::SetCursorPosX(16);
    }

    void WidgetsUtility::ColorButton(const char* id, float* colorX)
    {
        static bool         alpha_preview      = true;
        static bool         alpha_half_preview = false;
        static bool         drag_and_drop      = true;
        static bool         options_menu       = true;
        static bool         hdr                = true;
        ImGuiColorEditFlags misc_flags         = (hdr ? (ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float) : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) |
                                         (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);
        // Generate a dummy default palette. The palette will persist and can be edited.
        static bool   saved_palette_init = true;
        static ImVec4 saved_palette[32]  = {};
        if (saved_palette_init)
        {
            for (int n = 0; n < IM_ARRAYSIZE(saved_palette); n++)
            {
                ImGui::ColorConvertHSVtoRGB(n / 31.0f, 0.8f, 0.8f, saved_palette[n].x, saved_palette[n].y, saved_palette[n].z);
                saved_palette[n].w = 1.0f; // Alpha
            }
            saved_palette_init = false;
        }

        std::string   buf(id);
        static ImVec4 backup_color;
        bool          open_popup = ImGui::ColorButton(buf.c_str(), ImVec4(colorX[0], colorX[1], colorX[2], colorX[3]), misc_flags);
        buf.append("##p");
        if (open_popup)
        {
            ImGui::OpenPopup(buf.c_str());
            backup_color = ImVec4(colorX[0], colorX[1], colorX[2], colorX[3]);
        }
        if (ImGui::BeginPopup(buf.c_str()))
        {
            ImGui::PushItemWidth(160);

            buf.append("##picker");
            ImGui::Separator();
            ImGui::ColorPicker4(buf.c_str(), colorX, misc_flags | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);
            ImGui::SameLine();

            buf.append("##current");
            ImGui::BeginGroup(); // Lock X position
            ImGui::Text("Current");
            ImGui::ColorButton(buf.c_str(), ImVec4(colorX[0], colorX[1], colorX[2], colorX[3]), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40));
            ImGui::Text("Previous");

            if (ImGui::ColorButton("##previous", backup_color, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40)))
            {
                colorX[0] = backup_color.x;
                colorX[1] = backup_color.y;
                colorX[2] = backup_color.z;
                colorX[3] = backup_color.w;
            }

            ImGui::EndGroup();
            ImGui::EndPopup();
        }
    }

    bool WidgetsUtility::ToggleButton(const char* label, bool* v, ImVec2 size)
    {
        bool         isPressed            = false;
        const ImVec4 inactiveColor        = ImVec4(0.85f, 0.85f, 0.85f, 1.0f);
        const ImVec4 inactiveHoveredColor = ImVec4(0.78f, 0.78f, 0.78f, 1.0f);
        const ImVec4 activeColor          = ImGui::GetStyleColorVec4(ImGuiCol_Header);
        const ImVec4 activeHoveredColor   = ImGui::GetStyleColorVec4(ImGuiCol_Header);

        ImVec2      p         = ImGui::GetCursorScreenPos();
        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        if (size.x == 0.0f && size.y == 0.0f)
            size = DEFAULT_TOGGLE_SIZE;

        float radius = size.y * 0.50f;

        ImGui::InvisibleButton(label, size);
        if (v != nullptr && ImGui::IsItemClicked())
            *v = !*v;

        float t = v == nullptr ? 1.0f : *v ? 1.0f
                                           : 0.0f;

        ImVec4 usedActiveColor = activeColor;
        if (v == nullptr)
            usedActiveColor = ImVec4(activeColor.x - 0.15f, activeColor.y - 0.15f, activeColor.z - 0.15f, activeColor.w);

        ImGuiContext& g          = *GImGui;
        float         ANIM_SPEED = 0.08f;
        if (v != nullptr && g.LastActiveId == g.CurrentWindow->GetID(label)) // && g.LastActiveIdTimer < ANIM_SPEED)
        {
            float t_anim = ImSaturate(g.LastActiveIdTimer / ANIM_SPEED);
            t            = *v ? (t_anim) : (1.0f - t_anim);
        }

        ImU32 col_bg;
        if (ImGui::IsItemHovered() && v != nullptr)
            col_bg = ImGui::GetColorU32(ImLerp(inactiveHoveredColor, activeHoveredColor, t));
        else
            col_bg = ImGui::GetColorU32(ImLerp(inactiveColor, usedActiveColor, t));

        draw_list->AddRectFilled(p, ImVec2(p.x + size.x, p.y + size.y), col_bg, size.y * 0.5f);
        draw_list->AddCircleFilled(ImVec2(p.x + radius + t * (size.x - radius * 2.0f), p.y + radius), radius - 1.5f, IM_COL32(255, 255, 255, 255));

        return v != nullptr ? *v : false;
    }

    void WidgetsUtility::BeginMovableChild(const char* childID, ImVec2 size, const ImVec2& defaultPosition, const ImRect& confineRect, bool isHorizontal, ImVec2 iconCursorOffset)
    {
        const ImVec2 confineSize = ImVec2(confineRect.Max.x - confineRect.Min.x, confineRect.Max.y - confineRect.Min.y);
        const float  iconOffset  = 12.0f;
        if (isHorizontal)
            size.x += iconOffset;
        else
            size.y += iconOffset;

        // Set the position only if first launch.
        const std::string childIDStr     = std::string(childID);
        ImVec2            targetPosition = ImVec2(confineRect.Min.x + m_movableChildData[childIDStr].m_position.x, confineRect.Min.y + m_movableChildData[childIDStr].m_position.y);

        if (targetPosition.x > confineRect.Max.x - size.x)
            targetPosition.x = confineRect.Max.x - size.x;
        if (targetPosition.y > confineRect.Max.y - size.y)
            targetPosition.y = confineRect.Max.y - size.y;

        ImGui::SetNextWindowPos(targetPosition);

        ImGui::BeginChild(childID, size, true);
        ImGui::SetCursorPos(iconCursorOffset);

        IconSmall(isHorizontal ? ICON_FA_ELLIPSIS_V : ICON_FA_ELLIPSIS_H);

        if (ImGui::IsItemClicked())
        {
            if (!m_movableChildData[childIDStr].m_isDragging)
            {
                m_movableChildData[childIDStr].m_isDragging    = true;
                m_movableChildData[childIDStr].m_pressPosition = m_movableChildData[childIDStr].m_position;
            }
        }
        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        {
            m_movableChildData[childIDStr].m_isDragging = false;
        }

        if (ImGui::IsMouseDragging(ImGuiMouseButton_Left) && m_movableChildData[childIDStr].m_isDragging)
        {
            // Calc new window position.
            const ImVec2 pressPos        = m_movableChildData[childIDStr].m_pressPosition;
            const ImVec2 drag            = ImGui::GetMouseDragDelta();
            ImVec2       desiredPosition = ImVec2(pressPos.x + drag.x, pressPos.y + drag.y);

            // Confine window position to confine rect.
            const float positionLimitPadding = 2.0f;
            if (desiredPosition.x < positionLimitPadding)
                desiredPosition.x = positionLimitPadding;
            else if (desiredPosition.x > confineSize.x - positionLimitPadding - size.x)
                desiredPosition.x = confineSize.x - positionLimitPadding - size.x;
            if (desiredPosition.y < positionLimitPadding)
                desiredPosition.y = positionLimitPadding;
            else if (desiredPosition.y > confineSize.y - positionLimitPadding - size.y)
                desiredPosition.y = confineSize.y - positionLimitPadding - size.y;

            m_movableChildData[childIDStr].m_position = desiredPosition;
        }
        else
        {
            if (!m_movableChildData[childIDStr].m_initialPositionSet)
            {
                m_movableChildData[childIDStr].m_initialPositionSet = true;
                m_movableChildData[childIDStr].m_position           = defaultPosition;
            }
        }
    }

    void WidgetsUtility::DisplayEditorCameraSettings(const ImVec2 position)
    {
        ImVec2 cameraSettingsSize = ImVec2(210, 60);
        ImGui::SetNextWindowPos(position);
        ImGui::SetNextWindowBgAlpha(0.5f);
        ImGui::BeginChild("##scenePanel_cameraSettings", cameraSettingsSize, false, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking);
        float cursorPosLabels = 12;
        WidgetsUtility::IncrementCursorPosY(6);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 0));
        ImGui::SetCursorPosX(cursorPosLabels);
        WidgetsUtility::PropertyLabel("Camera Speed");
        ImGui::SameLine();

        float cursorPosValues = ImGui::CalcTextSize("Camera Speed").x + 24;
        ImGui::SetCursorPosX(cursorPosValues);
        ImGui::SetNextItemWidth(100);

        ImGui::SliderFloat("##editcamspd", &m_editorCameraSpeed, 0.0f, 1.0f);
        ImGui::SetCursorPosX(cursorPosLabels);
        WidgetsUtility::PropertyLabel("Multiplier");
        ImGui::SameLine();
        ImGui::SetCursorPosX(cursorPosValues);
        ImGui::SetNextItemWidth(100);
        ImGui::DragFloat("##editcammultip", &m_editorCameraSpeedMultiplier, 1.0f, 0.0f, 20.0f);
        ImGui::PopStyleVar();
        EditorApplication::Get()->GetCameraSystem().SetCameraSpeedMultiplier(m_editorCameraSpeed * m_editorCameraSpeedMultiplier);
        ImGui::EndChild();
    }

    void WidgetsUtility::TransformOperationTools(const char* childID, ImRect confineRect)
    {
        const float   buttonsHorizontalOffset = 8.0f;
        const float   childYSpaceOffset       = 4.0f;
        const float   rounding                = 4.0f;
        const float   itemSpacingY            = ImGui::GetStyle().ItemSpacing.y;
        const float   childRounding           = 6.0f;
        const ImVec2  buttonSize              = ImVec2(28, 28);
        const ImVec2  currentWindowPos        = ImGui::GetWindowPos();
        const ImVec2  contentOffset           = ImVec2(0.5f, -2.0f);
        static ImVec2 childSize               = ImVec2(buttonSize.x + buttonsHorizontalOffset * 2, buttonSize.y * 4 + itemSpacingY * 5 + childYSpaceOffset * 2);

        ImGui::SetNextWindowBgAlpha(0.5f);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, childRounding);

        WidgetsUtility::BeginMovableChild(childID, childSize, ImVec2(CURSOR_X_LABELS, CURSOR_X_LABELS), confineRect, false, ImVec2(buttonsHorizontalOffset + 7, 0));

        PushIconFontSmall();
        ImGui::SetCursorPosX(buttonsHorizontalOffset);

        const int  currentOperation = GUILayer::Get()->GetLevelPanel().GetTransformOperation();
        const bool isGlobal         = GUILayer::Get()->GetLevelPanel().GetIsGizmoGlobal();
        bool       isLocked         = currentOperation == 0;

        if (Button(ICON_FA_ARROWS_ALT, buttonSize, 1, rounding, contentOffset, isLocked))
            Event::EventSystem::Get()->Trigger<ETransformGizmoChanged>(ETransformGizmoChanged{0});
        if (ImGui::IsItemHovered())
        {
            ImGui::PopFont();
            Tooltip("Move");
            PushIconFontSmall();
        }

        isLocked = currentOperation == 1;
        ImGui::SetCursorPosX(buttonsHorizontalOffset);
        if (Button(ICON_FA_SYNC_ALT, buttonSize, 1, rounding, contentOffset, isLocked))
            Event::EventSystem::Get()->Trigger<ETransformGizmoChanged>(ETransformGizmoChanged{1});
        if (ImGui::IsItemHovered())
        {
            ImGui::PopFont();
            Tooltip("Rotate");
            PushIconFontSmall();
        }
        isLocked = currentOperation == 2;
        ImGui::SetCursorPosX(buttonsHorizontalOffset);
        if (Button(ICON_FA_COMPRESS_ALT, buttonSize, 1, rounding, contentOffset, isLocked))
            Event::EventSystem::Get()->Trigger<ETransformGizmoChanged>(ETransformGizmoChanged{2});
        if (ImGui::IsItemHovered())
        {
            ImGui::PopFont();
            Tooltip("Scale");
            PushIconFontSmall();
        }

        ImGui::SetCursorPosX(buttonsHorizontalOffset);
        const char* icon = isGlobal ? ICON_FA_GLOBE_AMERICAS : ICON_FA_CUBE;
        if (Button(icon, buttonSize, 1, rounding, contentOffset))
            Event::EventSystem::Get()->Trigger<ETransformPivotChanged>(ETransformPivotChanged{!isGlobal});

        if (ImGui::IsItemHovered())
        {
            ImGui::PopFont();
            Tooltip(isGlobal ? "Current: World" : "Current: Local");
            PushIconFontSmall();
        }

        ImGui::PopFont();
        ImGui::EndChild();
        ImGui::PopStyleVar();
    }

    void WidgetsUtility::PlayOperationTools(const char* childID, ImRect confineRect)
    {
        const float   rounding         = 4.0f;
        const float   itemSpacingX     = ImGui::GetStyle().ItemSpacing.x;
        const float   itemSpacingY     = ImGui::GetStyle().ItemSpacing.y;
        const float   childRounding    = 6.0f;
        const ImVec2  buttonSize       = ImVec2(28, 28);
        const ImVec2  currentWindowPos = ImGui::GetWindowPos();
        const ImVec2  contentOffset    = ImVec2(0.5f, -2.0f);
        static ImVec2 childSize        = ImVec2(buttonSize.x * 6 + itemSpacingX * 7, buttonSize.y + itemSpacingY * 3);

        ImGui::SetNextWindowBgAlpha(0.5f);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, childRounding);
        const ImVec2 defaultPos = ImVec2((confineRect.Max.x - confineRect.Min.x) - childSize.x - CURSOR_X_LABELS - 10, CURSOR_X_LABELS);

        WidgetsUtility::BeginMovableChild(childID, childSize, defaultPos, confineRect, true, ImVec2(5, 10));
        ImGui::SameLine();
        PushIconFontSmall();
        ImGui::SetCursorPos(ImVec2(17, 6));

        const bool isInPlayMode = Engine::Get()->GetPlayMode();
        const bool isPaused     = Engine::Get()->GetIsPaused();

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 0.9f, 0.2f, 1.0f));
        if (Button(ICON_FA_PLAY, buttonSize, 1, rounding, contentOffset, isInPlayMode))
            Engine::Get()->SetPlayMode(!isInPlayMode);
        ImGui::PopStyleColor();

        if (ImGui::IsItemHovered())
        {
            ImGui::PopFont();
            Tooltip("Enter Playmode");
            PushIconFontSmall();
        }

        ImGui::SameLine();
        if (Button(ICON_FA_PAUSE, buttonSize, 1, rounding, contentOffset, isPaused))
            Engine::Get()->SetIsPaused(!isPaused);

        if (ImGui::IsItemHovered())
        {
            ImGui::PopFont();
            Tooltip("Pause");
            PushIconFontSmall();
        }

        ImGui::SameLine();
        if (Button(ICON_FA_FAST_FORWARD, buttonSize, 1, rounding, contentOffset))
            Engine::Get()->SkipNextFrame();

        if (ImGui::IsItemHovered())
        {
            ImGui::PopFont();
            Tooltip("Skip Frame");
            PushIconFontSmall();
        }

        ImGui::SameLine();
        if (Button(ICON_FA_CAMERA, buttonSize, 1, rounding, contentOffset, m_shouldShowCameraOptions))
        {
            m_shouldShowCameraOptions = !m_shouldShowCameraOptions;
        }

        if (ImGui::IsItemHovered())
        {
            ImGui::PopFont();
            Tooltip("Show Camera Options");
            PushIconFontSmall();
        }

        ImGui::SameLine();
        auto& levelPanel = GUILayer::Get()->GetLevelPanel();
        if (Button(ICON_FA_EYE, buttonSize, 1, rounding, contentOffset, levelPanel.m_shouldShowGizmos))
            levelPanel.m_shouldShowGizmos = !levelPanel.m_shouldShowGizmos;

        if (ImGui::IsItemHovered())
        {
            ImGui::PopFont();
            Tooltip("Show Gizmos");
            PushIconFontSmall();
        }

        ImGui::SameLine();
        if (Button(ICON_FA_VECTOR_SQUARE, buttonSize, 1, rounding, contentOffset, levelPanel.m_shouldShowBounds))
            levelPanel.m_shouldShowBounds = !levelPanel.m_shouldShowBounds;

        if (ImGui::IsItemHovered())
        {
            ImGui::PopFont();
            Tooltip("Show Bounds");
            PushIconFontSmall();
        }

        ImGui::PopFont();
        ImGui::EndChild();
        ImGui::PopStyleVar();

        if (m_shouldShowCameraOptions)
        {
            const ImVec2 pos = m_movableChildData[childID].m_position;
            DisplayEditorCameraSettings(ImVec2(confineRect.Min.x + pos.x, confineRect.Min.y + pos.y + childSize.y + 2));
        }
    }

    int WidgetsUtility::SelectPrimitiveCombobox(const char* comboID, const std::vector<std::string>& primitives, int currentSelected, float widthDecrease)
    {
        int primitiveToReturn = currentSelected;

        const std::string label = Utility::GetFileWithoutExtension(Utility::GetFileNameOnly(primitives[currentSelected]));
        if (BeginComboBox(comboID, label.c_str(), false, widthDecrease))
        {
            for (int i = 0; i < primitives.size(); i++)
            {
                const bool        selected     = i == currentSelected;
                const std::string primitiveStr = Utility::GetFileWithoutExtension(Utility::GetFileNameOnly(primitives[i]));
                if (ImGui::Selectable(primitiveStr.c_str(), selected))
                    primitiveToReturn = i;

                if (selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        return primitiveToReturn;
    }

    void WidgetsUtility::HorizontalDivider(float yOffset, float thickness, float maxOverride)
    {
        const float maxWidth = maxOverride == 0.0f ? ImGui::GetWindowPos().x + ImGui::GetWindowWidth() : ImGui::GetWindowPos().x + maxOverride;
        ImVec2      min      = ImVec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + ImGui::GetCursorPosY() + yOffset);
        ImVec2      max      = ImVec2(maxWidth, min.y);
        ImGui::GetWindowDrawList()->AddLine(min, max, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_Text)), thickness);
    }

    void WidgetsUtility::WindowButtons(const char* windowID, float yOffset, bool isAppWindow, float sizeMultiplier)
    {

        ImVec2 windowPos   = ImGui::GetWindowPos();
        float  windowWidth = ImGui::GetWindowWidth();

        const float  offset1                   = 35.0f;
        const float  gap                       = 25.0f;
        const ImVec2 buttonSize                = ImVec2(25 * sizeMultiplier, 20 * sizeMultiplier);
        const float  closeButtonAdditionalSize = 8;
        const float  frameRounding             = 0.4f;

        ImGui::SetCursorPosY(yOffset);
        const float cursorY = ImGui::GetCursorPosY();
        ImGui::SetCursorPosX(windowWidth - offset1 - closeButtonAdditionalSize - gap * 2);

        PushIconFontSmall();
        if (Button(ICON_FA_MINUS, buttonSize, 1.0f, frameRounding, ImVec2(0.0f, -0.6f)))
        {
            if (isAppWindow)
                Graphics::WindowBackend::Get()->Iconify();
            else
                GUILayer::Get()->m_editorPanels[windowID]->ToggleCollapse();
        }

        ImGui::SameLine();
        ImGui::SetCursorPosX(windowWidth - offset1 - closeButtonAdditionalSize - gap);
        ImGui::SetCursorPosY(cursorY);

        bool isMaximized = isAppWindow ? Graphics::WindowBackend::Get()->GetProperties().m_windowState == WindowState::Maximized : GUILayer::Get()->m_editorPanels[windowID]->IsMaximized();
        if (Button(isMaximized ? ICON_FA_WINDOW_RESTORE : ICON_FA_WINDOW_MAXIMIZE, buttonSize, 1.0f, frameRounding, ImVec2(0.0f, -2.0f)))
        {
            if (isAppWindow)
                Graphics::WindowBackend::Get()->Maximize();
            else
                GUILayer::Get()->m_editorPanels[windowID]->ToggleMaximize();
        }

        ImGui::SameLine();
        ImGui::SetCursorPosX(windowWidth - offset1 - closeButtonAdditionalSize);
        ImGui::SetCursorPosY(cursorY);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.3f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.5f, 0.3f, 0.3f, 1.0f));
        if (Button(ICON_FA_TIMES, ImVec2(buttonSize.x + closeButtonAdditionalSize, buttonSize.y), 1.0f, frameRounding, ImVec2(0.0f, -2.2f)))
        {
            if (isAppWindow)
                Graphics::WindowBackend::Get()->Close();
            else
                GUILayer::Get()->m_editorPanels[windowID]->Close();
        }

        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopFont();
    }

    void WidgetsUtility::WindowTitlebar(const char* id)
    {
        if (ImGui::IsWindowDocked())
            return;

        const ImVec2 windowPos = ImGui::GetWindowPos();
        const ImU32  textCol   = ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_Text));
        const ImVec2 iconPos   = ImVec2(windowPos.x + 12.5f, windowPos.y + 3);
        const ImVec2 textPos   = ImVec2(iconPos.x + ImGui::GetStyle().ItemSpacing.x + 12, windowPos.y + 3);
        PushIconFontSmall();
        ImGui::GetWindowDrawList()->AddText(iconPos, textCol, GUILayer::Get()->m_windowIconMap[id]);
        ImGui::PopFont();
        ImGui::GetWindowDrawList()->AddText(textPos, textCol, GUILayer::Get()->m_editorPanels[id]->GetTitle().c_str());

        // Draw Buttons
        WindowButtons(id, 4.0f, false, 0.9f);
    }

    bool WidgetsUtility::ComponentHeader(TypeID tid, const char* componentLabel, const char* componentIcon, bool* toggled, bool* removed, bool* copied, bool* pasted, bool* resetted, bool moveButton, bool disableHeader)
    {
        const ImVec2 cursorPos  = ImGui::GetCursorPos();
        const ImVec2 windowPos  = ImGui::GetWindowPos();
        const ImVec2 windowSize = ImGui::GetWindowSize();
        const ImVec2 rectSize   = ImVec2(windowSize.x, HEADER_WIDGET_HEIGHT);

        // Draw background
        const ImVec4 normalColor   = ImGui::GetStyleColorVec4(ImGuiCol_TitleHeader);
        const ImVec4 pressColor    = ImGui::GetStyleColorVec4(ImGuiCol_TitleHeaderPressed);
        const ImVec4 hoverColor    = ImGui::GetStyleColorVec4(ImGuiCol_TitleHeaderHover);
        const ImVec4 disabledColor = ImGui::GetStyleColorVec4(ImGuiCol_TitleHeaderDisabled);
        const ImVec2 rectMin       = ImVec2(windowPos.x, windowPos.y + cursorPos.y);
        const ImVec2 rectMax       = ImVec2(rectMin.x + rectSize.x, rectMin.y + rectSize.y);
        const bool   bgHovered     = !disableHeader && ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(rectMin, rectMax);
        const bool   bgPressed     = bgHovered && ImGui::IsMouseDown(ImGuiMouseButton_Left);
        const bool   bgReleased    = bgHovered && Input::InputEngineBackend::Get()->GetMouseButtonDown(0);
        const ImVec4 bgColor       = disableHeader ? disabledColor : bgPressed ? pressColor
                                                                 : bgHovered   ? hoverColor
                                                                               : normalColor;
        ImGui::GetWindowDrawList()->AddRectFilled(rectMin, rectMax, ImGui::ColorConvertFloat4ToU32(bgColor));

        const ImVec4 borderColor = ImGui::GetStyleColorVec4(ImGuiCol_TitleHeaderBorder);
        ImGui::GetWindowDrawList()->AddRect(rectMin, rectMax, ImGui::ColorConvertFloat4ToU32(borderColor));

        WidgetsUtility::IncrementCursorPosY(3.4f);
        ImGui::SetCursorPosX(CURSOR_X_LABELS);

        IconSmall(m_classFoldoutMap[tid] ? ICON_FA_CARET_DOWN : ICON_FA_CARET_RIGHT);
        ImGui::SameLine();

        // Draw component icon.
        if (componentIcon != nullptr)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_Header));
            IconSmall(componentIcon);
            ImGui::PopStyleColor();
            ImGui::SameLine();
        }

        ImGui::Text(componentLabel);

        // Title is the drag and drop target.
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
        {
            // Set payload to carry the type id.
            ImGui::SetDragDropPayload("COMP_MOVE_PAYLOAD", &tid, sizeof(int));

            // Display preview
            ImGui::Text("Move ");
            ImGui::EndDragDropSource();
        }

        // Dropped on another title, swap component orders.
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("COMP_MOVE_PAYLOAD"))
            {
                IM_ASSERT(payload->DataSize == sizeof(TypeID));
                TypeID payloadID = *(const TypeID*)payload->Data;
                Event::EventSystem::Get()->Trigger<EComponentOrderSwapped>(EComponentOrderSwapped{payloadID, tid});
            }

            ImGui::EndDragDropTarget();
        }

        ImGui::SameLine();

        const float cursorPosX = ImGui::GetWindowWidth() - VALUE_OFFSET_FROM_WINDOW - 15;
        ImGui::SetCursorPosX(cursorPosX);
        bool              anyButtonHovered = false;
        const std::string popupName        = "##popup_" + std::string(componentLabel);

        if (IconButton(ICON_FA_COG))
        {
            ImGui::OpenPopup(popupName.c_str());
        }

        if (ImGui::IsItemHovered())
            anyButtonHovered = true;

        WidgetsUtility::PushPopupStyle();

        std::vector<std::pair<const char*, bool*>> buttons;
        buttons.push_back(std::make_pair("Copy", copied));
        buttons.push_back(std::make_pair("Paste", pasted));
        buttons.push_back(std::make_pair("Reset", resetted));
        buttons.push_back(std::make_pair("Remove", removed));

        if (ImGui::BeginPopup(popupName.c_str()))
        {
            for (auto& [name, ptr] : buttons)
            {
                if (ptr == nullptr)
                    ImGui::BeginDisabled();

                if (ImGui::MenuItem(name))
                {
                    if (ptr != nullptr)
                        *ptr = true;
                }

                if (ptr == nullptr)
                    ImGui::EndDisabled();
            }
            ImGui::EndPopup();
        }
        PopPopupStyle();

        const ImVec2 toggleSize = DEFAULT_TOGGLE_SIZE;
        ImGui::SameLine();
        ImGui::SetCursorPosX(cursorPosX - ImGui::GetStyle().ItemSpacing.x - toggleSize.x);
        WidgetsUtility::IncrementCursorPosY(2.1f);

        if (toggled != nullptr)
        {
            // Toggle
            const std::string toggleID = "##_toggle_" + std::string(componentLabel);
            ToggleButton(toggleID.c_str(), toggled);

            if (ImGui::IsItemHovered())
                anyButtonHovered = true;
        }

        if (bgReleased && !anyButtonHovered)
            m_classFoldoutMap[tid] = !m_classFoldoutMap[tid];

        ImGui::SetCursorPos(cursorPos);
        IncrementCursorPosY(rectSize.y + ImGui::GetStyle().ItemSpacing.y);
        return m_classFoldoutMap[tid];
    }

    bool WidgetsUtility::Header(TypeID tid, const char* label, ImVec2* outCursorPos)
    {
        const ImVec2 windowSize            = ImGui::GetWindowSize();
        const ImVec2 rectSize              = ImVec2(windowSize.x, HEADER_WIDGET_HEIGHT);
        const ImVec4 normalColor           = ImGui::GetStyleColorVec4(ImGuiCol_TitleHeader);
        const ImVec4 pressColor            = ImGui::GetStyleColorVec4(ImGuiCol_TitleHeaderHover);
        const ImVec4 hoverColor            = ImGui::GetStyleColorVec4(ImGuiCol_TitleHeaderPressed);
        const ImVec2 cursorPosBeforeButton = ImGui::GetCursorPos();
        const float  iconScale             = 0.65f;

        ImGui::PushStyleColor(ImGuiCol_Button, normalColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoverColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, normalColor);

        const std::string id = "##_" + std::string(label);
        if (Button(id.c_str(), rectSize))
            m_headerFoldoutMap[tid][label] = !m_headerFoldoutMap[tid][label];

        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();

        const ImVec2 cursorPosAfterButton = ImGui::GetCursorPos();
        const ImVec2 textSize             = ImGui::CalcTextSize(label);
        const ImVec2 cursorPosInside      = ImVec2(cursorPosBeforeButton.x + VALUE_OFFSET_FROM_WINDOW, cursorPosBeforeButton.y + rectSize.y / 2.0f - textSize.y / 2.0f);
        ImGui::SetCursorPos(cursorPosInside);
        IconSmall(m_headerFoldoutMap[tid][label] ? ICON_FA_CARET_DOWN : ICON_FA_CARET_RIGHT);
        ImGui::SameLine();
        ImGui::Text(label);
        ImGui::SameLine();
        ImGui::SetCursorPos(cursorPosAfterButton);

        return m_headerFoldoutMap[tid][label];
    }

    void WidgetsUtility::DropShadow()
    {
        static int   countS     = 4;
        static float thicknessS = 4.0f;
        static float yOffsetS   = 3.0f;

        ImVec4 color        = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
        int    dividerCount = 4;
        float  dividerSize  = 4.0f;
        float  yOffset      = 3.0f;
        float  colorFade    = 1.0f / (float)dividerCount;

        for (int i = 0; i < dividerCount; i++)
        {
            color.w = 1.0f - i * colorFade;
            ImGui::PushStyleColor(ImGuiCol_Text, color);
            HorizontalDivider(i * yOffset, thicknessS);
            ImGui::PopStyleColor();
        }
    }

    bool WidgetsUtility::CaretTitle(const char* title, const std::string& id)
    {
        bool   clicked          = false;
        bool   hovered          = false;
        ImVec4 caretNormalColor = ImVec4(0.75f, 0.75f, 0.75f, 1.0f);
        ImVec4 caretHoverColor  = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        ImVec4 caretColor       = hovered ? caretHoverColor : caretNormalColor;

        ImGui::SetCursorPosX(CURSOR_X_LABELS);

        ImGui::PushStyleColor(ImGuiCol_Text, caretColor);
        IconSmall(m_idFoldoutMap[id] ? ICON_FA_CARET_DOWN : ICON_FA_CARET_RIGHT);
        ImGui::PopStyleColor();

        if (ImGui::IsItemClicked())
            clicked = true;

        ImGui::SameLine();
        IncrementCursorPosY(0);
        ImGui::Text(title);

        if (ImGui::IsItemClicked())
            clicked = true;

        if (clicked)
            m_idFoldoutMap[id] = !m_idFoldoutMap[id];

        return m_idFoldoutMap[id];
    }

    void WidgetsUtility::PropertyLabel(const char* label, bool sameLine, const std::string& tooltip)
    {
        ImGui::SetCursorPosX(CURSOR_X_LABELS);
        ImGui::AlignTextToFramePadding();
        ImGui::Text(label);

        if (tooltip.compare("") != 0)
        {
            if (ImGui::IsItemHovered())
                Tooltip(tooltip.c_str());
        }

        if (sameLine)
        {
            ImGui::SameLine();
            ImGui::SetCursorPosX(CURSOR_X_VALUES);

            // const ImVec2 p1 = ImVec2(ImGui::GetCursorScreenPos().x - 8, ImGui::GetCursorScreenPos().y - ImGui::GetFrameHeight());
            // const ImVec2 p2 = ImVec2(ImGui::GetCursorScreenPos().x - 8, ImGui::GetCursorScreenPos().y + ImGui::GetFrameHeight());
            // ImGui::GetWindowDrawList()->AddLine(p1, p2, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_PopupBg)), 2.0f);
        }
    }

    bool WidgetsUtility::BeginComboBox(const char* comboID, const char* label, bool hasRemoveButton, float widthDecrease)
    {
        PushPopupStyle();
        const float currentCursorX = ImGui::GetCursorPosX();
        const float currentCursorY = ImGui::GetCursorPosY();
        const float windowWidth    = ImGui::GetWindowWidth();
        const float remaining      = windowWidth - currentCursorX;
        const float comboWidth     = remaining - VALUE_OFFSET_FROM_WINDOW - widthDecrease - (hasRemoveButton ? ImGui::GetFrameHeight() : 0.0f);
        ImGui::SetNextItemWidth(comboWidth);
        const bool combo = ImGui::BeginCombo(comboID, label, ImGuiComboFlags_NoArrowButton);
        PopPopupStyle();

        const ImVec2 iconPos = ImVec2(ImGui::GetWindowPos().x + currentCursorX + comboWidth - 22, ImGui::GetWindowPos().y + currentCursorY + -ImGui::GetScrollY() + 3.0f);
        PushIconFontSmall();
        ImGui::GetWindowDrawList()->AddText(iconPos, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_Text)), ICON_FA_CHEVRON_CIRCLE_DOWN);
        ImGui::PopFont();

        return combo;
    }

    bool Editor::WidgetsUtility::PostComboBox(const char* id)
    {
        ImGuiStyle& style = ImGui::GetStyle();
        ImGui::SameLine(0.0f, 0.0f);
        float             frameHeight = ImGui::GetFrameHeight() - 2;
        const std::string btnLabel    = std::string(ICON_FA_MINUS) + "##" + std::string(id);
        bool              button      = Button(btnLabel.c_str(), ImVec2(frameHeight, frameHeight), 0.7f, 2.0f, ImVec2(0.1f, 0.0f));

        if (ImGui::IsItemHovered())
            Tooltip("Remove");

        return button;
    }

    void WidgetsUtility::VerticalResizeDivider(bool canResize, float* pressedPos, float* leftPaneWidth, float leftPaneMinWidth, float leftPaneMaxWidth, bool* lockWindowPos, bool* dragging)
    {
        const ImVec2 windowPos     = ImGui::GetWindowPos();
        const ImVec2 windowSize    = ImGui::GetWindowSize();
        const ImVec2 borderLineMin = ImVec2(windowPos.x + *leftPaneWidth, windowPos.y - 1);
        const ImVec2 borderLineMax = ImVec2(borderLineMin.x, borderLineMin.y + windowSize.y);
        ImGui::GetWindowDrawList()->AddLine(borderLineMin, borderLineMax, ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f)), 1.5f);

        const ImVec2 hoverRectMin = ImVec2(borderLineMin.x - 10, borderLineMin.y);
        const ImVec2 hoverRectMax = ImVec2(borderLineMin.x + 10, borderLineMax.y);

        if (canResize)
        {
            bool canDrag = false;

            if (ImGui::IsMouseHoveringRect(hoverRectMin, hoverRectMax))
            {
                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
                canDrag        = true;
                *lockWindowPos = true;
            }
            else
                *lockWindowPos = false;

            if (canDrag)
            {

                if (Input::InputEngineBackend::Get()->GetMouseButtonDown(LINA_MOUSE_1))
                {
                    *pressedPos = *leftPaneWidth;
                    *dragging   = true;
                }
            }
        }
        else
            *lockWindowPos = false;

        if (*dragging && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
        {

            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
            const float delta  = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left).x;
            float       newPos = *pressedPos + delta;

            if (newPos < leftPaneMinWidth)
                newPos = leftPaneMinWidth;
            else if (newPos > leftPaneMaxWidth)
                newPos = leftPaneMaxWidth;

            *leftPaneWidth = newPos;
        }

        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
            *dragging = false;
    }

    int Editor::WidgetsUtility::SimulationTypeComboBox(const char* comboID, int currentShapeID)
    {
        int simTypeToReturn = currentShapeID;

        if (BeginComboBox(comboID, Physics::SIMULATION_TYPES[currentShapeID].c_str(), false))
        {
            int counter = 0;
            for (auto& shape : Physics::SIMULATION_TYPES)
            {
                const bool selected = currentShapeID == counter;

                if (ImGui::Selectable(Physics::SIMULATION_TYPES[counter].c_str(), selected))
                    simTypeToReturn = counter;

                if (selected)
                    ImGui::SetItemDefaultFocus();

                counter++;
            }

            ImGui::EndCombo();
        }

        return simTypeToReturn;
    }

    int Editor::WidgetsUtility::CollisionShapeComboBox(const char* comboID, int currentShapeID)
    {
        int shapeToReturn = currentShapeID;

        if (BeginComboBox(comboID, Physics::COLLISION_SHAPES[currentShapeID].c_str(), false))
        {
            int counter = 0;
            for (auto& shape : Physics::COLLISION_SHAPES)
            {
                const bool selected = currentShapeID == counter;

                if (ImGui::Selectable(Physics::COLLISION_SHAPES[counter].c_str(), selected))
                    shapeToReturn = counter;

                if (selected)
                    ImGui::SetItemDefaultFocus();

                counter++;
            }

            ImGui::EndCombo();
        }

        return shapeToReturn;
    }

    int WidgetsUtility::SurfaceTypeComboBox(const char* comboID, int currentType)
    {
        int typeToReturn = currentType;

        if (BeginComboBox(comboID, Graphics::g_materialSurfaceTypeStr[currentType], false))
        {
            int counter = 0;
            for (auto& type : Graphics::g_materialSurfaceTypeStr)
            {
                const bool selected = currentType == counter;

                if (ImGui::Selectable(Graphics::g_materialSurfaceTypeStr[counter], selected))
                    typeToReturn = counter;

                if (selected)
                    ImGui::SetItemDefaultFocus();

                counter++;
            }

            ImGui::EndCombo();
        }

        return typeToReturn;
    }

    int WidgetsUtility::WorkflowComboBox(const char* comboID, int currentType)
    {
        int typeToReturn = currentType;

        if (BeginComboBox(comboID, Graphics::g_materialWorkflowTypeStr[currentType], false))
        {
            int counter = 0;
            for (auto& type : Graphics::g_materialWorkflowTypeStr)
            {
                const bool selected = currentType == counter;

                if (ImGui::Selectable(Graphics::g_materialWorkflowTypeStr[counter], selected))
                    typeToReturn = counter;

                if (selected)
                    ImGui::SetItemDefaultFocus();

                counter++;
            }

            ImGui::EndCombo();
        }

        return typeToReturn;
    }

    StringIDType WidgetsUtility::ResourceSelection(const std::string& id, void* currentResource, void* currentHandle, const char* resourceStr, bool* removed, TypeID resourceType)
    {
        std::string     resourceName        = "None";
        constexpr float spaceFromEnd        = 10.0f;
        const float     removeButtonSize    = ImGui::GetFrameHeight();
        const float     buttonOffsetFromEnd = (ImGui::GetWindowWidth() - ImGui::GetCursorPos().x) - spaceFromEnd - removeButtonSize - ImGui::GetStyle().ItemSpacing.x;

        if (currentResource != nullptr)
            resourceName = Utility::GetFileWithoutExtension(Utility::GetFileNameOnly(((Resources::IResource*)currentResource)->GetPath()));

        // Selection button.
        ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_FrameBg));
        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));
        const ImVec2 currentCursor = ImGui::GetCursorScreenPos();
        bool         pressed       = ImGui::Button(resourceName.c_str(), ImVec2(buttonOffsetFromEnd, removeButtonSize));
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();

        const float  iconOffset = -spaceFromEnd - removeButtonSize - 28.0f;
        const ImVec2 iconPos    = ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowWidth() + iconOffset, currentCursor.y + -ImGui::GetScrollY() + 3.0f);
        PushIconFontSmall();
        ImGui::GetWindowDrawList()->AddText(iconPos, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_Text)), ICON_FA_DOT_CIRCLE);
        ImGui::PopFont();

        // Remove button.
        ImGui::SameLine();
        PushIconFontSmall();
        *removed = Button(ICON_FA_MINUS, ImVec2(removeButtonSize, removeButtonSize), 1, 1.5f, ImVec2(0.6f, -2.4f));
        ImGui::PopFont();

        auto& resSelector = GUILayer::Get()->GetResourceSelector();

        if (pressed)
        {
            resSelector.SetCurrentTypeID(resourceType, resourceStr, id);
            resSelector.Open();
        }

        const StringIDType selectedResource = GUILayer::Get()->GetResourceSelector().m_selectedResource;

        if (resSelector.m_currentSelectorID.compare(id) == 0 && selectedResource != 0)
        {
            resSelector.m_selectedResource  = 0;
            resSelector.m_currentSelectorID = "";
            return selectedResource;
        }

        return 0;
    }

    StringIDType WidgetsUtility::ResourceSelectionMaterial(const std::string& id, void* handleAddr)
    {
        Resources::ResourceHandle<Graphics::Material>* handle = static_cast<Resources::ResourceHandle<Graphics::Material>*>(handleAddr);

        bool         pressed  = false;
        bool         removed  = false;
        StringIDType selected = ResourceSelection(id, static_cast<void*>(handle->m_value), static_cast<void*>(handle), "Material", &removed, GetTypeID<Graphics::Material>());

        if (selected != 0)
        {
            handle->m_sid   = selected;
            handle->m_value = Resources::ResourceStorage::Get()->GetResource<Graphics::Material>(selected);
        }

        if (removed)
        {
            handle->m_sid   = 0;
            handle->m_value = nullptr;
        }

        return selected;
    }

    StringIDType WidgetsUtility::ResourceSelectionTexture(const std::string& id, void* handleAddr)
    {
        Resources::ResourceHandle<Graphics::Texture>* handle = static_cast<Resources::ResourceHandle<Graphics::Texture>*>(handleAddr);

        bool         pressed  = false;
        bool         removed  = false;
        StringIDType selected = ResourceSelection(id, static_cast<void*>(handle->m_value), static_cast<void*>(handle), "Texture", &removed, GetTypeID<Graphics::Texture>());

        if (selected != 0)
        {
            handle->m_sid   = selected;
            handle->m_value = Resources::ResourceStorage::Get()->GetResource<Graphics::Texture>(selected);
            LINA_TRACE("Selected : {0}", handle->m_value->GetPath());
        }

        if (removed)
        {
            handle->m_sid   = 0;
            handle->m_value = nullptr;
        }

        return selected;
    }

    StringIDType WidgetsUtility::ResourceSelectionAudio(const std::string& id, void* handleAddr)
    {
        Resources::ResourceHandle<Audio::Audio>* handle = static_cast<Resources::ResourceHandle<Audio::Audio>*>(handleAddr);

        bool         pressed  = false;
        bool         removed  = false;
        StringIDType selected = ResourceSelection(id, static_cast<void*>(handle->m_value), static_cast<void*>(handle), "Audio", &removed, GetTypeID<Audio::Audio>());

        if (selected != 0)
        {
            handle->m_sid   = selected;
            handle->m_value = Resources::ResourceStorage::Get()->GetResource<Audio::Audio>(selected);
        }

        if (removed)
        {
            handle->m_sid   = 0;
            handle->m_value = nullptr;
        }

        return selected;
    }

    StringIDType WidgetsUtility::ResourceSelectionPhysicsMaterial(const std::string& id, void* handleAddr)
    {
        Resources::ResourceHandle<Physics::PhysicsMaterial>* handle = static_cast<Resources::ResourceHandle<Physics::PhysicsMaterial>*>(handleAddr);

        bool         pressed  = false;
        bool         removed  = false;
        StringIDType selected = ResourceSelection(id, static_cast<void*>(handle->m_value), static_cast<void*>(handle), "Physics Material", &removed, GetTypeID<Physics::PhysicsMaterial>());

        if (selected != 0)
        {
            handle->m_sid   = selected;
            handle->m_value = Resources::ResourceStorage::Get()->GetResource<Physics::PhysicsMaterial>(selected);
        }

        if (removed)
        {
            auto* defaultMat = Physics::PhysicsEngineBackend::Get()->GetDefaultPhysicsMaterial();
            handle->m_sid    = defaultMat->GetSID();
            handle->m_value  = defaultMat;
        }

        return selected;
    }

    StringIDType WidgetsUtility::ResourceSelectionShader(const std::string& id, void* handleAddr)
    {
        Resources::ResourceHandle<Graphics::Shader>* handle = static_cast<Resources::ResourceHandle<Graphics::Shader>*>(handleAddr);

        bool         pressed  = false;
        bool         removed  = false;
        StringIDType selected = ResourceSelection(id, static_cast<void*>(handle->m_value), static_cast<void*>(handle), "Shader", &removed, GetTypeID<Graphics::Shader>());

        if (selected != 0)
        {
            handle->m_sid   = selected;
            handle->m_value = Resources::ResourceStorage::Get()->GetResource<Graphics::Shader>(selected);
        }

        if (removed)
        {
            handle->m_sid   = 0;
            handle->m_value = nullptr;
        }

        return selected;
    }

    bool WidgetsUtility::Button(const char* label, const ImVec2& size, float textSize, float rounding, ImVec2 contentOffset, bool locked)
    {
        FrameRounding(rounding);

        if (textSize != 1.0f)
            WidgetsUtility::PushScaledFont(textSize);

        if (locked)
            ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonLocked));

        bool button = false;
        if (contentOffset.x == 0.0f && contentOffset.y == 0.0f)
        {
            button = ImGui::Button(label, size);
        }
        else
        {
            const ImVec2      currentCursor = ImGui::GetCursorScreenPos();
            const std::string finalLabel    = "##_" + std::string(label);
            button                          = ImGui::Button(finalLabel.c_str(), size);
            std::string  labelStr           = std::string(label);
            std::string  text               = labelStr.substr(0, labelStr.find("#"));
            const ImVec2 calcSize           = ImGui::CalcTextSize(text.c_str());
            const ImVec2 textPos            = ImVec2(currentCursor.x + size.x / 2.0f - calcSize.x / 2.0f + contentOffset.x, currentCursor.y + size.y / 2.0f - calcSize.y / 2.0f + contentOffset.y);
            ImGui::GetWindowDrawList()->AddText(textPos, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_Text)), text.c_str());
        }

        if (locked)
            ImGui::PopStyleColor();

        ImGui::SetItemAllowOverlap();
        ImGui::PopStyleVar();

        if (textSize != 1.0f)
            WidgetsUtility::PopScaledFont();
        return button;
    }

    bool WidgetsUtility::IconButton(const char* icon, bool useSmallIcon)
    {
        const float  frameHeight = ImGui::GetFrameHeight() - 5;
        const ImVec2 size        = ImVec2(frameHeight, frameHeight);
        const ImVec2 rectMin     = ImGui::GetCursorScreenPos();
        const ImVec2 rectMax     = ImVec2(rectMin.x + size.x, rectMin.y + size.y);
        const bool   hovered     = ImGui::IsMouseHoveringRect(rectMin, rectMax);
        const bool   pressed     = hovered && Input::InputEngineBackend::Get()->GetMouseButton(LINA_MOUSE_1);

        const ImVec4 iconColor = pressed ? ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive) : hovered ? ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered)
                                                                                                     : ImGui::GetStyleColorVec4(ImGuiCol_Text);

        ImGui::PushStyleColor(ImGuiCol_Text, iconColor);
        IconSmall(icon);
        ImGui::PopStyleColor();
        return hovered && Input::InputEngineBackend::Get()->GetMouseButtonUp(LINA_MOUSE_1);
    }

    bool WidgetsUtility::TreeNode(const void* id, ImGuiTreeNodeFlags flags, const char* name, bool drawArrow)
    {
        FramePaddingX(4);
        const ImVec2 iconMin = ImVec2(ImGui::GetCursorScreenPos().x + 10, ImGui::GetCursorScreenPos().y);
        const bool   node    = ImGui::TreeNodeEx(id, flags, name);

        if (drawArrow)
        {
            PushIconFontSmall();
            const char* arrow = node ? ICON_FA_CARET_DOWN : ICON_FA_CARET_RIGHT;
            ImGui::GetWindowDrawList()->AddText(iconMin, ImGui::GetColorU32(ImGui::GetStyleColorVec4(ImGuiCol_Text)), arrow);
            ImGui::PopFont();
        }
        ImGui::PopStyleVar();
        return node;
    }

    void Editor::WidgetsUtility::DragBehaviour(const char* id, float* var, ImRect rect)
    {

        if (ImGui::IsMouseHoveringRect(rect.Min, rect.Max))
        {
            if (!s_isDraggingWidgetInput)
            {
                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
            }

            if (Input::InputEngineBackend::Get()->GetMouseButtonDown(0))
            {
                s_isDraggingWidgetInput = true;
                s_draggedInput          = id;
                s_valueOnDragStart      = *var;
            }
        }

        if (s_isDraggingWidgetInput)
            ImGui::SetMouseCursor(ImGuiMouseCursor_None);

        if (s_isDraggingWidgetInput && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        {
            s_isDraggingWidgetInput = false;
            s_draggedInput          = "";
        }

        if (s_isDraggingWidgetInput && id == s_draggedInput)
        {
            const float dragDelta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left).x;
            *var                  = s_valueOnDragStart + dragDelta * DRAG_POWER;
        }
    }

    void Editor::WidgetsUtility::DragBehaviour(const char* id, int* var)
    {

        if (ImGui::IsItemHovered())
        {
            if (!s_isDraggingWidgetInput)
            {
                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
            }

            if (Input::InputEngineBackend::Get()->GetMouseButtonDown(0))
            {
                s_isDraggingWidgetInput = true;
                s_draggedInput          = id;
                s_valueOnDragStartInt   = *var;
            }

            if (s_isDraggingWidgetInput && s_draggedInput != id)
            {
                s_isDraggingWidgetInput = false;
                s_draggedInput          = "";
            }
        }

        if (s_isDraggingWidgetInput)
            ImGui::SetMouseCursor(ImGuiMouseCursor_None);

        if (s_isDraggingWidgetInput && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        {
            s_isDraggingWidgetInput = false;
            s_draggedInput          = "";
        }

        if (s_isDraggingWidgetInput && id == s_draggedInput)
        {
            const int dragDelta = (int)ImGui::GetMouseDragDelta(ImGuiMouseButton_Left).x;
            *var                = (int)(s_valueOnDragStartInt + dragDelta * DRAG_POWER);
        }
    }

    bool Editor::WidgetsUtility::DragFloat(const char* id, const char* label, float* var, float width)
    {
        bool isIcon = label == nullptr;

        if (isIcon)
            label = ICON_FA_ARROWS_ALT_H;

        if (width == -1.0f)
        {
            float windowWidth   = ImGui::GetWindowWidth();
            float currentCursor = ImGui::GetCursorPosX();
            float remaining     = (windowWidth - currentCursor);
            float comboWidth    = remaining - VALUE_OFFSET_FROM_WINDOW;
            ImGui::SetNextItemWidth(comboWidth);
        }
        else
            ImGui::SetNextItemWidth(width);

        const float       itemHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
        ImVec2            windowPos  = ImGui::GetWindowPos();
        ImVec2            cursorPos  = ImGui::GetCursorPos();
        ImVec2            rectMin    = ImVec2(windowPos.x + cursorPos.x + 1, windowPos.y + -ImGui::GetScrollY() + cursorPos.y + 1);
        ImVec2            rectMax    = ImVec2(rectMin.x + itemHeight - 2, rectMin.y + itemHeight - 2);
        ImVec2            rectSize   = ImVec2(rectMax.x - rectMin.x, rectMax.y - rectMin.y);
        ImVec4            rectCol    = ImGui::GetStyleColorVec4(ImGuiCol_Header);
        ImVec4            textCol    = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        const std::string rectID     = std::string(id) + "_rect";

        if (ImGui::IsMouseHoveringRect(rectMin, rectMax))
            ImGui::SetHoveredID(ImHashStr(rectID.c_str()));

        FramePaddingX(itemHeight + 6);
        bool result = ImGui::InputFloat(id, var);
        PopStyleVar();

        ImGui::GetWindowDrawList()->AddRectFilled(rectMin, rectMax, ImGui::ColorConvertFloat4ToU32(rectCol), 1);
        DragBehaviour(rectID.c_str(), var, ImRect(rectMin, rectMax));

        float yOffset = 0.0f;
        if (isIcon)
        {
            PushIconFontSmall();
            yOffset = 2.4f;
        }

        const ImVec2 textSize = ImGui::CalcTextSize(label);
        ImGui::GetWindowDrawList()->AddText(ImVec2(rectMin.x + rectSize.x / 2.0f - textSize.x / 2.0f, rectMin.y + rectSize.y / 2.0f - textSize.y / 2.0f - yOffset), ImGui::ColorConvertFloat4ToU32(textCol), label);

        if (isIcon)
            ImGui::PopFont();

        return result;
    }

    bool Editor::WidgetsUtility::DragInt(const char* id, const char* label, int* var, float width)
    {
        bool isIcon = label == nullptr;

        if (isIcon)
            label = ICON_FA_ARROWS_ALT_H;

        if (width == -1.0f)
        {
            float windowWidth   = ImGui::GetWindowWidth();
            float currentCursor = ImGui::GetCursorPosX();
            float remaining     = (windowWidth - currentCursor);
            float comboWidth    = remaining - VALUE_OFFSET_FROM_WINDOW;
            ImGui::SetNextItemWidth(comboWidth);
        }
        else
            ImGui::SetNextItemWidth(width);

        const float       itemHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
        ImVec2            windowPos  = ImGui::GetWindowPos();
        ImVec2            cursorPos  = ImGui::GetCursorPos();
        ImVec2            rectMin    = ImVec2(windowPos.x + cursorPos.x + 1, windowPos.y + -ImGui::GetScrollY() + cursorPos.y + 1);
        ImVec2            rectMax    = ImVec2(rectMin.x + itemHeight - 2, rectMin.y + itemHeight - 2);
        ImVec2            rectSize   = ImVec2(rectMax.x - rectMin.x, rectMax.y - rectMin.y);
        ImVec4            rectCol    = ImGui::GetStyleColorVec4(ImGuiCol_Header);
        ImVec4            textCol    = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        const std::string rectID     = std::string(id) + "_rect";

        if (ImGui::IsMouseHoveringRect(rectMin, rectMax))
            ImGui::SetHoveredID(ImHashStr(rectID.c_str()));

        FramePaddingX(itemHeight + 6);
        bool result = ImGui::InputInt(id, var);
        PopStyleVar();

        ImGui::GetWindowDrawList()->AddRectFilled(rectMin, rectMax, ImGui::ColorConvertFloat4ToU32(rectCol), 1);
        DragBehaviour(rectID.c_str(), var);

        float yOffset = 0.0f;
        if (isIcon)
        {
            PushIconFontSmall();
            yOffset = 2.4f;
        }

        const ImVec2 textSize = ImGui::CalcTextSize(label);
        ImGui::GetWindowDrawList()->AddText(ImVec2(rectMin.x + rectSize.x / 2.0f - textSize.x / 2.0f, rectMin.y + rectSize.y / 2.0f - textSize.y / 2.0f - yOffset), ImGui::ColorConvertFloat4ToU32(textCol), label);

        if (isIcon)
            ImGui::PopFont();

        return result;
    }

    bool Editor::WidgetsUtility::DragVector2(const char* id, float* var)
    {
        float       windowWidth    = ImGui::GetWindowWidth();
        float       currentCursor  = ImGui::GetCursorPosX();
        const float labelIncrement = 12;
        float       widthPerItem   = (windowWidth - currentCursor - VALUE_OFFSET_FROM_WINDOW - ImGui::GetStyle().ItemSpacing.x * 1.0f) / 2.0f;
        std::string xid            = std::string(id) + "_x";
        std::string yid            = std::string(id) + "_y";

        bool x = DragFloat(xid.c_str(), "X", &var[0], widthPerItem);
        ImGui::SameLine();
        bool y = DragFloat(yid.c_str(), "Y", &var[1], widthPerItem);

        return x || y;
    }

    bool Editor::WidgetsUtility::DragVector3(const char* id, float* var)
    {
        float       windowWidth    = ImGui::GetWindowWidth();
        float       currentCursor  = ImGui::GetCursorPosX();
        const float labelIncrement = 12;
        float       widthPerItem   = (windowWidth - currentCursor - VALUE_OFFSET_FROM_WINDOW - ImGui::GetStyle().ItemSpacing.x * 2.0f) / 3.0f;
        std::string xid            = std::string(id) + "_x";
        std::string yid            = std::string(id) + "_y";
        std::string zid            = std::string(id) + "_z";

        bool x = DragFloat(xid.c_str(), "X", &var[0], widthPerItem);
        ImGui::SameLine();
        bool y = DragFloat(yid.c_str(), "Y", &var[1], widthPerItem);
        ImGui::SameLine();
        bool z = DragFloat(zid.c_str(), "Z", &var[2], widthPerItem);

        return x || y || z;
    }

    bool Editor::WidgetsUtility::DragVector4(const char* id, float* var)
    {
        float       windowWidth    = ImGui::GetWindowWidth();
        float       currentCursor  = ImGui::GetCursorPosX();
        const float labelIncrement = 12;
        float       widthPerItem   = (windowWidth - currentCursor - VALUE_OFFSET_FROM_WINDOW - ImGui::GetStyle().ItemSpacing.x * 3.0f) / 4.0f;
        std::string xid            = std::string(id) + "_x";
        std::string yid            = std::string(id) + "_y";
        std::string zid            = std::string(id) + "_z";

        bool x = DragFloat(xid.c_str(), "X", &var[0], widthPerItem);
        ImGui::SameLine();
        bool y = DragFloat(yid.c_str(), "Y", &var[1], widthPerItem);
        ImGui::SameLine();
        bool z = DragFloat(zid.c_str(), "Z", &var[2], widthPerItem);
        ImGui::SameLine();
        bool w = DragFloat(zid.c_str(), "W", &var[3], widthPerItem);
        return x || y || z || w;
    }

    void WidgetsUtility::IncrementCursorPosX(float f)
    {
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + f);
    }

    void WidgetsUtility::IncrementCursorPosY(float f)
    {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + f);
    }

    void WidgetsUtility::IncrementCursorPos(const ImVec2& v)
    {
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + v.x, ImGui::GetCursorPosY() + v.y));
    }

    void WidgetsUtility::TableAlignCenter(float itemWidth)
    {
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetColumnWidth() / 2.0f - itemWidth / 2.0f);
    }

    void WidgetsUtility::CenteredText(const char* label)
    {
        float textW = ImGui::CalcTextSize(label).x;
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 - textW / 2.0f);
        ImGui::Text(label);
    }

    void WidgetsUtility::CenterCursorX()
    {
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2.0f);
    }

    void WidgetsUtility::CenterCursorY()
    {
        ImGui::SetCursorPosY(ImGui::GetWindowHeight() / 2.0f);
    }

    void WidgetsUtility::CenterCursor()
    {
        CenterCursorX();
        CenterCursorY();
    }

    void WidgetsUtility::PushScaledFont(float defaultScale)
    {
        ImGui::GetFont()->Scale = defaultScale;
        ImGui::PushFont(ImGui::GetFont());
    }

    void WidgetsUtility::PopScaledFont()
    {
        ImGui::GetFont()->Scale = 1.0f;
        ImGui::PopFont();
    }

    void WidgetsUtility::FramePaddingX(float amt)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(amt, ImGui::GetStyle().FramePadding.y));
    }

    void WidgetsUtility::FramePaddingY(float amt)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(ImGui::GetStyle().FramePadding.x, amt));
    }

    void WidgetsUtility::FramePadding(const ImVec2& amt)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, amt);
    }

    void WidgetsUtility::FrameRounding(float rounding)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, rounding);
    }

    void WidgetsUtility::WindowPaddingX(float amt)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(amt, ImGui::GetStyle().WindowPadding.y));
    }

    void WidgetsUtility::WindowPaddingY(float amt)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(ImGui::GetStyle().WindowPadding.x, amt));
    }

    void WidgetsUtility::WindowPadding(const ImVec2& amt)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, amt);
    }

    void WidgetsUtility::ItemSpacingX(float amt)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(amt, ImGui::GetStyle().ItemSpacing.y));
    }

    void WidgetsUtility::ItemSpacingY(float amt)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, amt));
    }

    void WidgetsUtility::ItemSpacing(const ImVec2& amt)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, amt);
    }

    void WidgetsUtility::WindowRounding(float rounding)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, rounding);
    }

    void WidgetsUtility::PushPopupStyle()
    {
        ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0.303f, 0.303f, 0.303f, 1.000f));
        const Vector2 padding = GUILayer::Get()->GetDefaultWindowPadding();
        ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, 1.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(padding.x, padding.y));
        ImGui::PushStyleColor(ImGuiCol_Border, ImGui::GetStyleColorVec4(ImGuiCol_PopupBorder));
    }

    void WidgetsUtility::PopPopupStyle()
    {
        ImGui::PopStyleVar();
        ImGui::PopStyleVar();
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
    }

    void WidgetsUtility::PopStyleVar()
    {
        ImGui::PopStyleVar();
    }

    void WidgetsUtility::Icon(const char* label, bool align, float scale)
    {
        PushScaledFont(scale);

        if (align)
        {
            ImVec2 cursorPos = ImGui::GetCursorPos();
            ImVec2 textSize  = ImGui::CalcTextSize(label);
            ImGui::SetCursorPos(ImVec2(cursorPos.x - textSize.x / 2.0f, cursorPos.y - textSize.x / 2.0f));
        }
        ImGui::Text(label);
        PopScaledFont();
    }

    void WidgetsUtility::IconSmall(const char* icon)
    {
        PushIconFontSmall();
        ImGui::Text(icon);
        ImGui::PopFont();
    }

    void WidgetsUtility::IconDefault(const char* icon)
    {
        PushIconFontDefault();
        ImGui::Text(icon);
        ImGui::PopFont();
    }

    void WidgetsUtility::PushIconFontDefault()
    {
        ImGui::PushFont(GUILayer::Get()->GetIconFontDefault());
    }

    void WidgetsUtility::PushIconFontSmall()
    {
        ImGui::PushFont(GUILayer::Get()->GetIconFontSmall());
    }

    ImVec2 WidgetsUtility::GetWindowPosWithContentRegion()
    {
        return ImVec2(ImGui::GetWindowContentRegionMin().x + ImGui::GetWindowPos().x, ImGui::GetWindowContentRegionMin().y + ImGui::GetWindowPos().y);
    }

    ImVec2 WidgetsUtility::GetWindowSizeWithContentRegion()
    {
        float yDiff = ImGui::GetWindowPos().y - WidgetsUtility::GetWindowPosWithContentRegion().y;
        return ImVec2(ImGui::GetWindowContentRegionWidth(), ImGui::GetWindowSize().y + yDiff);
    }

} // namespace Lina::Editor
