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
#include "Utility/UtilityFunctions.hpp"
#include "Widgets/MenuButton.hpp"
#include "Memory/Memory.hpp"
#include "Core/ImGuiCommon.hpp"
#include "Widgets/Snackbar.hpp"

namespace Lina::Editor
{

    static bool        s_isDraggingWidgetInput = false;
    static bool        s_mouseReleased         = true;
    static std::string s_draggedInput          = "";
    static float       s_valueOnDragStart      = 0.0f;
    static int         s_valueOnDragStartInt   = 0;
    void*              s_latestResourceHandle  = nullptr;

    void WidgetsUtility::Tooltip(const char* tooltip)
    {
        if (s_isDraggingWidgetInput)
            return;

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        ImGui::BeginTooltip();
        ImGui::Text(tooltip);
        ImGui::EndTooltip();
        ImGui::PopStyleColor();
    }

    bool WidgetsUtility::CustomToggle(const char* id, ImVec2 size, bool toggled, bool* hoveredPtr, const char* icon, float rounding, const char* tooltip)
    {
        const ImVec2 currentCursor = ImGui::GetCursorPos();
        const ImVec2 currentPos    = ImVec2(ImGui::GetWindowPos().x + ImGui::GetCursorPosX(), ImGui::GetWindowPos().y + ImGui::GetCursorPos().y);
        const ImRect absoluteRect  = ImRect(ImVec2(currentPos.x, currentPos.y), ImVec2(size.x + currentPos.x, size.y + currentPos.y));

        ImGui::ItemAdd(absoluteRect, ImHashStr(id));
        ImGui::ItemSize(size);

        bool       hovered  = ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(absoluteRect.Min, absoluteRect.Max) && !ImGui::IsAnyItemHovered();
        const bool pressing = hovered && ImGui::IsMouseDown(ImGuiMouseButton_Left);

        if (hoveredPtr != nullptr)
            *hoveredPtr = hovered;
        const ImVec4 normalColor = ImGui::GetStyleColorVec4(ImGuiCol_Button);
        const ImVec4 lockedColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonLocked);
        const ImVec4 hoverColor  = ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered);
        const ImVec4 rectCol     = pressing ? normalColor : (hovered ? hoverColor : toggled ? lockedColor
                                                                                            : normalColor);
        const ImU32  borderColor = ImGui::ColorConvertFloat4ToU32(ImVec4(0.3f, 0.3f, 0.3f, 0.55f));
        ImGui::GetWindowDrawList()->AddRectFilled(absoluteRect.Min, absoluteRect.Max, ImGui::ColorConvertFloat4ToU32(rectCol), rounding);
        IncrementCursorPosY(size.y / 2.0f);

        if (icon != nullptr)
        {
            const float iconScale = size.y * 0.035f;
            PushScaledFont(iconScale);
            ImVec2 textSize = ImGui::CalcTextSize(icon);

            ImGui::SameLine();
            ImGui::SetCursorPosX(currentCursor.x + size.x / 2.0f - textSize.x / 2.0f);
            ImGui::SetCursorPosY(currentCursor.y + size.y / 2.0f - textSize.y / 2.0f);
            ImGui::Text(icon);
            PopScaledFont();
        }

        if (tooltip != nullptr && hovered)
            Tooltip(tooltip);

        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && hovered)
            return true;

        return false;
    }

    bool WidgetsUtility::ColorsEqual(ImVec4 col1, ImVec4 col2)
    {
        return (col1.x == col2.x && col1.y == col2.y && col1.z == col2.z && col1.w == col2.w);
    }

    bool WidgetsUtility::DrawTreeFolder(Utility::Folder* folder, Utility::Folder*& selectedFolder)
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

            if (Input::InputEngineBackend::Get()->GetKeyDown(LINA_KEY_F2))
            {
                if (folder->m_fullPath.compare("Resources/") == 0 || folder->m_fullPath.find("Resources/Engine") != std::string::npos ||
                folder->m_fullPath.compare("Resources/Sandbox") == 0 || folder->m_fullPath.find("Resources/Editor") != std::string::npos)
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

        if (folder->m_isRenaming)
        {
            IncrementCursorPosX(28);

            const std::string inputLabel = "##_A_" + folder->m_name;
            if (ImGui::InputText(inputLabel.c_str(), str0, IM_ARRAYSIZE(str0), ImGuiInputTextFlags_EnterReturnsTrue))
            {
                folder->m_isRenaming = false;
                Utility::ChangeFolderName(folder, str0);
            }
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

    void WidgetsUtility::DrawResourceNode(TypeID tid, bool selected, const std::string& fullPath, float startCursorX, float sizeMultiplier)
    {
#pragma warning(disable : 4312)
        auto*        storage            = Resources::ResourceStorage::Get();
        const ImVec2 cursorPosScreen    = ImGui::GetCursorScreenPos();
        const ImVec2 cursorPos          = ImGui::GetCursorPos();
        const float  fileTypeRectHeight = 5;
        const ImVec2 imageSize          = ImVec2(70, 70);
        const ImVec2 totalSize          = ImVec2(imageSize.x + 30, 40 + imageSize.y);
        const float  windowWidth        = ImGui::GetWindowWidth();
        const ImVec2 itemRectMin        = ImVec2(cursorPosScreen.x, cursorPosScreen.y);
        const ImVec2 itemRectMax        = ImVec2(cursorPosScreen.x + totalSize.x, cursorPosScreen.y + totalSize.y);
        const bool   hovered            = ImGui::IsMouseHoveringRect(itemRectMin, itemRectMax);
        ImVec4       childBG            = selected ? ImGui::GetStyleColorVec4(ImGuiCol_FolderActive) : hovered ? ImGui::GetStyleColorVec4(ImGuiCol_FolderHovered)
                                                                                                               : ImGui::GetStyleColorVec4(ImGuiCol_ChildBg);
        if (selected)
            LINA_TRACE("Selected {0}", fullPath);

        ImGui::BeginChild(fullPath.c_str(), totalSize, false, ImGuiWindowFlags_NoScrollbar);

        // Background.
        ImGui::GetWindowDrawList()->AddRectFilled(cursorPosScreen, ImVec2(cursorPosScreen.x + totalSize.x, cursorPosScreen.y + totalSize.y), ImGui::ColorConvertFloat4ToU32(childBG), 2.0f);

        uint32 textureID = 0;

        if (tid == -1)
            textureID = storage->GetResource<Graphics::Texture>("Resources/Editor/Textures/Folder.png")->GetID();
        else
        {
            if (tid == GetTypeID<Graphics::Texture>())
                textureID = storage->GetResource<Graphics::Texture>(fullPath)->GetID();
        }

        // Prepare border sizes from incremented cursor.
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2.0f - imageSize.x / 2.0f);
        WidgetsUtility::IncrementCursorPosY(5);
        const ImVec2 borderMin = ImGui::GetCursorScreenPos();
        const ImVec2 borderMax = ImVec2(borderMin.x + imageSize.x, borderMin.y + imageSize.y);

        // Add checkered background to cover transparents.
        if (tid != -1)
            ImGui::GetWindowDrawList()->AddImage((void*)storage->GetResource<Graphics::Texture>("Resources/Editor/Textures/Checkered.png")->GetID(), borderMin, borderMax, ImVec2(0, 1), ImVec2(1, 0));

        // Add the actual resource image.
        ImGui::Image((void*)textureID, imageSize, ImVec2(0, 1), ImVec2(1, 0));

        // Draw a small colored rect indicating resource type.
        if (tid != -1)
        {
            const ImVec2 extRectMin   = ImVec2(borderMin.x, borderMax.y - fileTypeRectHeight);
            const ImVec2 extRectMax   = ImVec2(extRectMin.x + imageSize.x, extRectMin.y + fileTypeRectHeight);
            const Color  extColorLina = storage->GetTypeColor(tid);
            const ImVec4 extColor     = ImVec4(extColorLina.r, extColorLina.g, extColorLina.b, extColorLina.a);
            ImGui::GetWindowDrawList()->AddRectFilled(extRectMin, extRectMax, ImGui::ColorConvertFloat4ToU32(extColor), 2.0f, ImDrawFlags_RoundCornersTop);
        }

        // Add image border afterwards, should overlay the image.
        if (tid != -1)
            ImGui::GetWindowDrawList()->AddRect(borderMin, borderMax, ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f)), 0.0f, 0, 1.5f);

        const std::string name           = Utility::GetFileWithoutExtension(Utility::GetFileNameOnly(fullPath));
        const float       textSize       = ImGui::CalcTextSize(name.c_str()).x;
        const bool        shouldWrapText = textSize >= imageSize.x + 3;
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2.0f - textSize / 2.0f);

        if (shouldWrapText)
            ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + totalSize.x);

        ImGui::Text(name.c_str());

        if (shouldWrapText)
            ImGui::PopTextWrapPos();

        ImGui::EndChild();

        if (cursorPos.x + (totalSize.x + ImGui::GetStyle().ItemSpacing.x) * 2.0f < windowWidth)
            ImGui::SameLine();
        else
            ImGui::SetCursorPosX(startCursorX);
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

    void WidgetsUtility::HorizontalDivider(float yOffset, float thickness, float maxOverride)
    {
        const float maxWidth = maxOverride == 0.0f ? ImGui::GetWindowPos().x + ImGui::GetWindowWidth() : ImGui::GetWindowPos().x + maxOverride;
        ImVec2      min      = ImVec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + ImGui::GetCursorPosY() + yOffset);
        ImVec2      max      = ImVec2(maxWidth, min.y);
        ImGui::GetWindowDrawList()->AddLine(min, max, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_Text)), thickness);
    }

    void WidgetsUtility::WindowButtons(const char* windowID, float yOffset, bool isAppWindow)
    {

        ImVec2 windowPos   = ImGui::GetWindowPos();
        float  windowWidth = ImGui::GetWindowWidth();

        const float  offset1                   = 35.0f;
        const float  gap                       = 25.0f;
        const ImVec2 buttonSize                = ImVec2(25, 20);
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

    void WidgetsUtility::WindowTitlebar(const char* label)
    {
        if (ImGui::IsWindowDocked())
            return;

        const ImVec2 windowPos  = ImGui::GetWindowPos();
        const ImVec2 windowSize = ImGui::GetWindowSize();
        const float  height     = 30.0f;
        const float  lineOffset = 2.0f;

        ImRect titleRect      = ImRect(ImVec2(windowPos.x, windowPos.y), ImVec2(windowPos.x + windowSize.x, windowPos.y + height));
        ImVec4 titleRectColor = ImGui::GetStyleColorVec4(ImGuiCol_TitleBg);
        ImGui::GetWindowDrawList()->AddRectFilled(titleRect.Min, titleRect.Max, ImGui::ColorConvertFloat4ToU32(titleRectColor));

        // Draw title
        ImGui::SetCursorPosX(12.5f);
        IncrementCursorPosY(4);
        IconSmall(GUILayer::Get()->m_windowIconMap[label]);
        ImGui::SameLine();
        ImGui::Text(label);

        // Draw Buttons
        WindowButtons(label, 5.0f);
    }

    bool WidgetsUtility::ComponentHeader(TypeID tid, bool* foldoutOpen, const char* componentLabel, const char* componentIcon, bool* toggled, bool* removed, bool* copied, bool* pasted, bool* resetted, bool moveButton, bool disableHeader)
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

        static float w = 2.0f;

        w += Input::InputEngineBackend::Get()->GetVerticalAxisValue() * 0.1f;

        IncrementCursorPosY(2.0f);
        ImGui::SetCursorPosX(CURSOR_X_LABELS);
        Icon(*foldoutOpen ? ICON_FA_CARET_DOWN : ICON_FA_CARET_RIGHT, false, 1.0f);
        ImGui::SameLine();
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

        // Draw component icon.
        if (componentIcon != nullptr)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_Header));
            IncrementCursorPosY(10.2f);
            IncrementCursorPosX(ImGui::GetStyle().ItemSpacing.x);
            Icon(componentIcon, true, 0.7f);
            ImGui::PopStyleColor();
            ImGui::SameLine();
        }

        ImGui::SetCursorPosX(cursorPos.x);
        ImGui::SetCursorPosY(cursorPos.y + 2.6f);
        const float cursorPosX = ImGui::GetWindowWidth() - VALUE_OFFSET_FROM_WINDOW;

        std::vector<std::pair<const char*, bool*>> buttons;
        std::vector<std::string>                   buttonNames{"Remove", "Paste", "Copy", "Reset"};
        buttons.push_back(std::make_pair(ICON_FA_TIMES, removed));
        buttons.push_back(std::make_pair(ICON_FA_PASTE, pasted));
        buttons.push_back(std::make_pair(ICON_FA_COPY, copied));
        buttons.push_back(std::make_pair(ICON_FA_SYNC_ALT, resetted));

        float       lastCursorX      = 0.0f;
        const float buttonWidth      = 26.0f;
        const float buttonHeight     = 20.0f;
        bool        anyButtonHovered = false;

        for (int i = 0; i < buttons.size(); i++)
        {
            lastCursorX = cursorPosX - buttonWidth * (i + 1) - (ImGui::GetStyle().ItemSpacing.x * i);
            ImGui::SetCursorPosX(lastCursorX);

            if (buttons[i].second == nullptr)
                ImGui::BeginDisabled();

            const std::string btnLbl = std::string(buttons[i].first) + "##_" + componentLabel;
            if (Button(btnLbl.c_str(), ImVec2(buttonWidth, buttonHeight), 0.6f, 2.0f))
                *buttons[i].second = !*buttons[i].second;

            if (ImGui::IsItemHovered())
            {
                Tooltip(buttonNames[i].c_str());
                anyButtonHovered = true;
            }

            if (buttons[i].second == nullptr)
                ImGui::EndDisabled();

            ImGui::SameLine();
        }

        const ImVec2 toggleSize = DEFAULT_TOGGLE_SIZE;

        if (toggled != nullptr)
        {
            // Toggle
            const std::string toggleID = "##_toggle_" + std::string(componentLabel);
            ImGui::SetCursorPosX(lastCursorX - toggleSize.x - ImGui::GetStyle().ItemSpacing.x);
            ImGui::SetCursorPosY(cursorPos.y + 4.4f);
            ToggleButton(toggleID.c_str(), toggled);
            if (ImGui::IsItemHovered())
                anyButtonHovered = true;

            ImGui::SameLine();
            lastCursorX = ImGui::GetCursorPosX();
        }

        if (bgReleased && !anyButtonHovered)
            *foldoutOpen = !*foldoutOpen;

        ImGui::SetCursorPos(cursorPos);
        IncrementCursorPosY(rectSize.y + ImGui::GetStyle().ItemSpacing.y);
        return *foldoutOpen;
    }

    bool WidgetsUtility::Header(const char* label, bool* foldoutOpen, ImVec2* outCursorPos)
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
            *foldoutOpen = !*foldoutOpen;

        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();

        const ImVec2 cursorPosAfterButton = ImGui::GetCursorPos();
        const ImVec2 textSize             = ImGui::CalcTextSize(label);
        const ImVec2 cursorPosInside      = ImVec2(cursorPosBeforeButton.x + VALUE_OFFSET_FROM_WINDOW, cursorPosBeforeButton.y + rectSize.y / 2.0f - textSize.y / 2.0f);
        ImGui::SetCursorPos(cursorPosInside);
        Icon(*foldoutOpen ? ICON_FA_CARET_DOWN : ICON_FA_CARET_RIGHT, false, 1.0f);
        ImGui::SameLine();
        ImGui::Text(label);
        ImGui::SameLine();
        ImGui::SetCursorPos(cursorPosAfterButton);

        return *foldoutOpen;
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

    bool WidgetsUtility::CaretTitle(const char* title, bool* caretOpen)
    {
        bool   clicked          = false;
        bool   hovered          = false;
        ImVec4 caretNormalColor = ImVec4(0.75f, 0.75f, 0.75f, 1.0f);
        ImVec4 caretHoverColor  = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        ImVec4 caretColor       = hovered ? caretHoverColor : caretNormalColor;

        ImGui::SetCursorPosX(CURSOR_X_LABELS);

        ImGui::PushStyleColor(ImGuiCol_Text, caretColor);
        ImGui::Text(*caretOpen ? ICON_FA_CARET_DOWN : ICON_FA_CARET_RIGHT);
        ImGui::PopStyleColor();

        if (ImGui::IsItemClicked())
            clicked = true;

        ImGui::SameLine();
        IncrementCursorPosY(0);
        ImGui::Text(title);

        if (ImGui::IsItemClicked())
            clicked = true;

        if (clicked)
            *caretOpen = !*caretOpen;

        return *caretOpen;
    }

    void WidgetsUtility::PropertyLabel(const char* label, bool sameLine)
    {
        ImGui::SetCursorPosX(CURSOR_X_LABELS);
        ImGui::AlignTextToFramePadding();
        ImGui::Text(label);

        if (sameLine)
        {
            ImGui::SameLine();
            ImGui::SetCursorPosX(CURSOR_X_VALUES);
        }
    }

    bool Editor::WidgetsUtility::BeginComboBox(const char* comboID, const char* label, bool hasRemoveButton)
    {
        PushPopupStyle();
        const float currentCursor = ImGui::GetCursorPosX();
        const float windowWidth   = ImGui::GetWindowWidth();
        const float remaining     = windowWidth - currentCursor;
        const float comboWidth    = remaining - VALUE_OFFSET_FROM_WINDOW - (hasRemoveButton ? ImGui::GetFrameHeight() : 0.0f);
        const bool  combo         = ImGui::BeginCombo(comboID, label, ImGuiComboFlags_NoArrowButton);
        PopPopupStyle();
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

    Graphics::Material* WidgetsUtility::MaterialComboBox(const char* comboID, const std::string& currentPath, bool* removed)
    {
        Graphics::Material* materialToReturn = nullptr;

        // std::string materialLabel = "";
        // if (Graphics::Material::MaterialExists(currentPath))
        // {
        //     materialLabel = Utility::GetFileWithoutExtension(Utility::GetFileNameOnly(currentPath));
        // }
        //
        // if (BeginComboBox(comboID, materialLabel.c_str(), true))
        // {
        //     auto& loadedMaterials = Graphics::Material::GetLoadedMaterials();
        //
        //     for (auto& material : loadedMaterials)
        //     {
        //         const bool selected = currentPath == material.second.GetPath();
        //
        //         std::string label = material.second.GetPath();
        //         label             = Utility::GetFileWithoutExtension(Utility::GetFileNameOnly(label));
        //         if (ImGui::Selectable(label.c_str(), selected))
        //         {
        //             materialToReturn = &material.second;
        //         }
        //
        //         if (selected)
        //             ImGui::SetItemDefaultFocus();
        //     }
        //     ImGui::EndCombo();
        // }

        const std::string comboEndID = std::string(comboID) + "_comboEnd";
        if (PostComboBox(comboEndID.c_str()) && removed != nullptr)
            *removed = true;

        return materialToReturn;
    }

    Graphics::Model* WidgetsUtility::ModelComboBox(const char* comboID, int currentModelID, bool* removed)
    {
        Graphics::Model* modelToReturn = nullptr;

        std::string modelLabel = "";
        //if (Graphics::Model::ModelExists(currentModelID))
        //{
        //    const std::string modelLabelFull = Graphics::Model::GetModel(currentModelID).GetPath();
        //    modelLabel                       = Utility::GetFileWithoutExtension(Utility::GetFileNameOnly(modelLabelFull));
        //}
        //
        //if (BeginComboBox(comboID, modelLabel.c_str(), true))
        //{
        //    auto& loadedModels = Graphics::Model::GetLoadedModels();
        //
        //    for (auto& model : loadedModels)
        //    {
        //        const bool selected = currentModelID == model.second.GetID();
        //
        //        std::string label = model.second.GetPath();
        //        label             = Utility::GetFileWithoutExtension(Utility::GetFileNameOnly(label));
        //        if (ImGui::Selectable(label.c_str(), selected))
        //        {
        //            modelToReturn = &model.second;
        //        }
        //
        //        if (selected)
        //            ImGui::SetItemDefaultFocus();
        //    }
        //
        //    ImGui::EndCombo();
        //}

        const std::string comboEndID = std::string(comboID) + "_comboEnd";
        if (PostComboBox(comboEndID.c_str()) && removed != nullptr)
            *removed = true;

        return modelToReturn;
    }

    Graphics::Shader* WidgetsUtility::ShaderComboBox(const char* comboID, int currentShaderID, bool* removed)
    {
        Graphics::Shader* shaderToReturn = nullptr;
        auto*             storage        = Resources::ResourceStorage::Get();
        auto&             shaderCache    = storage->GetCache<Graphics::Shader>();

        std::string shaderLabel = "";
        if (storage->Exists<Graphics::Shader>(currentShaderID))
        {
            const std::string shaderLabelFull = storage->GetResource<Graphics::Shader>(currentShaderID)->GetPath();
            shaderLabel                       = Utility::GetFileWithoutExtension(Utility::GetFileNameOnly(shaderLabelFull));
        }

        if (BeginComboBox(comboID, shaderLabel.c_str(), true))
        {

            for (auto& shaderResource : shaderCache)
            {
                Graphics::Shader* shader = storage->GetResource<Graphics::Shader>(shaderResource.first);

                const bool selected = currentShaderID == shader->GetSID();

                std::string label = shader->GetPath();
                label             = Utility::GetFileWithoutExtension(Utility::GetFileNameOnly(label));

                if (ImGui::Selectable(label.c_str(), selected))
                    shaderToReturn = shader;

                if (selected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }

        const std::string comboEndID = std::string(comboID) + "_comboEnd";
        if (PostComboBox(comboEndID.c_str()) && removed != nullptr)
            *removed = true;

        return shaderToReturn;
    }

    Physics::PhysicsMaterial* Editor::WidgetsUtility::PhysicsMaterialComboBox(const char* comboID, const std::string& currentPath, bool* removed)
    {
        auto*                     storage          = Resources::ResourceStorage::Get();
        Physics::PhysicsMaterial* materialToReturn = nullptr;
        std::string               materialLabel    = "";
        if (storage->Exists<Physics::PhysicsMaterial>(currentPath))
        {
            materialLabel = Utility::GetFileWithoutExtension(Utility::GetFileNameOnly(currentPath));
        }

        if (BeginComboBox(comboID, materialLabel.c_str(), true))
        {
            auto* storage = Resources::ResourceStorage::Get();
            auto& cache   = storage->GetCache<Physics::PhysicsMaterial>();

            for (const auto& p : cache)
            {
                auto*      material = storage->GetResource<Physics::PhysicsMaterial>(p.first);
                const bool selected = currentPath == material->GetPath();

                std::string label = material->GetPath();
                label             = Utility::GetFileWithoutExtension(Utility::GetFileNameOnly(label));
                if (ImGui::Selectable(label.c_str(), selected))
                {
                    materialToReturn = material;
                }

                if (selected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }

        const std::string comboEndID = std::string(comboID) + "_comboEnd";
        if (PostComboBox(comboEndID.c_str()) && removed != nullptr)
            *removed = true;

        return materialToReturn;
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

    StringIDType WidgetsUtility::ResourceSelection(void* currentResource, void* currentHandle, const char* resourceStr, bool* removed, TypeID resourceType)
    {
        std::string     resourceName     = "None";
        constexpr float spaceFromEnd     = 10.0f;
        const float     removeButtonSize = ImGui::GetFrameHeight();

        if (currentResource != nullptr)
            resourceName = Utility::GetFileWithoutExtension(Utility::GetFileNameOnly(((Resources::IResource*)currentResource)->GetPath()));

        // Selection button.
        ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_FrameBg));
        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));
        bool pressed = ImGui::Button(resourceName.c_str(), ImVec2(-spaceFromEnd - removeButtonSize - ImGui::GetStyle().ItemSpacing.x, removeButtonSize));
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();

        // Remove button.
        ImGui::SameLine();
        PushIconFontSmall();
        *removed = Button(ICON_FA_MINUS, ImVec2(removeButtonSize, removeButtonSize), 1, 3);
        ImGui::PopFont();

        if (pressed)
        {
            s_latestResourceHandle = currentHandle;
            GUILayer::Get()->GetResourceSelector().SetCurrentTypeID(resourceType, resourceStr);
            GUILayer::Get()->GetResourceSelector().Open();
        }

        const StringIDType selectedResource = GUILayer::Get()->GetResourceSelector().GetSelectedResource();
        if (s_latestResourceHandle == currentHandle && selectedResource != 0)
        {
            s_latestResourceHandle = nullptr;
            return selectedResource;
        }

        return 0;
    }

    StringIDType WidgetsUtility::ResourceSelectionMaterial(void* handleAddr)
    {
        Resources::ResourceHandle<Graphics::Material>* handle = static_cast<Resources::ResourceHandle<Graphics::Material>*>(handleAddr);

        bool         pressed  = false;
        bool         removed  = false;
        StringIDType selected = ResourceSelection(static_cast<void*>(handle->m_value), static_cast<void*>(handle), "Material", &removed, GetTypeID<Graphics::Material>());

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

    StringIDType WidgetsUtility::ResourceSelectionTexture(void* handleAddr)
    {

        return 0;
    }

    StringIDType WidgetsUtility::ResourceSelectionAudio(void* handleAddr)
    {
        return StringIDType();
    }

    StringIDType WidgetsUtility::ResourceSelectionModelNode(void* handleAddr)
    {
        return StringIDType();
    }

    StringIDType WidgetsUtility::ResourceSelectionPhysicsMaterial(void* handleAddr)
    {
        return StringIDType();
    }

    StringIDType WidgetsUtility::ResourceSelectionShader(void* handleAddr)
    {
        return StringIDType();
    }

    bool WidgetsUtility::Button(const char* label, const ImVec2& size, float textSize, float rounding, ImVec2 contentOffset)
    {
        FrameRounding(rounding);

        if (textSize != 1.0f)
            WidgetsUtility::PushScaledFont(textSize);

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

    bool WidgetsUtility::ToolbarToggleIcon(const char* label, const ImVec2 size, int imagePadding, bool toggled, float cursorPosY, const std::string& tooltip, ImVec4 color, float scale)
    {
        ImGui::SetCursorPosY(cursorPosY);
        ImVec2 windowPos = ImGui::GetWindowPos();
        ImVec2 min       = ImVec2(windowPos.x + ImGui::GetCursorPosX(), windowPos.y + ImGui::GetCursorPosY());
        ImVec2 max       = ImVec2(windowPos.x + ImGui::GetCursorPosX() + size.x, windowPos.y + ImGui::GetCursorPosY() + size.y);

        bool hovered = ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(min, max);
        bool pressed = hovered && ImGui::IsMouseDown(ImGuiMouseButton_Left);

        ImVec4 toggledColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonLocked);
        ImVec4 hoveredColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered);
        ImVec4 pressedColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive);
        ImVec4 defaultColor = ImGui::GetStyleColorVec4(ImGuiCol_Button);
        ImVec4 col          = toggled ? toggledColor : (pressed ? pressedColor : (hovered ? hoveredColor : defaultColor));
        ImGui::GetWindowDrawList()->AddRectFilled(min, max, ImGui::ColorConvertFloat4ToU32(col), 4.0f);

        if (tooltip.compare("") != 0 && hovered)
        {
            Tooltip(tooltip.c_str());
        }

        const float  yIncrement    = size.y / 4.0f + 1;
        const ImVec2 currentCursor = ImGui::GetCursorPos();
        PushScaledFont(scale);
        ImVec2 textSize = ImGui::CalcTextSize(label);
        ImGui::SameLine();
        ImGui::SetCursorPosX(currentCursor.x + size.x / 2.0f - textSize.x / 2.0f);
        ImGui::SetCursorPosY(currentCursor.y + size.y / 2.0f - textSize.y / 2.0f);
        ImGui::Text(label);
        PopScaledFont();

        if (hovered && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
            return true;

        return false;
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
        ImVec2            rectMin    = ImVec2(windowPos.x + cursorPos.x, windowPos.y + cursorPos.y + 2);
        ImVec2            rectMax    = ImVec2(rectMin.x + itemHeight - 2, rectMin.y + itemHeight - 4);
        ImVec2            rectSize   = ImVec2(rectMax.x - rectMin.x, rectMax.y - rectMin.y);
        ImVec4            rectCol    = ImVec4(0.13f, 0.13f, 0.13f, 1.0f);
        ImVec4            textCol    = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        const std::string rectID     = std::string(id) + "_rect";

        if (ImGui::IsMouseHoveringRect(rectMin, rectMax))
            ImGui::SetHoveredID(ImHashStr(rectID.c_str()));

        FramePaddingX(itemHeight + 1);
        bool result = ImGui::InputFloat(id, var);
        PopStyleVar();

        ImGui::GetWindowDrawList()->AddRectFilled(rectMin, rectMax, ImGui::ColorConvertFloat4ToU32(rectCol), 1);
        DragBehaviour(rectID.c_str(), var, ImRect(rectMin, rectMax));

        if (isIcon)
            PushScaledFont(0.6f);

        const ImVec2 textSize = ImGui::CalcTextSize(label);
        ImGui::GetWindowDrawList()->AddText(ImVec2(rectMin.x + rectSize.x / 2.0f - textSize.x / 2.0f, rectMin.y + rectSize.y / 2.0f - textSize.y / 2.0f), ImGui::ColorConvertFloat4ToU32(textCol), label);

        if (isIcon)
            PopScaledFont();

        return result;
    }

    bool Editor::WidgetsUtility::DragInt(const char* id, const char* label, int* var, int count)
    {
        if (label != nullptr)
        {
            float labelSize = ImGui::CalcTextSize(label).x + ImGui::GetStyle().ItemSpacing.x;
            IncrementCursorPosX(-labelSize);
            ImGui::Text(label);
            ImGui::SameLine();
        }
        else
        {

            IncrementCursorPosX(-21.2f);
            IncrementCursorPosY(6.2f);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 0.3f));
            PushScaledFont(0.7f);
            ImGui::Text(ICON_FA_ARROWS_ALT_H);
            PopScaledFont();
            ImGui::PopStyleColor();
            ImGui::SameLine();
            IncrementCursorPosX(-0.6f);
            IncrementCursorPosY(-5.8f);
        }

        DragBehaviour(id, var);

        float windowWidth   = ImGui::GetWindowWidth();
        float currentCursor = ImGui::GetCursorPosX();
        float remaining     = (windowWidth - currentCursor) / (float)count - 10;
        float comboWidth    = remaining - 10;
        ImGui::SetNextItemWidth(comboWidth);
        return ImGui::InputInt(id, var);
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
