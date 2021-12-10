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
#include "Utility/Math/Math.hpp"
#include "Utility/Math/Quaternion.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/Model.hpp"
#include "Rendering/Shader.hpp"
#include "Drawers/ComponentDrawer.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "IconsFontAwesome5.h"

namespace Lina::Editor
{
	std::map<std::string, std::tuple<bool, bool>> WidgetsUtility::s_iconButtons;
	std::map<std::string, float> WidgetsUtility::s_debugFloats;
	std::map<std::string, bool> WidgetsUtility::s_carets;

	void WidgetsUtility::ColorButton(const char* id, float* colorX)
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

		std::string buf(id);
		static ImVec4 backup_color;	
		bool open_popup = ImGui::ColorButton(buf.c_str(), ImVec4(colorX[0], colorX[1], colorX[2], colorX[3]), misc_flags);
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

	bool WidgetsUtility::ToggleButton(const char* label, bool* v, float heightMultiplier, float widthMultiplier, const ImVec4& activeColor, const ImVec4& activeHoveredColor, const ImVec4& inactiveColor, const ImVec4& inactiveHoveredColor)
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

		if (min.x == 0 && min.y == 0)
			min = ImVec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + ImGui::GetCursorPosY());

		if (max.x == 0 && max.y == 0)
			max = ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowWidth(), ImGui::GetWindowPos().y + ImGui::GetCursorPosY());

		for (int i = 0; i < height; i++)
		{
			ImGui::GetWindowDrawList()->AddLine(ImVec2(min.x, min.y + thickness * i), ImVec2(max.x, max.y + thickness * i), ImGui::ColorConvertFloat4ToU32(ImVec4(color.x, color.y, color.z, Lina::Math::Remap((float)i, 0.0f, (float)height, 1.0f, 0.0f))), thickness);
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

	void WidgetsUtility::ScreenPosLine()
	{
		ImVec2 min = ImVec2(ImGui::GetCursorScreenPos());
		ImVec2 max = ImVec2(ImVec2(ImGui::GetCursorScreenPos().x + ImGui::GetContentRegionMax().x, ImGui::GetCursorScreenPos().y));
		ImGui::GetWindowDrawList()->AddLine(min, max, ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)));
	}

	bool WidgetsUtility::InputQuaternion(const char* label, Lina::Quaternion& v)
	{
		float rot[3] = { v.GetEuler().x, v.GetEuler().y,v.GetEuler().z };
		bool edited = ImGui::InputFloat3(label, rot);
		v = Lina::Quaternion::Euler(rot[0], rot[1], rot[2]);
		return edited;
	}

	bool WidgetsUtility::DragQuaternion(const char* label, Lina::Quaternion& v)
	{
		float rot[3] = { v.GetEuler().x, v.GetEuler().y,v.GetEuler().z };
		bool edited = ImGui::DragFloat3(label, rot);
		v = Lina::Quaternion::Euler(rot[0], rot[1], rot[2]);
		return edited;
	}

	void WidgetsUtility::AlignedText(const char* label)
	{
		ImGui::AlignTextToFramePadding();
		ImGui::Text(label);
	}

	bool WidgetsUtility::Caret(const char* title)
	{
		const char* caret = s_carets[title] ? ICON_FA_CARET_DOWN : ICON_FA_CARET_RIGHT;
		if (WidgetsUtility::IconButtonNoDecoration(caret, 30, 0.8f))
			s_carets[title] = !s_carets[title];
		return s_carets[title];
	}

	void WidgetsUtility::IncrementCursorPosX(float f)
	{
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + f);
	}

	void WidgetsUtility::IncrementCursorPosY(float f)
	{
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + f);
	}

	void WidgetsUtility::IncrementCursorPos(const  ImVec2& v)
	{
		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + v.x, ImGui::GetCursorPosY() + v.y));
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

	float WidgetsUtility::DebugFloat(const char* id, bool currentWindow)
	{
		if (!currentWindow)
			ImGui::Begin("WidgetsUtility");

		ImGui::InputFloat(id, &s_debugFloats[id]);

		if (!currentWindow)
			ImGui::End();

		return s_debugFloats[id];
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

	void WidgetsUtility::PopStyleVar()
	{
		ImGui::PopStyleVar();
	}

	Lina::Graphics::Material* WidgetsUtility::MaterialComboBox(const char* comboID, const std::string& currentPath)
	{
		Graphics::Material* materialToReturn = nullptr;

		std::string materialLabel = "";
		if (Graphics::Material::MaterialExists(currentPath))
		{
			materialLabel = Utility::GetFileWithoutExtension(Utility::GetFileNameOnly(currentPath));
		}

		if (ImGui::BeginCombo(comboID, materialLabel.c_str()))
		{
			auto& loadedMaterials = Graphics::Material::GetLoadedMaterials();

			for (auto& material : loadedMaterials)
			{
				const bool selected = currentPath == material.second.GetPath();

				std::string label = material.second.GetPath();
				label = Utility::GetFileWithoutExtension(Utility::GetFileNameOnly(label));
				if (ImGui::Selectable(label.c_str(), selected))
				{
					materialToReturn = &material.second;
				}

				if (selected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		return materialToReturn;
	}

	Lina::Graphics::Model* WidgetsUtility::ModelComboBox(const char* comboID, int currentModelID)
	{
		Lina::Graphics::Model* modelToReturn = nullptr;

		std::string modelLabel = "";
		if (Graphics::Model::ModelExists(currentModelID))
		{
			const std::string modelLabelFull = Graphics::Model::GetModel(currentModelID).GetPath();
			modelLabel = Utility::GetFileWithoutExtension(Utility::GetFileNameOnly(modelLabelFull));
		}

		if (ImGui::BeginCombo(comboID, modelLabel.c_str()))
		{
			auto& loadedModels = Graphics::Model::GetLoadedModels();

			for (auto& model : loadedModels)
			{
				const bool selected = currentModelID == model.second.GetID();

				std::string label = model.second.GetPath();
				label = Utility::GetFileWithoutExtension(Utility::GetFileNameOnly(label));
				if (ImGui::Selectable(label.c_str(), selected))
				{
					modelToReturn = &model.second;
				}

				if (selected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		return modelToReturn;
	}

	Lina::Graphics::Shader* WidgetsUtility::ShaderComboBox(const char* comboID, int currentShaderID)
	{
		Lina::Graphics::Shader* shaderToReturn = nullptr;

		std::string shaderLabel = "";
		if (Graphics::Shader::ShaderExists(currentShaderID))
		{
			const std::string shaderLabelFull = Graphics::Shader::GetShader(currentShaderID).GetPath();
			shaderLabel = Utility::GetFileWithoutExtension(Utility::GetFileNameOnly(shaderLabelFull));
		}


		if (ImGui::BeginCombo(comboID, shaderLabel.c_str()))
		{
			auto& loadedShaders = Lina::Graphics::Shader::GetLoadedShaders();

			for (auto& shader : loadedShaders)
			{
				const bool selected = currentShaderID == shader.second->GetID();

				std::string label = shader.second->GetPath();
				label = Utility::GetFileWithoutExtension(Utility::GetFileNameOnly(label));
				if (ImGui::Selectable(label.c_str(), selected))
				{
					shaderToReturn = shader.second;
				}

				if (selected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		return shaderToReturn;
	}

	void WidgetsUtility::Icon(const char* label, float scale, const ImVec4& color)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, color);
		PushScaledFont(scale);
		ImGui::Text(label);
		PopScaledFont();
		ImGui::PopStyleColor();
	}

	bool WidgetsUtility::IconButtonNoDecoration(const char* label, float width, float scale)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ChildBg));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::GetStyleColorVec4(ImGuiCol_ChildBg));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyleColorVec4(ImGuiCol_ChildBg));

		if (width != 0.0f)
			ImGui::SetNextItemWidth(width);

		PushScaledFont(scale);
		ImGui::Text(label);
		bool pressed = ImGui::IsItemClicked();
		PopScaledFont();

		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		return pressed;
	}


	bool WidgetsUtility::IconButton(const char* id, const char* label, float width, float scale, const ImVec4& color, const ImVec4& hoverColor, const ImVec4& pressedColor, bool disabled)
	{
		if (width != 0.0f)
			ImGui::SetNextItemWidth(width);

		bool isHovered = std::get<0>(s_iconButtons[id]);
		bool isPressed = std::get<1>(s_iconButtons[id]);

		if (!disabled)
		{
			Icon(label, scale, isPressed ? pressedColor : (isHovered ? hoverColor : color));
			bool pressed = ImGui::IsItemClicked();
			bool hovered = ImGui::IsItemHovered();
			bool beingPressed = hovered && ImGui::IsMouseDown(ImGuiMouseButton_Left);
			bool released = hovered && ImGui::IsMouseReleased(ImGuiMouseButton_Left);
			std::get<0>(s_iconButtons[id]) = hovered;
			std::get<1>(s_iconButtons[id]) = hovered && beingPressed;
			return released;
		}
		else
		{
			ImVec4 disabledColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
			Icon(label, scale, disabledColor);
			return false;
		}
		

	}

	bool WidgetsUtility::Button(const char* label, const ImVec2& size)
	{
		FrameRounding(2.0f);
		bool button = ImGui::Button(label, size);
		PopStyleVar();
		return button;
	}

}