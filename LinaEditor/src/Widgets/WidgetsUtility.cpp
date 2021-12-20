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
#include "Core/GUILayer.hpp"
#include "EventSystem/EventSystem.hpp"
#include "Core/WindowBackend.hpp"
#include "Core/EditorCommon.hpp"
#include "Math/Math.hpp"
#include "Math/Quaternion.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/Model.hpp"
#include "Rendering/Shader.hpp"
#include "Physics/PhysicsMaterial.hpp"
#include "Drawers/ComponentDrawer.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "Core/CustomFontIcons.hpp"
#include "Core/PhysicsCommon.hpp"
#include "IconsFontAwesome5.h"

namespace Lina::Editor
{
	std::map<std::string, std::tuple<bool, bool>> WidgetsUtility::s_iconButtons;
	std::map<std::string, float> WidgetsUtility::s_debugFloats;
	std::map<std::string, bool> WidgetsUtility::s_carets;

	void WidgetsUtility::Tooltip(const char* tooltip)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(9, 2));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
		ImGui::BeginTooltip();
		ImGui::Text(tooltip);
		ImGui::EndTooltip();
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
	}

	bool WidgetsUtility::CustomButton(const char* id, ImVec2 size, bool* isHovered, bool locked, const char* icon, float rounding, const char* tooltip)
	{
		const ImVec2 currentCursor = ImGui::GetCursorPos();
		const ImVec2 currentPos = ImVec2(ImGui::GetWindowPos().x + ImGui::GetCursorPosX(), ImGui::GetWindowPos().y + ImGui::GetCursorPos().y);
		const ImRect absoluteRect = ImRect(ImVec2(currentPos.x, currentPos.y), ImVec2(size.x + currentPos.x, size.y + currentPos.y));
		
		ImGui::ItemAdd(absoluteRect, ImGuiID(id));
		ImGui::ItemSize(size);

		bool hovered = ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(absoluteRect.Min, absoluteRect.Max) && !ImGui::IsAnyItemHovered();
		if (isHovered != nullptr)
			*isHovered = hovered;
		
		const bool pressing = hovered && ImGui::IsMouseDown(ImGuiMouseButton_Left);

		const ImVec4 normalColor = ImGui::GetStyleColorVec4(ImGuiCol_Button);
		const ImVec4 lockedColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonLocked);
		const ImVec4 hoverColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered);
		const ImVec4 rectCol = locked ? lockedColor : pressing ? normalColor : (hovered ? hoverColor : normalColor);
		const ImU32 borderColor = ImGui::ColorConvertFloat4ToU32(ImVec4(0.3f, 0.3f, 0.3f, 0.55f));
		ImGui::GetWindowDrawList()->AddRectFilled(absoluteRect.Min, absoluteRect.Max, ImGui::ColorConvertFloat4ToU32(rectCol), rounding);
		IncrementCursorPosY(size.y / 2.0f);
	
		if (icon != nullptr)
		{
			const float iconScale = size.y * 0.035f;
			PushScaledFont(iconScale);
			ImVec2 textSize = ImGui::CalcTextSize(icon);
		
			ImGui::SameLine();
			ImGui::SetCursorPosX(currentCursor.x + size.x / 2.0f - textSize.x / 2.0f);
			ImGui::SetCursorPosY(currentCursor.y + size.y / 2.0f - size.y * 0.2f);
			ImGui::Text(icon);
			PopScaledFont();
		}

		if (tooltip != nullptr && hovered)
			Tooltip(tooltip);

		if (!locked && ImGui::IsMouseReleased(ImGuiMouseButton_Left) && hovered)
			return true;

		return false;
	}

	bool WidgetsUtility::CustomToggle(const char* id, ImVec2 size, bool toggled, bool* hoveredPtr, const char* icon, float rounding, const char* tooltip)
	{
		const ImVec2 currentCursor = ImGui::GetCursorPos();
		const ImVec2 currentPos = ImVec2(ImGui::GetWindowPos().x + ImGui::GetCursorPosX(), ImGui::GetWindowPos().y + ImGui::GetCursorPos().y);
		const ImRect absoluteRect = ImRect(ImVec2(currentPos.x, currentPos.y), ImVec2(size.x + currentPos.x, size.y + currentPos.y));

		ImGui::ItemAdd(absoluteRect, ImGuiID(id));
		ImGui::ItemSize(size);

		bool hovered = ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(absoluteRect.Min, absoluteRect.Max) && !ImGui::IsAnyItemHovered();
		const bool pressing = hovered && ImGui::IsMouseDown(ImGuiMouseButton_Left);

		if (hoveredPtr != nullptr)
			*hoveredPtr = hovered;
		const ImVec4 normalColor = ImGui::GetStyleColorVec4(ImGuiCol_Button);
		const ImVec4 lockedColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonLocked);
		const ImVec4 hoverColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered);
		const ImVec4 rectCol = pressing ? normalColor : (hovered ? hoverColor : toggled ? lockedColor : normalColor);
		const ImU32 borderColor = ImGui::ColorConvertFloat4ToU32(ImVec4(0.3f, 0.3f, 0.3f, 0.55f));
		ImGui::GetWindowDrawList()->AddRectFilled(absoluteRect.Min, absoluteRect.Max, ImGui::ColorConvertFloat4ToU32(rectCol), rounding);
		IncrementCursorPosY(size.y / 2.0f);

		if (icon != nullptr)
		{
			const float iconScale = size.y * 0.035f;
			PushScaledFont(iconScale);
			ImVec2 textSize = ImGui::CalcTextSize(icon);

			ImGui::SameLine();
			ImGui::SetCursorPosX(currentCursor.x + size.x / 2.0f - textSize.x / 2.0f);
			ImGui::SetCursorPosY(currentCursor.y + size.y / 2.0f - size.y * 0.2f);
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

	void WidgetsUtility::DrawTreeFolder(Utility::Folder& folder, Utility::Folder*& selectedFolder, Utility::Folder*& hoveredFolder, float height, float offset, ImVec4 defaultBackground, ImVec4 hoverBackground, ImVec4 selectedBackground)
	{

		if (ColorsEqual(hoverBackground, ImVec4(0, 0, 0, 0)))
			hoverBackground = ImGui::GetStyleColorVec4(ImGuiCol_HeaderHovered);

		if (ColorsEqual(selectedBackground, ImVec4(0, 0, 0, 0)))
			selectedBackground = ImGui::GetStyleColorVec4(ImGuiCol_Header);


		ImVec2 pos = ImGui::GetCurrentWindow()->Pos;
		ImVec2 size = ImGui::GetCurrentWindow()->Size;
		ImVec2 min = ImVec2(ImGui::GetCurrentWindow()->Pos.x, -ImGui::GetScrollY() + ImGui::GetCursorPosY() + ImGui::GetCurrentWindow()->Pos.y);
		ImVec2 max = ImVec2(min.x + size.x, min.y + height);

		// Hover state.
		if (ImGui::IsMouseHoveringRect(min, max) && ImGui::IsWindowFocused())
			hoveredFolder = &folder;

		bool hovered = hoveredFolder == &folder;
		bool selected = selectedFolder == &folder;
		bool open = folder.m_isOpen;

		// Color & selection
		ImVec4 background = selected ? selectedBackground : (hovered ? hoverBackground : defaultBackground);
		ImVec4 usedBackground = background;
		if (hovered)
		{
			if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
			{
				usedBackground = ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive);
			}
			if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			{
				selectedFolder = &folder;
			}
			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				selectedFolder = &folder;
			}
		}

		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered())
				selectedFolder = nullptr;
		}


		// Background
		ImGui::GetWindowDrawList()->AddRectFilled(min, max, ImGui::ColorConvertFloat4ToU32(usedBackground));

		if (hovered && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			folder.m_isOpen = !folder.m_isOpen;

		// Draw the folder data
		PushScaledFont(0.7f);

		IncrementCursorPosY(6);
		ImGui::SetCursorPosX(4 + offset);

		if (folder.m_folders.size() != 0)
		{
			if (selected)
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
			else
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));

			if (open)
				ImGui::Text(ICON_FA_CARET_DOWN);
			else
				ImGui::Text(ICON_FA_CARET_RIGHT);

			ImGui::PopStyleColor();


			if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
				folder.m_isOpen = !folder.m_isOpen;

			ImGui::SameLine();
		}

		IncrementCursorPosY(0);
		PopScaledFont();
		ImGui::SetCursorPosX(4 + offset + 12);
		Icon(ICON_FA_FOLDER, 0.7f, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));

		if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
			folder.m_isOpen = !folder.m_isOpen;

		ImGui::SameLine();
		IncrementCursorPosY(-5);
		ImGui::Text(folder.m_name.c_str());



		//	ImGui::EndChild();
		//	ImGui::PopStyleVar();
	}

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

	bool WidgetsUtility::ToggleButton(const char* label, bool* v, float heightMultiplier, float widthMultiplier, const ImVec4& activeColor, const ImVec4& activeHoveredColor, const ImVec4& inactiveColor, const ImVec4& inactiveHoveredColor)
	{
		ImVec2 p = ImGui::GetCursorScreenPos();
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		float height = ImGui::GetFrameHeight() * heightMultiplier;
		float width = height * 1.55f * widthMultiplier;
		float radius = height * 0.50f;

		ImGui::InvisibleButton(label, ImVec2(width, height));
		if (v != nullptr && ImGui::IsItemClicked())
			*v = !*v;

		float t = v == nullptr ? 1.0f : *v ? 1.0f : 0.0f;

		ImVec4 usedActiveColor = activeColor;
		if (v == nullptr)
			usedActiveColor = ImVec4(activeColor.x - 0.15f, activeColor.y - 0.15f, activeColor.z - 0.15f, activeColor.w);

		ImGuiContext& g = *GImGui;
		float ANIM_SPEED = 0.08f;
		if (v != nullptr && g.LastActiveId == g.CurrentWindow->GetID(label))// && g.LastActiveIdTimer < ANIM_SPEED)
		{
			float t_anim = ImSaturate(g.LastActiveIdTimer / ANIM_SPEED);
			t = *v ? (t_anim) : (1.0f - t_anim);
		}

		ImU32 col_bg;
		if (ImGui::IsItemHovered() && v != nullptr)
			col_bg = ImGui::GetColorU32(ImLerp(inactiveHoveredColor, activeHoveredColor, t));
		else
			col_bg = ImGui::GetColorU32(ImLerp(inactiveColor, usedActiveColor, t));

		draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), col_bg, height * 0.5f);
		draw_list->AddCircleFilled(ImVec2(p.x + radius + t * (width - radius * 2.0f), p.y + radius), radius - 1.5f, IM_COL32(255, 255, 255, 255));

		return v != nullptr ? *v : false;
	}

	void WidgetsUtility::HorizontalDivider(float yOffset, float thickness)
	{
		ImVec2 min = ImVec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + ImGui::GetCursorPosY() + yOffset);
		ImVec2 max = ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowWidth(), min.y);
		ImGui::GetWindowDrawList()->AddLine(min, max, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_Text)), thickness);
	}

	void WidgetsUtility::WindowButtons(const char* windowID, float yOffset, bool isAppWindow)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));

		ImVec2 windowPos = ImGui::GetWindowPos();
		float windowWidth = ImGui::GetWindowWidth();

		static float offset1 = 28.0f;
		static float gap = 25;
		
		ImGui::SetCursorPosY(yOffset);
		const float cursorY = ImGui::GetCursorPosY();
		ImGui::SetCursorPosX(windowWidth - offset1 - gap * 2);
		if (WidgetsUtility::CustomButton("##header_minimize", ImVec2(20, 20), nullptr, false, ICON_FA_WINDOW_MINIMIZE, 2.0f))
		{
			if (isAppWindow)
				Lina::Graphics::WindowBackend::Get()->Iconify();
			else
				GUILayer::s_editorPanels[windowID]->ToggleCollapse();
		}

		ImGui::SameLine();
		ImGui::SetCursorPosX(windowWidth - offset1 - gap);
		ImGui::SetCursorPosY(cursorY);
		if (WidgetsUtility::CustomButton("##header_maximize", ImVec2(20, 20), nullptr, false, ICON_FA_WINDOW_MAXIMIZE, 2.0f))
		{
			if (isAppWindow)
				Lina::Graphics::WindowBackend::Get()->Maximize();
			else
				GUILayer::s_editorPanels[windowID]->ToggleMaximize();
		}

		ImGui::SameLine();
		ImGui::SetCursorPosX(windowWidth - offset1);
		ImGui::SetCursorPosY(cursorY);
		if (WidgetsUtility::CustomButton("##header_minimize", ImVec2(20, 20), nullptr, false, ICON_FA_TIMES, 2.0f))
		{
			if (isAppWindow)
				Lina::Graphics::WindowBackend::Get()->Close();
			else
				GUILayer::s_editorPanels[windowID]->Close();
		}

		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
	}

	void WidgetsUtility::WindowTitlebar(const char* label)
	{
		if (ImGui::IsWindowDocked()) return;

		const ImVec2 windowPos = ImGui::GetWindowPos();
		const ImVec2 windowSize = ImGui::GetWindowSize();
		const float height = 30.0f;
		const float lineOffset = 2.0f;

		// Draw title rect & line.
		ImRect titleRect = ImRect(windowPos, ImVec2(windowPos.x + windowSize.x, windowPos.y + height));
		ImVec4 titleRectColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
		ImGui::GetWindowDrawList()->AddRectFilled(titleRect.Min, titleRect.Max, ImGui::ColorConvertFloat4ToU32(titleRectColor));
		ImGui::GetWindowDrawList()->AddLine(ImVec2(windowPos.x, windowPos.y + lineOffset), ImVec2(windowPos.x + windowSize.x, windowPos.y + lineOffset), ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_Header)), 4.0f);

		// Draw title
		ImGui::SetCursorPosX(12.5f);
		ImGui::Text(label);

		// Draw Buttons
		WindowButtons(label, 5.0f);
	}

	bool WidgetsUtility::ComponentHeader(Lina::ECS::TypeID tid, bool* foldoutOpen, const char* componentLabel, const char* componentIcon, bool* toggled, bool* removed, bool* copied, bool* pasted, bool* resetted, bool moveButton)
	{
		const ImVec2 windowSize = ImGui::GetWindowSize();
		const ImVec2 rectSize = ImVec2(windowSize.x, 25);
		const ImVec4 normalColor = ImVec4(0.03f, 0.03f, 0.03f, 1.0f);
		const ImVec4 hoverColor = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
		const float iconScale = 0.65f;

		ImGui::PushStyleColor(ImGuiCol_Button, normalColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoverColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, normalColor);
		const std::string componentHeaderID = std::to_string(static_cast<uint32>(tid));
		bool isPressed = CustomButton(componentHeaderID.c_str(), rectSize);
		bool hovered = ImGui::IsItemHovered();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::SameLine();

		// Horizontal dividers as borders.
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 0.3f, 0.3f, 0.55f));
		HorizontalDivider();
		HorizontalDivider(rectSize.y);
		ImGui::PopStyleColor();

		const float caretPos = 6;
		const float labelPos = -4;
		const float iconPos = 3;
		const float togglePos = -1.7f;
		const float moveHandlePos = 2;
		const float buttonOffsetFromSize1 = 36.0f;
		const float buttonOffsetFromSize2 = 34.0f;
		const float buttonWidth = 29.0f;
		const float buttonHeight = 18.0f;
		const float buttonRounding = 2.0f;
		float buttonsPos = -4.2f;

		// Caret
		ImGui::SetCursorPosX(12);
		IncrementCursorPosY(caretPos);
		ImGui::Text(*foldoutOpen ? ICON_FA_CARET_DOWN : ICON_FA_CARET_RIGHT);

		// Label
		ImGui::SameLine();
		IncrementCursorPosY(labelPos);
		ImGui::Text(componentLabel);

		// Icon
		if (componentIcon != nullptr)
		{
			ImGui::SameLine();
			PushScaledFont(iconScale);
			IncrementCursorPosY(3);
			ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_Header));
			IncrementCursorPosY(iconPos);
			ImGui::Text(componentIcon);
			ImGui::PopStyleColor();
			PopScaledFont();
		}

		// Toggle
		bool toggleButtonHovered = false;
		if (toggled != nullptr)
		{
			ImGui::SameLine();
			IncrementCursorPosY(togglePos);
			std::string toggleID = "#tb_" + std::string(componentLabel);
			ToggleButton(toggleID.c_str(), toggled, 0.6f, 1.5f, ImGui::GetStyleColorVec4(ImGuiCol_Header), ImGui::GetStyleColorVec4(ImGuiCol_Header));
			toggleButtonHovered = ImGui::IsItemHovered();
		}
		else 
			buttonsPos += 4.0f;


		bool wasDraggedDropped = false;
		PushScaledFont(iconScale);

		if (moveButton)
		{
			// Move
			ImGui::SameLine();
			IncrementCursorPosY(moveHandlePos);
			ImGui::Text(ICON_FA_BARS);

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
					IM_ASSERT(payload->DataSize == sizeof(Lina::ECS::TypeID));
					Lina::ECS::TypeID payloadID = *(const Lina::ECS::TypeID*)payload->Data;
					Lina::Event::EventSystem::Get()->Trigger<EComponentOrderSwapped>(EComponentOrderSwapped{ payloadID, tid });
				}

				wasDraggedDropped = true;

				ImGui::EndDragDropTarget();
			}
		}
		else
			buttonsPos += 2.0f;


		ImGui::SameLine();
		IncrementCursorPosY(buttonsPos);

		std::vector<std::pair<const char*, bool*>> buttons;
		std::vector<std::string> buttonNames{ "Remove", "Paste", "Copy", "Reset" };

		buttons.push_back(std::make_pair(ICON_FA_TIMES, removed));
		buttons.push_back(std::make_pair(ICON_FA_PASTE, pasted));
		buttons.push_back(std::make_pair(ICON_FA_COPY, copied));
		buttons.push_back(std::make_pair(ICON_FA_SYNC_ALT, resetted));

		bool anyButtonRectHovered = false;
		float currentCursorPosY = ImGui::GetCursorPosY();
		for (int i = 0; i < buttons.size(); i++)
		{
			ImGui::SetCursorPosY(currentCursorPosY);

			if (i == 0)
				ImGui::SetCursorPosX(windowSize.x - buttonOffsetFromSize2);
			else
				ImGui::SetCursorPosX(windowSize.x - buttonOffsetFromSize2 - (i * buttonOffsetFromSize1));

			bool locked = buttons[i].second == nullptr;

			ImVec4 buttonLockedColor = ImVec4(0.06f, 0.06f, 0.06f, 1.0f);
			ImVec4 buttonNormalColor = ImVec4(0.15f, 0.15f, 0.15f, 0.8f);
			ImVec4 buttonHoverColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
			//ImGui::PushStyleColor(ImGuiCol_Button, buttonNormalColor);
			//ImGui::PushStyleColor(ImGuiCol_ButtonLocked, buttonLockedColor);
			//ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonHoverColor);
			const std::string id = componentHeaderID + std::to_string(i);
			bool buttonHovered = false;
			bool buttonPressed = CustomButton(id.c_str(), ImVec2(buttonWidth, buttonHeight), &buttonHovered, locked, buttons[i].first, buttonRounding, buttonNames[i].c_str());
			//ImGui::PopStyleColor();
			//ImGui::PopStyleColor();
			//ImGui::PopStyleColor();

			if (!locked && buttonHovered && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
				*(buttons[i].second) = true;

			if (i != buttons.size() - 1)
				ImGui::SameLine();

			if (buttonHovered)
			{
				anyButtonRectHovered = true;
			}
		}


		PopScaledFont();

		if (!wasDraggedDropped && !anyButtonRectHovered && !toggleButtonHovered && isPressed)
			*foldoutOpen = !(*foldoutOpen);

		return *foldoutOpen;
	}

	bool WidgetsUtility::Header(const char* label, bool* foldoutOpen)
	{
		const ImVec2 windowSize = ImGui::GetWindowSize();
		const ImVec2 rectSize = ImVec2(windowSize.x, 25);
		const ImVec4 normalColor = ImVec4(0.03f, 0.03f, 0.03f, 1.0f);
		const ImVec4 hoverColor = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
		const float iconScale = 0.65f;

		ImGui::PushStyleColor(ImGuiCol_Button, normalColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoverColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, normalColor);

		if (CustomButton(label, rectSize))
			*foldoutOpen = !*foldoutOpen;

		bool hovered = ImGui::IsItemHovered();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::SameLine();

		// Horizontal dividers as borders.
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 0.3f, 0.3f, 0.55f));
		HorizontalDivider();
		HorizontalDivider(rectSize.y);
		ImGui::PopStyleColor();

		static float caretPos = 6;
		static float labelPos = -4;

		// Caret
		ImGui::SetCursorPosX(12);
		IncrementCursorPosY(caretPos);
		ImGui::Text(*foldoutOpen ? ICON_FA_CARET_DOWN : ICON_FA_CARET_RIGHT);

		// Label
		ImGui::SameLine();
		IncrementCursorPosY(labelPos);
		ImGui::Text(label);

		return *foldoutOpen;
	}

	void WidgetsUtility::DropShadow()
	{
		static int countS = 4;
		static float thicknessS = 4.0f;
		static float yOffsetS = 3.0f;

		ImVec4 color = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
		int dividerCount = 4;
		float dividerSize = 4.0f;
		float yOffset = 3.0f;
		float colorFade = 1.0f / (float)dividerCount;

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
		bool clicked = false;
		bool hovered = false;
		ImVec4 caretNormalColor = ImVec4(0.75f, 0.75f, 0.75f, 1.0f);
		ImVec4 caretHoverColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		ImVec4 caretColor = hovered ? caretHoverColor : caretNormalColor;

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
		WidgetsUtility::AlignedText(label);

		if (sameLine)
		{
			ImGui::SameLine();
			ImGui::SetCursorPosX(CURSOR_X_VALUES);
		}
	}

	Lina::Graphics::Material* WidgetsUtility::MaterialComboBox(const char* comboID, const std::string& currentPath, bool* removed)
	{
		Graphics::Material* materialToReturn = nullptr;

		std::string materialLabel = "";
		if (Graphics::Material::MaterialExists(currentPath))
		{
			materialLabel = Utility::GetFileWithoutExtension(Utility::GetFileNameOnly(currentPath));
		}


		float currentCursor = ImGui::GetCursorPosX();
		float windowWidth = ImGui::GetWindowWidth();
		float remaining = windowWidth - currentCursor;
		float comboWidth = removed == nullptr ? remaining - 12.4f : remaining - 28.0f;
		ImGui::SetNextItemWidth(comboWidth);

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

		if (removed != nullptr)
		{
			ImGui::SameLine();
			ImGui::SetCursorPosX(windowWidth - 22.4f);
			WidgetsUtility::IncrementCursorPosY(6);

			// Remove Model
			const std::string removeID = "##removeMaterial_" + std::string(comboID);
			if (WidgetsUtility::IconButton(removeID.c_str(), ICON_FA_MINUS_SQUARE, 0.0f, .7f, ImVec4(1, 1, 1, 0.8f), ImVec4(1, 1, 1, 1), ImGui::GetStyleColorVec4(ImGuiCol_Header)))
				*removed = true;


			if (ImGui::IsItemHovered())
				Tooltip("Remove");

		}

		return materialToReturn;
	}

	Lina::Graphics::Model* WidgetsUtility::ModelComboBox(const char* comboID, int currentModelID, bool* removed)
	{
		Lina::Graphics::Model* modelToReturn = nullptr;

		std::string modelLabel = "";
		if (Graphics::Model::ModelExists(currentModelID))
		{
			const std::string modelLabelFull = Graphics::Model::GetModel(currentModelID).GetPath();
			modelLabel = Utility::GetFileWithoutExtension(Utility::GetFileNameOnly(modelLabelFull));
		}

		float currentCursor = ImGui::GetCursorPosX();
		float windowWidth = ImGui::GetWindowWidth();
		float remaining = windowWidth - currentCursor;
		float comboWidth = removed == nullptr ? remaining - 12.4f : remaining - 28.0f;

		ImGui::SetNextItemWidth(comboWidth);
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

		if (removed != nullptr)
		{
			ImGui::SameLine();
			ImGui::SetCursorPosX(windowWidth - 22.4f);
			WidgetsUtility::IncrementCursorPosY(6);

			// Remove
			const std::string removeID = "##removeModel_" + std::string(comboID);
			if (WidgetsUtility::IconButton(removeID.c_str(), ICON_FA_MINUS_SQUARE, 0.0f, .7f, ImVec4(1, 1, 1, 0.8f), ImVec4(1, 1, 1, 1), ImGui::GetStyleColorVec4(ImGuiCol_Header)))
				*removed = true;

			if (ImGui::IsItemHovered())
				Tooltip("Remove");
		}

		return modelToReturn;
	}

	Lina::Graphics::Shader* WidgetsUtility::ShaderComboBox(const char* comboID, int currentShaderID, bool* removed)
	{
		Lina::Graphics::Shader* shaderToReturn = nullptr;

		std::string shaderLabel = "";
		if (Graphics::Shader::ShaderExists(currentShaderID))
		{
			const std::string shaderLabelFull = Graphics::Shader::GetShader(currentShaderID).GetPath();
			shaderLabel = Utility::GetFileWithoutExtension(Utility::GetFileNameOnly(shaderLabelFull));
		}


		float currentCursor = ImGui::GetCursorPosX();
		float windowWidth = ImGui::GetWindowWidth();
		float remaining = windowWidth - currentCursor;
		float comboWidth = removed == nullptr ? remaining - 12.4f : remaining - 28.0f;
		if (ImGui::BeginCombo(comboID, shaderLabel.c_str()))
		{
			auto& loadedShaders = Lina::Graphics::Shader::GetLoadedShaders();

			for (auto& shader : loadedShaders)
			{
				const bool selected = currentShaderID == shader.second->GetSID();

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

		if (removed != nullptr)
		{
			ImGui::SameLine();
			ImGui::SetCursorPosX(windowWidth - 22.4f);
			WidgetsUtility::IncrementCursorPosY(6);

			// Remove Model
			const std::string removeID = "##removeShader_" + std::string(comboID);
			if (WidgetsUtility::IconButton(removeID.c_str(), ICON_FA_MINUS_SQUARE, 0.0f, .7f, ImVec4(1, 1, 1, 0.8f), ImVec4(1, 1, 1, 1), ImGui::GetStyleColorVec4(ImGuiCol_Header)))
				*removed = true;

			if (ImGui::IsItemHovered())
				Tooltip("Remove");
		}


		return shaderToReturn;
	}

	Lina::Physics::PhysicsMaterial* Lina::Editor::WidgetsUtility::PhysicsMaterialComboBox(const char* comboID, const std::string& currentPath, bool* removed)
	{
		Physics::PhysicsMaterial* materialToReturn = nullptr;

		std::string materialLabel = "";
		if (Physics::PhysicsMaterial::MaterialExists(currentPath))
		{
			materialLabel = Utility::GetFileWithoutExtension(Utility::GetFileNameOnly(currentPath));
		}


		float currentCursor = ImGui::GetCursorPosX();
		float windowWidth = ImGui::GetWindowWidth();
		float remaining = windowWidth - currentCursor;
		float comboWidth = removed == nullptr ? remaining - 12.4f : remaining - 28.0f;
		ImGui::SetNextItemWidth(comboWidth);

		if (ImGui::BeginCombo(comboID, materialLabel.c_str()))
		{
			auto& loadedMaterials = Physics::PhysicsMaterial::GetLoadedMaterials();

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

		if (removed != nullptr)
		{
			ImGui::SameLine();
			ImGui::SetCursorPosX(windowWidth - 22.4f);
			WidgetsUtility::IncrementCursorPosY(6);

			// Remove Model
			const std::string removeID = "##removeMaterial_" + std::string(comboID);
			if (WidgetsUtility::IconButton(removeID.c_str(), ICON_FA_MINUS_SQUARE, 0.0f, .7f, ImVec4(1, 1, 1, 0.8f), ImVec4(1, 1, 1, 1), ImGui::GetStyleColorVec4(ImGuiCol_Header)))
				*removed = true;


			if (ImGui::IsItemHovered())
				Tooltip("Remove");

		}

		return materialToReturn;
	}

	int Lina::Editor::WidgetsUtility::CollisionShapeComboBox(const char* comboID, int currentShapeID)
	{
		int shapeToReturn = currentShapeID;

		float currentCursor = ImGui::GetCursorPosX();
		float windowWidth = ImGui::GetWindowWidth();
		float remaining = windowWidth - currentCursor;
		float comboWidth = remaining - 12.4f;
		if (ImGui::BeginCombo(comboID, Lina::Physics::COLLISION_SHAPES[currentShapeID].c_str()))
		{
			int counter = 0;
			for (auto& shape : Lina::Physics::COLLISION_SHAPES)
			{
				const bool selected = currentShapeID == counter;

				if (ImGui::Selectable(Lina::Physics::COLLISION_SHAPES[counter].c_str(), selected))
					shapeToReturn = counter;

				if (selected)
					ImGui::SetItemDefaultFocus();

				counter++;
			}

			ImGui::EndCombo();
		}

		return shapeToReturn;
	}

	bool WidgetsUtility::Button(const char* label, const ImVec2& size)
	{
		FrameRounding(4.0f);
		bool button = ImGui::Button(label, size);
		PopStyleVar();
		return button;
	}

	bool WidgetsUtility::ToolbarToggleIcon(const char* label, const ImVec2 size, int imagePadding, bool toggled, float cursorPosY, const std::string& tooltip, ImVec4 color, float scale)
	{
		ImGui::SetCursorPosY(cursorPosY);
		ImVec2 windowPos = ImGui::GetWindowPos();
		ImVec2 min = ImVec2(windowPos.x + ImGui::GetCursorPosX(), windowPos.y + ImGui::GetCursorPosY());
		ImVec2 max = ImVec2(windowPos.x + ImGui::GetCursorPosX() + size.x, windowPos.y + ImGui::GetCursorPosY() + size.y);

		bool hovered = ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(min, max);
		bool pressed = hovered && ImGui::IsMouseDown(ImGuiMouseButton_Left);

		ImVec4 toggledColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonLocked);
		ImVec4 hoveredColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered);
		ImVec4 pressedColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive);
		ImVec4 defaultColor = ImGui::GetStyleColorVec4(ImGuiCol_Button);
		ImVec4 col = toggled ? toggledColor : (pressed ? pressedColor : (hovered ? hoveredColor : defaultColor));
		ImGui::GetWindowDrawList()->AddRectFilled(min, max, ImGui::ColorConvertFloat4ToU32(col), 4.0f);

		if (tooltip.compare("") != 0 && hovered)
		{
			Tooltip(tooltip.c_str());
		}

		const float yIncrement = size.y / 4.0f + 1;
		const float currentCursorPos = ImGui::GetCursorPosY();
		IncrementCursorPosY(yIncrement);
		IncrementCursorPosX(size.x / 4.0f + 1);
		PushScaledFont(scale);
		ImGui::PushStyleColor(ImGuiCol_Text, color);
		ImGui::Text(label);
		ImGui::PopStyleColor();
		PopScaledFont();

		if (hovered && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			return true;

		return false;
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
			//ret = ImGui::TempInputText(window->Rect, id, "##Input", buf, (int)buf_size, ImGuiInputTextFlags_None);
			window->DC.CursorPos = pos_after;
		}
		else
		{
			window->DrawList->AddText(pos_before, ImGui::GetColorU32(ImGuiCol_Text), buf);
		}

		ImGui::PopID();
		return ret;
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

	bool WidgetsUtility::CaretAndLabel(const char* title, const char* label)
	{
		const char* caret = s_carets[title] ? ICON_FA_CARET_DOWN : ICON_FA_CARET_RIGHT;
		if (WidgetsUtility::IconButtonNoDecoration(caret, 30, 0.8f))
			s_carets[title] = !s_carets[title];

		ImGui::SameLine();
		ImGui::Text(label);
		if (ImGui::IsItemClicked())
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

	ImVec2 WidgetsUtility::GetWindowPosWithContentRegion()
	{
		return ImVec2(ImGui::GetWindowContentRegionMin().x + ImGui::GetWindowPos().x, ImGui::GetWindowContentRegionMin().y + ImGui::GetWindowPos().y);
	}

	ImVec2 WidgetsUtility::GetWindowSizeWithContentRegion()
	{
		float yDiff = ImGui::GetWindowPos().y - WidgetsUtility::GetWindowPosWithContentRegion().y;
		return ImVec2(ImGui::GetWindowContentRegionWidth(), ImGui::GetWindowSize().y + yDiff);
	}


}