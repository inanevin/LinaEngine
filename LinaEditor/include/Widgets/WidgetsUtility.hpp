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
}

namespace Lina::Editor
{
	class WidgetsUtility
	{
		
	public:
		
		static void ColorButton(const char* id, float* colorX);
		static bool SelectableInput(const char* str_id, bool selected, int flags, char* buf, size_t buf_size);
		static bool ToggleButton(const char* label, bool* v, float heightMultiplier = 1.0f, float widthMultiplier = 1.0f, const ImVec4& activeColor = ImVec4(0.56f, 0.83f, 0.26f, 1.0f), const ImVec4& activeHoveredColor = ImVec4(0.64f, 0.83f, 0.34f, 1.0f), const ImVec4& inActiveColor = ImVec4(0.85f, 0.85f, 0.85f, 1.0f), const ImVec4& inActiveHovered = ImVec4(0.78f, 0.78f, 0.78f, 1.0f));   // toggle button
		static void DrawWindowBorders(const ImVec4& color, float thickness);
		static void DrawShadowedLine(int height = 10, const ImVec4& color = ImVec4(0.1f, 0.1f,0.1f, 1.0f), float thickness = 1.0f, ImVec2 min = ImVec2(0,0), ImVec2 max = ImVec2(0,0));
		static void DrawBeveledLine(ImVec2 min = ImVec2(0, 0), ImVec2 max = ImVec2(0, 0));
		static void ScreenPosLine();
		static void Icon(const char* label, float scale = 0.6f, const ImVec4& color = ImVec4(1, 1, 1, 1));
		static bool IconButtonNoDecoration(const char* label, float width = 0.0f, float scale = 0.6f);
		static bool IconButton(const char* id, const char* label, float width = 0.0f, float scale = 0.6f, const ImVec4& color = ImVec4(1, 1, 1, 0.6f), const ImVec4& hoverColor = ImVec4(1,1,1,.8f), const ImVec4& pressedColor = ImVec4(1, 1, 1, 1.0f), bool disabled = false);
		static bool Button(const char* label, const ImVec2& size = ImVec2(0,0));
		static bool InputQuaternion(const char* label, Lina::Quaternion& v);
		static bool DragQuaternion(const char* label, Lina::Quaternion& v);
		static void AlignedText(const char* label);
		static bool Caret(const char* title);
		static void IncrementCursorPosX(float f);
		static void IncrementCursorPosY(float f);
		static void IncrementCursorPos(const ImVec2& v);
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
		static Lina::Graphics::Material* MaterialComboBox(const char* comboID, const std::string& currentPath);
		static Lina::Graphics::Model* ModelComboBox(const char* comboID, int currentModelID);
		static Lina::Graphics::Shader* ShaderComboBox(const char* comboID, int currentShaderID);

		static std::map<std::string, bool> s_carets;

	private:
	
		static std::map<std::string, std::tuple<bool,bool>> s_iconButtons;
		static std::map<std::string, float> s_debugFloats;
	};
}

#endif
