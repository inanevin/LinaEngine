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

#include "Utility/UtilityFunctions.hpp"
#include "Utility/StringId.hpp"
#include "ECS/ECS.hpp"
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
		class Shader;
	}

	namespace Physics
	{
		class PhysicsMaterial;
	}
}

namespace Lina::Editor
{
	class WidgetsUtility
	{

	public:

		/// <summary>
		/// Draws a simple tooltip pop-up.
		/// </summary>
		static void Tooltip(const char* tooltip);

		/// <summary>
		/// Draws a button with the given size, with an option to draw an icon/text in the middle.
		/// Use ImGuiCol_ButtonX to style, ImGuiCol_Icon to style the icon color.</summary>
		/// <returns></returns>
		static bool CustomButton(const char* id, ImVec2 size, bool* isHovered = nullptr, bool locked = false, const char* icon = nullptr, float rounding = 0.0f, const char* tooltip = nullptr);

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
		static void DrawTreeFolder(Utility::Folder& folder, Utility::Folder*& selectedFolder, Utility::Folder*& hoveredFolder, float height, float offset, ImVec4 defaultBackground, ImVec4 hoverBackground = ImVec4(0, 0, 0, 0), ImVec4 selectedBackground = ImVec4(0, 0, 0, 0));

		/// <summary>
		/// Button with color-picker pop-up
		/// </summary>
		/// <param name="id"></param>
		/// <param name="colorX"></param>
		static void ColorButton(const char* id, float* colorX);

		/// <summary>
		/// Draws a button that the user can toggle from left-to right with toggling animation.
		/// </summary>
		static bool ToggleButton(const char* label, bool* v, float heightMultiplier = 1.0f, float widthMultiplier = 1.0f, const ImVec4& activeColor = ImVec4(0.56f, 0.83f, 0.26f, 1.0f), const ImVec4& activeHoveredColor = ImVec4(0.64f, 0.83f, 0.34f, 1.0f), const ImVec4& inActiveColor = ImVec4(0.85f, 0.85f, 0.85f, 1.0f), const ImVec4& inActiveHovered = ImVec4(0.78f, 0.78f, 0.78f, 1.0f));   // toggle button

		/// <summary>
		/// Draws a full-window-width line, the Y position determines the local offset from current cursor pos.
		/// Use ImGuiCol_Text to style.
		/// </summary>
		/// <param name="thickness"></param>
		/// <param name="color"></param>
		static void HorizontalDivider(float yOffset = 0.0f, float thickness = 1.0f);

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
		static bool ComponentHeader(Lina::ECS::TypeID tid, bool* foldoutOpen, const char* componentLabel, const char* componentIcon, bool* toggled, bool* removed, bool* copied, bool* pasted, bool* resetted, bool moveButton = true);

		/// <summary>
		/// Draws a header same style as component headers, no icons or component buttons. Returns true if pressed.
		/// </summary>
		static bool Header(const char* title, bool* foldoutOpen);

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
		/// Drop-down combo-box for selecting any loaded material in the project.
		/// </summary>
		static Lina::Graphics::Material* MaterialComboBox(const char* comboID, const std::string& currentPath, bool* removed = nullptr);

		/// <summary>
		/// Drop-down combo-box for selecting any loaded model in the project.
		/// </summary>
		static Lina::Graphics::Model* ModelComboBox(const char* comboID, int currentModelID, bool* removed = nullptr);

		/// <summary>
		/// Drop-down combo-box for selecting any loaded shader material in the project.
		/// </summary>
		static Lina::Graphics::Shader* ShaderComboBox(const char* comboID, int currentShaderID, bool* removed = nullptr);


		/// <summary>
		/// Drop-down combo-box for selecting any loaded physics material in the project.
		/// </summary>
		static Lina::Physics::PhysicsMaterial* PhysicsMaterialComboBox(const char* comboID, const std::string& currentPath, bool* removed = nullptr);


		/// <summary>
		/// Drop-down combo-box for selecting collision shapes.
		/// </summary>
		static int CollisionShapeComboBox(const char* comboID, int currentShapeID);

		/// <summary>
		/// Default IMGUI button with fixed styling options.
		/// </summary>
		static bool Button(const char* label, const ImVec2& size = ImVec2(0, 0));

		/// <summary>
		/// Draws icon buttons used in the main toolbar.
		/// </summary>
		static bool ToolbarToggleIcon(const char* label, const ImVec2 size, int imagePadding, bool toggled, float cursorPosY, const std::string& tooltip, ImVec4 color = ImVec4(1, 1, 1, 1), float fontScale = 0.75f);

		static bool SelectableInput(const char* str_id, bool selected, int flags, char* buf, size_t buf_size);
		static void DrawWindowBorders(const ImVec4& color, float thickness);
		static void DrawShadowedLine(int height = 10, const ImVec4& color = ImVec4(0.1f, 0.1f, 0.1f, 1.0f), float thickness = 1.0f, ImVec2 min = ImVec2(0, 0), ImVec2 max = ImVec2(0, 0));
		static void DrawBeveledLine(ImVec2 min = ImVec2(0, 0), ImVec2 max = ImVec2(0, 0));
		static void ScreenPosLine();
		static void Icon(const char* label, float scale = 0.6f, const ImVec4& color = ImVec4(1, 1, 1, 1));
		static bool IconButtonNoDecoration(const char* label, float width = 0.0f, float scale = 0.6f);
		static bool IconButton(const char* id, const char* label, float width = 0.0f, float scale = 0.6f, const ImVec4& color = ImVec4(1, 1, 1, 0.6f), const ImVec4& hoverColor = ImVec4(1, 1, 1, .8f), const ImVec4& pressedColor = ImVec4(1, 1, 1, 1.0f), bool disabled = false);
		static ImVec2 GetWindowPosWithContentRegion();
		static ImVec2 GetWindowSizeWithContentRegion();
		static bool InputQuaternion(const char* label, Lina::Quaternion& v);
		static bool DragQuaternion(const char* label, Lina::Quaternion& v);
		static void AlignedText(const char* label);
		static bool Caret(const char* title);
		static bool CaretAndLabel(const char* title, const char* label);
		static void IncrementCursorPosX(float f);
		static void IncrementCursorPosY(float f);
		static void IncrementCursorPos(const ImVec2& v);
		static void CenteredText(const char* label);
		static void CenterCursorX();
		static void CenterCursorY();
		static void CenterCursor();
		static float DebugFloat(const char* id = "debug", bool currentWindow = false);
		static void PushScaledFont(float defaultScale = 0.6f);
		static void PopScaledFont();
		static void FramePaddingX(float amt);
		static void FramePaddingY(float amt);
		static void FramePadding(const ImVec2& amt);
		static void FrameRounding(float rounding);
		static void WindowPaddingX(float amt);
		static void WindowPaddingY(float amt);
		static void WindowPadding(const ImVec2& amt);
		static void ItemSpacingX(float amt);
		static void ItemSpacingY(float amt);
		static void ItemSpacing(const ImVec2& amt);
		static void WindowRounding(float rounding);
		static void PopStyleVar();

		static std::map<std::string, bool> s_carets;

	private:

		static std::map<std::string, std::tuple<bool, bool>> s_iconButtons;
		static std::map<std::string, float> s_debugFloats;
	};
}

#endif
