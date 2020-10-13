/*
Author: Inan Evin
www.inanevin.com
https://github.com/inanevin/LinaEngine

Copyright 2020~ Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: WidgetsUtility
Timestamp: 10/11/2020 1:39:01 PM

*/
#pragma once

#ifndef WidgetsUtility_HPP
#define WidgetsUtility_HPP

// Headers here.
#include "Utility/Math/Vector.hpp"
#include "Utility/Math/Quaternion.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_internal.h"
#include <map>

namespace LinaEditor
{
	class WidgetsUtility
	{
		
	public:
		
		static void ColorButton(float* colorX);
		static bool SelectableInput(const char* str_id, bool selected, int flags, char* buf, size_t buf_size);
		static bool ToggleButton(const char* label, bool* v, float heightMultiplier = 1.0f, float widthMultiplier = 1.0f, const ImVec4& activeColor = ImVec4(0.56f, 0.83f, 0.26f, 1.0f), const ImVec4& activeHoveredColor = ImVec4(0.64f, 0.83f, 0.34f, 1.0f), const ImVec4& inActiveColor = ImVec4(0.85f, 0.85f, 0.85f, 1.0f), const ImVec4& inActiveHovered = ImVec4(0.78f, 0.78f, 0.78f, 1.0f));   // toggle button
		static void DrawWindowBorders(const ImVec4& color, float thickness);
		static void DrawShadowedLine(int height = 10, const ImVec4& color = ImVec4(0.1f, 0.1f,0.1f, 1.0f), float thickness = 1.0f, ImVec2 min = ImVec2(0,0), ImVec2 max = ImVec2(0,0));
		static void DrawBeveledLine(ImVec2 min = ImVec2(0, 0), ImVec2 max = ImVec2(0, 0));
		static bool DrawComponentTitle(const char* title, const char* icon, bool* enabled, bool* foldoutOpen, const ImVec4& iconFolor = ImVec4(1,1,1,1));
		static void Icon(const char* label, float scale = 0.6f, const ImVec4& color = ImVec4(1, 1, 1, 1));
		static bool IconButtonNoDecoration(const char* label, float width = 0.0f, float scale = 0.6f);
		static bool IconButton(const char* id, const char* label, float width = 0.0f, float scale = 0.6f, const ImVec4& color = ImVec4(1, 1, 1, 0.6f), const ImVec4& hoverColor = ImVec4(1,1,1,.8f), const ImVec4& pressedColor = ImVec4(1, 1, 1, 1.0f));
		static bool Button(const char* label, const ImVec2& size = ImVec2(0,0));
		static bool InputQuaternion(const char* label, LinaEngine::Quaternion& v);
		static bool DragQuaternion(const char* label, LinaEngine::Quaternion& v);
		static void AlignedText(const char* label);
		static void IncrementCursorPosX(float f);
		static void IncrementCursorPosY(float f);
		static void IncrementCursorPos(const LinaEngine::Vector2& v);
		static void CenterCursorX();
		static void CenterCursorY();
		static void CenterCursor();
		static float DebugFloat(const char* id = "debug", bool currentWindow = false);
		static void PushScaledFont(float defaultScale = 0.6f);
		static void PopScaledFont();
		static void FramePaddingX(float amt);
		static void FramePaddingY(float amt);
		static void FrameRounding(float rounding);
		static void PopStyleVar();
	

	private:
	
		static std::map<std::string, std::tuple<bool,bool>> s_iconButtons;
		static std::map<std::string, float> s_debugFloats;
	};
}

#endif
