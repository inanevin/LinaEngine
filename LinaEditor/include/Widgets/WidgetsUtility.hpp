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
Class: WidgetsUtility

Wraps ImGui functions and provides extended functionality.

Timestamp: 10/11/2020 1:39:01 PM
*/

#pragma once

#ifndef WidgetsUtility_HPP
#define WidgetsUtility_HPP

#include "ECS/Registry.hpp"
#include "Utility/StringId.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "Core/CommonResources.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_internal.h"

#include <map>
#include <string>

namespace Lina
{
    class Quaternion;

    namespace Graphics
    {
        class Material;
        class Model;
        class ModelNode;
        class Shader;
    } // namespace Graphics

    namespace Audio
    {
        class Audio;
    }

    namespace Physics
    {
        class PhysicsMaterial;
    }
} // namespace Lina

namespace Lina::Editor
{
    class MenuBarElement;
    class Menu;

    class WidgetsUtility
    {

    public:
        /// <summary>
        /// Draws a simple tooltip pop-up.
        /// </summary>
        static void Tooltip(const char* tooltip);

        /// <summary>
        /// Draws a button with the given size, with an option to draw an icon/text in the middle. Use the toggled parameter to
        /// keep the button pressed/unpressed. Use ImGuiCol_ButtonLocked to style the toggled state, ImGuiCol_Icon to style the icon color.</summary>
        /// </summary>
        static bool CustomToggle(const char* id, ImVec2 size, bool toggled, bool* hoveredPtr = nullptr, const char* icon = nullptr, float rounding = 0.0f, const char* tooltip = nullptr);

        /// <summary>
        /// Returns whether two ImGui colors are equal or not.
        /// </summary>
        static bool ColorsEqual(ImVec4 col1, ImVec4 col2);

        /// <summary>
        /// Draws a folder in the style of Resources Panel.
        /// </summary>
        static bool DrawTreeFolder(Utility::Folder* folder, Utility::Folder*& selectedFolder, bool canRename);

        /// <summary>
        /// Draws the rectangular nodes in the Resources right pane.
        /// </summary>
        /// <param name="isFolder"></param>
        /// <param name="fullPath"></param>
        static void DrawResourceNode(Utility::DirectoryItem* item, bool selected, bool* renamedItem, float sizeMultiplier = 1.0f, bool canRename = false);

        /// <summary>
        /// Button with color-picker pop-up
        /// </summary>
        /// <param name="id"></param>
        /// <param name="colorX"></param>
        static void ColorButton(const char* id, float* colorX);

        /// <summary>
        /// Draws a button that the user can toggle from left-to right with toggling animation.
        /// </summary>
        static bool ToggleButton(const char* label, bool* toggled, ImVec2 size = ImVec2(0.0f, 0.0f)); // toggle button

        /// <summary>
        /// Draws a full-window-width line, the Y position determines the local offset from current cursor pos.
        /// Use ImGuiCol_Text to style.
        /// </summary>
        /// <param name="thickness"></param>
        /// <param name="color"></param>
        static void HorizontalDivider(float yOffset = 0.0f, float thickness = 1.0f, float maxOverride = 0.0f);

        /// <summary>
        /// Draws minimize, maximize and close buttons on the window. Pass in the window ID used for BeginWindow().
        /// </summary>
        static void WindowButtons(const char* windowID, float yOffset = 0.0f, bool isAppWindow = false);

        /// <summary>
        /// Draws a custom title bar for the window.
        /// </summary>
        /// <param name="label"></param>
        static void WindowTitlebar(const char* label);

        /// <summary>
        /// Draws multiple horizontal dividers to create a drop shadow effect.
        /// </summary>
        static void DropShadow();

        /// <summary>
        /// Draws a horizontal reactangle, with foldout caret, component title, icon, and component buttons.
        /// </summary>
        static bool ComponentHeader(TypeID tid, bool* foldoutOpen, const char* componentLabel, const char* componentIcon, bool* toggled, bool* removed, bool* copied, bool* pasted, bool* resetted, bool moveButton = true, bool disableHeader = false);

        /// <summary>
        /// Draws a header same style as component headers, no icons or component buttons. Returns true if pressed.
        /// Send in a cursor pos value to get the cursor position inside the header.
        /// </summary>
        static bool Header(const char* title, bool* foldoutOpen, ImVec2* cursorPos = nullptr);

        /// <summary>
        /// Draws a simple caret and a title, return true upon press.
        /// </summary>
        static bool CaretTitle(const char* title, bool* caretOpen);

        /// <summary>
        /// Draws a property label, automatically sets the cursor position, asks for the same line.
        /// </summary>
        /// <param name="label"></param>
        static void PropertyLabel(const char* label, bool sameLine = true);

        /// <summary>
        /// Base implementation of a combobox begin, uses ImGui Combo with custom remove buttons.
        /// </summary>
        static bool BeginComboBox(const char* comboID, const char* label, bool hasRemoveButton = false);

        /// <summary>
        /// Base implementation of a combobox end, uses ImGui Combo with custom remove buttons.
        /// </summary>
        static bool PostComboBox(const char* id);

        /// <summary>
        /// Drop-down combo-box for selecting any loaded material in the project.
        /// </summary>
        static Graphics::Material* MaterialComboBox(const char* comboID, const std::string& currentPath, bool* removed = nullptr);

        /// <summary>
        /// Drop-down combo-box for selecting any loaded model in the project.
        /// </summary>
        static Graphics::Model* ModelComboBox(const char* comboID, int currentModelID, bool* removed = nullptr);

        /// <summary>
        /// Drop-down combo-box for selecting any loaded shader material in the project.
        /// </summary>
        static Graphics::Shader* ShaderComboBox(const char* comboID, int currentShaderID, bool* removed = nullptr);

        /// <summary>
        /// Drop-down combo-box for selecting any loaded physics material in the project.
        /// </summary>
        static Physics::PhysicsMaterial* PhysicsMaterialComboBox(const char* comboID, const std::string& currentPath, bool* removed = nullptr);

        /// <summary>
        /// Drop-down combo-box for selecting physics simulation type.
        /// </summary>
        static int SimulationTypeComboBox(const char* comboID, int currentShapeID);

        /// <summary>
        /// Drop-down combo-box for selecting collision shapes.
        /// </summary>
        static int CollisionShapeComboBox(const char* comboID, int currentShapeID);

        /// <summary>
        /// Draws a combo-box selection type of frame which launches the resource selector panel upon clicking.
        /// </summary>
        static StringIDType ResourceSelection(void* currentResource, void* currentHandle, const char* resourceStr, bool* removed, TypeID resourceType);

        /// <summary>
        /// Material selection helper, calls ResourceSelection().
        /// </summary>
        static StringIDType ResourceSelectionMaterial(void* handleAddr);

        /// <summary>
        /// Material selection helper, calls ResourceSelection().
        /// </summary>
        static StringIDType ResourceSelectionTexture(void* handleAddr);

        /// <summary>
        /// Material selection helper, calls ResourceSelection().
        /// </summary>
        static StringIDType ResourceSelectionAudio(void* handleAddr);

        /// <summary>
        /// Material selection helper, calls ResourceSelection().
        /// </summary>
        static StringIDType ResourceSelectionModelNode(void* handleAddr);

        /// <summary>
        /// Material selection helper, calls ResourceSelection().
        /// </summary>
        static StringIDType ResourceSelectionPhysicsMaterial(void* handleAddr);

        /// <summary>
        /// Material selection helper, calls ResourceSelection().
        /// </summary>
        static StringIDType ResourceSelectionShader(void* handleAddr);

        /// <summary>
        /// Default IMGUI button with fixed styling options.
        /// </summary>
        static bool Button(const char* label, const ImVec2& size = ImVec2(0, 0), float textSize = 1.0f, float rounding = 0.0f, ImVec2 contentOffset = ImVec2(0.0f, 0.0f));

        /// <summary>
        /// Draws a simple icon button with no background, hovering sets icon color.
        /// </summary>
        static bool IconButton(const char* icon, bool useSmallIcon = true);

        /// <summary>
        /// Tree node with a custom arrow.
        /// </summary>
        static bool TreeNode(const void* id, ImGuiTreeNodeFlags flags, const char* name, bool drawArrow);

        /// <summary>
        /// Draws icon buttons used in the main toolbar.
        /// </summary>
        static bool ToolbarToggleIcon(const char* label, const ImVec2 size, int imagePadding, bool toggled, float cursorPosY, const std::string& tooltip, ImVec4 color = ImVec4(1, 1, 1, 1), float fontScale = 0.75f);

        /// <summary>
        /// Dragging functionality for custom drag widgets.
        /// </summary>
        static void DragBehaviour(const char* id, float* var, ImRect rect);

        /// <summary>
        /// Dragging functionality for custom drag widgets.
        /// </summary>
        static void DragBehaviour(const char* id, int* var);

        /// <summary>
        /// Draws ImGui::DragFloat with custom dragger.
        /// </summary>
        static bool DragFloat(const char* id, const char* label, float* var, float width = -1.0f);

        /// <summary>
        /// Draws ImGui::DragInt with custom dragger.
        /// </summary>
        static bool DragInt(const char* id, const char* label, int* var, int count = 1);

        /// <summary>
        /// Draws 2 drag floats side by side.
        /// </summary>
        static bool DragVector2(const char* id, float* var);

        /// <summary>
        /// Draws 3 drag floats side by side.
        /// </summary>
        static bool DragVector3(const char* id, float* var);

        /// <summary>
        /// Draws 4 drag floats side by side.
        /// </summary>
        static bool DragVector4(const char* id, float* var);

        /// <summary>
        /// Returns current window position, excluding the window's title bar.
        /// </summary>
        /// <returns></returns>
        static ImVec2 GetWindowPosWithContentRegion();

        /// <summary>
        /// Returns current window size, excluding the window's title bar.
        /// </summary>
        /// <returns></returns>
        static ImVec2 GetWindowSizeWithContentRegion();

        /// <summary>
        /// Sets the cursor pos X such that the next item will be positioned in the middle of the column,
        /// given item width.
        /// </summary>
        /// <param name="itemWidth"></param>
        static void TableAlignCenter(float itemWidth);

        /// <summary>
        /// Draws a text centered within the current window.
        /// </summary>
        static void CenteredText(const char* label);

        /// <summary>
        /// Sets ImGui cursor position X & Y to the current window center.
        /// </summary>
        static void CenterCursor();

        /// <summary>
        /// Sets ImGui cursor position X to current window center.
        /// </summary>
        static void CenterCursorX();

        /// <summary>
        /// Sets ImGui cursor position Y to current window center.
        /// </summary>
        static void CenterCursorY();

        /// <summary>
        /// Increments ImGui Cursor, both X & Y.
        /// </summary>
        static void IncrementCursorPos(const ImVec2& v);

        /// <summary>
        /// Increments ImGui Cursor Pos X
        /// </summary>
        static void IncrementCursorPosX(float f);

        /// <summary>
        /// Increments ImGui Cursor Pos Y
        /// </summary>
        static void IncrementCursorPosY(float f);

        /// <summary>
        /// Use Push & Pop Scaled Font functions to scale only a single text/icon item.
        /// </summary>
        static void PushScaledFont(float defaultScale = 0.6f);

        /// <summary>
        /// Use Push & Pop Scaled Font functions to scale only a single text/icon item.
        /// </summary>
        static void PopScaledFont();

        /// <summary>
        /// ImGui Styling option, pushes ImGuiStyleVar_FramePadding X & Y
        /// </summary>
        static void FramePadding(const ImVec2& amt);

        /// <summary>
        /// ImGui Styling option, pushes ImGuiStyleVar_FramePadding X
        /// </summary>
        static void FramePaddingX(float amt);

        /// <summary>
        /// ImGui Styling option, pushes ImGuiStyleVar_FramePadding Y
        /// </summary>
        static void FramePaddingY(float amt);

        /// <summary>
        /// ImGui Styling option, pushes ImGuiStyleVar_FrameRounding
        /// </summary>
        static void FrameRounding(float rounding);

        /// <summary>
        /// ImGui Styling option, pushes ImGuiStyleVar_WindowPadding, both X & Y
        /// </summary>
        static void WindowPadding(const ImVec2& amt);

        /// <summary>
        /// ImGui Styling option, pushes ImGuiStyleVar_WindowPadding X
        /// </summary>
        static void WindowPaddingX(float amt);

        /// <summary>
        /// ImGui Styling option, pushes ImGuiStyleVar_WindowPadding Y
        /// </summary>
        static void WindowPaddingY(float amt);

        /// <summary>
        /// ImGui Styling option, pushes ImGuiStyleVar_ItemSpacing, both X & Y
        /// </summary>
        static void ItemSpacing(const ImVec2& amt);

        /// <summary>
        /// ImGui Styling option, pushes ImGuiStyleVar_ItemSpacing X
        /// </summary>
        static void ItemSpacingX(float amt);

        /// <summary>
        /// ImGui Styling option, pushes ImGuiStyleVar_ItemSpacing Y
        /// </summary>
        static void ItemSpacingY(float amt);

        /// <summary>
        /// ImGui Styling option, pushes ImGuiStyleVar_WindowRounding, both X & Y
        /// </summary>
        static void WindowRounding(float rounding);

        /// <summary>
        /// Pushes style vars for creating a uniform popup accross the engine.
        /// </summary>
        static void PushPopupStyle();

        /// <summary>
        /// Pops the last popup style.
        /// </summary>
        static void PopPopupStyle();

        /// <summary>
        /// Pops ImGui StyleVar Stack (ImGui::PopStyleVar)
        /// </summary>
        static void PopStyleVar();

        /// <summary>
        /// Draws a simple icon in the current cursor position, offers to modify the cursor to align.
        /// </summary>
        static void Icon(const char* label, bool align, float scale = 0.6f);

        /// <summary>
        /// Pushes the small icon font to the font stack & draws icon, then pops the stack.
        /// </summary>
        static void IconSmall(const char* icon);

        /// <summary>
        /// Pushes the default icon font to the font stack & draws icon, then pops the stack.
        /// </summary>
        /// <param name="icon"></param>
        static void IconDefault(const char* icon);

        /// <summary>
        /// Pushes the default icon font.
        /// </summary>
        static void PushIconFontDefault();

        /// <summary>
        /// Pushes the small icon font.
        /// </summary>
        static void PushIconFontSmall();
    };
} // namespace Lina::Editor

#endif
