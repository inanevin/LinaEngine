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

Class: EditorUtility
Timestamp: 5/9/2020 1:23:05 AM

*/


#include "Utility/EditorUtility.hpp"
#include "Rendering/Material.hpp"
//#include "boost/filesystem.hpp"
#include <filesystem>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_internal.h"

#include <cereal/archives/binary.hpp>
#include <fstream>

namespace LinaEditor
{
	bool EditorUtility::CreateFolderInPath(const std::string& path)
	{
		return std::filesystem::create_directory(path);
	}

	bool EditorUtility::GetDirectories(std::vector<std::string>& vec, const std::string& path)
	{
		
		return false;
	}

	bool EditorUtility::DeleteDirectory(const std::string& path)
	{
		return false;
	}

	void EditorUtility::SerializeMaterial(const std::string& path, LinaEngine::Graphics::Material& mat)
	{
		std::ofstream os(path);
		{
			cereal::BinaryOutputArchive oarchive(os); // Create an output archive
			oarchive(mat); // Write the data to the archive
		}
	}

	void EditorUtility::ColorButton(float* colorX)
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

	bool EditorUtility::SelectableInput(const char* str_id, bool selected, int flags, char* buf, size_t buf_size)
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

}