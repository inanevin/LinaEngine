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
Timestamp: 10/11/2020 1:39:27 PM

*/


#include "Widgets/WidgetsUtility.hpp"
#include "Utility/Math/Math.hpp"

namespace LinaEditor
{

	void WidgetsUtility::ColorButton(float* colorX)
	{
		static bool alpha_preview = true;
		static bool alpha_half_preview = false;
		static bool drag_and_drop = true;
		static bool options_menu = true;
		static bool hdr = true;
		ImGuiColorEditFlags misc_flags = (hdr ? (ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float) : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);
		// Generate a dummy default palette. The palette will persist and can be edited.
		static bool saved_palette_init = true;
		static ImVec4 saved_palette[32] = {};
		if (saved_palette_init)
		{
			for (int n = 0; n < IM_ARRAYSIZE(saved_palette); n++)
			{
				ImGui::ColorConvertHSVtoRGB(n / 31.0f, 0.8f, 0.8f,
					saved_palette[n].x, saved_palette[n].y, saved_palette[n].z);
				saved_palette[n].w = 1.0f; // Alpha
			}
			saved_palette_init = false;
		}

		static ImVec4 backup_color;
		bool open_popup = ImGui::ColorButton("MyColor##3b", ImVec4(colorX[0], colorX[1], colorX[2], colorX[3]), misc_flags);
		ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
		open_popup |= ImGui::Button("Light Color");
		if (open_popup)
		{
			ImGui::OpenPopup("mypicker");
			backup_color = ImVec4(colorX[0], colorX[1], colorX[2], colorX[3]);
		}
		if (ImGui::BeginPopup("mypicker"))
		{
			ImGui::PushItemWidth(160);

			ImGui::Text("Color Picker!");
			ImGui::Separator();
			ImGui::ColorPicker4("##picker", colorX, misc_flags | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);
			ImGui::SameLine();

			ImGui::BeginGroup(); // Lock X position
			ImGui::Text("Current");
			ImGui::ColorButton("##current", ImVec4(colorX[0], colorX[1], colorX[2], colorX[3]), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40));
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

	bool WidgetsUtility::SelectableInput(const char* str_id, bool selected, int flags, char* buf, size_t buf_size)
	{
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;
		ImVec2 pos_before = window->DC.CursorPos;

		ImGui::PushID(str_id);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(g.Style.ItemSpacing.x, g.Style.FramePadding.y * 2.0f));
		bool ret = ImGui::Selectable("##Selectable", selected, flags | ImGuiSelectableFlags_AllowDoubleClick | ImGuiSelectableFlags_AllowItemOverlap);
		ImGui::PopStyleVar();

		ImGuiID id = window->GetID("##Input");
		bool temp_input_is_active = ImGui::TempInputIsActive(id);
		bool temp_input_start = ret ? ImGui::IsMouseDoubleClicked(0) : false;

		if (temp_input_start)
			ImGui::SetActiveID(id, window);

		if (temp_input_is_active || temp_input_start)
		{
			ImVec2 pos_after = window->DC.CursorPos;
			window->DC.CursorPos = pos_before;
			ret = ImGui::TempInputText(window->DC.LastItemRect, id, "##Input", buf, (int)buf_size, ImGuiInputTextFlags_None);
			window->DC.CursorPos = pos_after;
		}
		else
		{
			window->DrawList->AddText(pos_before, ImGui::GetColorU32(ImGuiCol_Text), buf);
		}

		ImGui::PopID();
		return ret;
	}

	bool WidgetsUtility::ToggleButton(char* label, bool* v, float heightMultiplier, float widthMultiplier, const ImVec4& activeColor, const ImVec4& activeHoveredColor, const ImVec4& inactiveColor, const ImVec4& inactiveHoveredColor)
	{
		ImVec2 p = ImGui::GetCursorScreenPos();
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		float height = ImGui::GetFrameHeight() * heightMultiplier;
		float width = height * 1.55f * widthMultiplier;
		float radius = height * 0.50f;

		ImGui::InvisibleButton(label, ImVec2(width, height));
		if (ImGui::IsItemClicked())
			*v = !*v;

		float t = *v ? 1.0f : 0.0f;

		ImGuiContext& g = *GImGui;
		float ANIM_SPEED = 0.08f;
		if (g.LastActiveId == g.CurrentWindow->GetID(label))// && g.LastActiveIdTimer < ANIM_SPEED)
		{
			float t_anim = ImSaturate(g.LastActiveIdTimer / ANIM_SPEED);
			t = *v ? (t_anim) : (1.0f - t_anim);
		}

		ImU32 col_bg;
		if (ImGui::IsItemHovered())
			col_bg = ImGui::GetColorU32(ImLerp(inactiveHoveredColor, activeHoveredColor, t));
		else
			col_bg = ImGui::GetColorU32(ImLerp(inactiveColor, activeColor, t));

		draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), col_bg, height * 0.5f);
		draw_list->AddCircleFilled(ImVec2(p.x + radius + t * (width - radius * 2.0f), p.y + radius), radius - 1.5f, IM_COL32(255, 255, 255, 255));

		return *v;
	}

	void WidgetsUtility::DrawWindowBorders(const ImVec4& color, float thickness)
	{
		ImVec2 min = ImVec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
		ImVec2 max = ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowWidth(), ImGui::GetWindowPos().y + ImGui::GetWindowHeight());
		ImGui::BeginChild("##ch");
		ImGui::PushClipRect(min, max, false);
		ImGui::GetWindowDrawList()->AddRect(min, max, ImGui::ColorConvertFloat4ToU32(color), 0, 15, thickness);
		ImGui::PopClipRect();
		ImGui::EndChild();
	}

	void WidgetsUtility::DrawShadowedLine(int height, const ImVec4& color, float thickness, ImVec2 min, ImVec2 max)
	{
		{

			if (min.x == 0 && min.y == 0)
				min = ImVec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + ImGui::GetCursorPosY());

			if (max.x == 0 && max.y == 0)
				max = ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowWidth(), ImGui::GetWindowPos().y + ImGui::GetCursorPosY());

			for (int i = 0; i < height; i++)
			{
				ImGui::GetWindowDrawList()->AddLine(ImVec2(min.x, min.y + thickness * i), ImVec2(max.x, max.y + thickness * i), ImGui::ColorConvertFloat4ToU32(ImVec4(color.x, color.y, color.z, LinaEngine::Math::Remap((float)i, 0.0f, (float)height, 1.0f, 0.0f))), thickness);
			}
		}
	}

	void WidgetsUtility::DrawBeveledLine(ImVec2 min, ImVec2 max)
	{
		if (min.x == 0 && min.y == 0)
			min = ImVec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + ImGui::GetCursorPosY());

		if (max.x == 0 && max.y == 0)
			max = ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowWidth(), ImGui::GetWindowPos().y + ImGui::GetCursorPosY());

		ImGui::GetWindowDrawList()->AddLine(min, max, ImGui::ColorConvertFloat4ToU32(ImVec4(0.1f, 0.1f, 0.1f, 1.0f)), 1);
		ImGui::GetWindowDrawList()->AddLine(ImVec2(min.x, min.y + 2), ImVec2(max.x, max.y + 2), ImGui::ColorConvertFloat4ToU32(ImVec4(0.2f, 0.2f, 0.2f, 1.0f)), 1);
	}
}