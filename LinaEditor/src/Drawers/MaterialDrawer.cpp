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

#include "Drawers/MaterialDrawer.hpp"
#include "Core/Application.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/RenderEngine.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "Rendering/RenderingCommon.hpp"
#include "Core/EditorCommon.hpp"
#include "imgui/imgui.h"
#include "IconsFontAwesome5.h"

namespace LinaEditor
{


#define CURSORPOS_X_LABELS 30
#define CURSORPOS_XPERC_VALUES 0.52f

	void MaterialDrawer::SetSelectedMaterial(LinaEngine::Graphics::Material& mat)
	{
		m_selectedMaterial = &mat;
	}

	void MaterialDrawer::DrawSelectedMaterial()
	{
		float cursorPosValues = ImGui::GetWindowSize().x * CURSORPOS_XPERC_VALUES;
		float cursorPosLabels = CURSORPOS_X_LABELS;

		WidgetsUtility::IncrementCursorPos(ImVec2(11, 11));

		// Caret
		bool caretGeneral = WidgetsUtility::Caret("##matdraw_general");
		ImGui::SameLine();
		ImGui::AlignTextToFramePadding();
		WidgetsUtility::IncrementCursorPosY(-5);
		ImGui::Text("General Settings");
		ImGui::AlignTextToFramePadding();

		if (caretGeneral)
		{
			WidgetsUtility::IncrementCursorPosY(12);

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Uses HDRI");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			ImGui::Checkbox("##useshdri", &m_selectedMaterial->m_usesHDRI);

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Receives Lighting");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			ImGui::Checkbox("##receiveslighting", &m_selectedMaterial->m_receivesLighting);

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Is Shadow Mapped");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			ImGui::Checkbox("##isshadowmapped", &m_selectedMaterial->m_isShadowMapped);

			WidgetsUtility::FramePaddingX(4);
			const char* surfaceTypeLabel = LinaEngine::Graphics::g_materialSurfaceTypeStr[m_selectedMaterial->GetSurfaceType()];
			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Surface Type");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 12 - ImGui::GetCursorPosX());

			if (ImGui::BeginCombo("##surfaceType", surfaceTypeLabel))
			{
				for (int n = 0; n < IM_ARRAYSIZE(LinaEngine::Graphics::g_materialSurfaceTypeStr); n++)
				{
					const bool is_surface_selected = (m_selectedMaterial->GetSurfaceType() == n);
					if (ImGui::Selectable(LinaEngine::Graphics::g_materialSurfaceTypeStr[n], is_surface_selected))
					{
						m_selectedMaterial->SetSurfaceType((LinaEngine::Graphics::MaterialSurfaceType)n);
					}

					if (is_surface_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			WidgetsUtility::PopStyleVar();

			WidgetsUtility::FramePaddingX(4);
			const char* shaderLabel = LinaEngine::Graphics::g_shadersStr[m_selectedMaterial->GetShaderType()];
			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Shader");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 12 - ImGui::GetCursorPosX());

			if (ImGui::BeginCombo("##shaderType", shaderLabel))
			{
				for (int n = 0; n < IM_ARRAYSIZE(LinaEngine::Graphics::g_shadersStr); n++)
				{
					const bool is_shader_selected = (m_selectedMaterial->GetShaderType() == n);
					if (ImGui::Selectable(LinaEngine::Graphics::g_shadersStr[n], is_shader_selected))
					{
						LinaEngine::Graphics::Shaders st = (LinaEngine::Graphics::Shaders)n;
						std::cout << st << std::endl;
						LinaEngine::Application::GetRenderEngine().SetMaterialShader(*m_selectedMaterial, st);
					}

					if (is_shader_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			WidgetsUtility::PopStyleVar();
		}


		// Caret.
		WidgetsUtility::IncrementCursorPosX(11);
		WidgetsUtility::IncrementCursorPosY(11);
		bool caretFloats = WidgetsUtility::Caret("##matdraw_floats");
		ImGui::SameLine();
		ImGui::AlignTextToFramePadding();
		WidgetsUtility::IncrementCursorPosY(-5);
		ImGui::Text("Floats");
		ImGui::AlignTextToFramePadding();

		if (caretFloats)
		{
			WidgetsUtility::IncrementCursorPosY(11);

			for (std::map<std::string, float>::iterator it = m_selectedMaterial->m_floats.begin(); it != m_selectedMaterial->m_floats.end(); ++it)
			{
				WidgetsUtility::FramePaddingX(4);
				ImGui::SetCursorPosX(cursorPosLabels);
				WidgetsUtility::AlignedText(it->first.c_str());
				ImGui::SameLine();
				ImGui::SetCursorPosX(cursorPosValues);
				ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 12 - ImGui::GetCursorPosX());
				std::string label = "##f" + it->first;
				ImGui::DragFloat(label.c_str(), &it->second, 0.4f);
				WidgetsUtility::PopStyleVar();
			}
		}

		// Caret.
		WidgetsUtility::IncrementCursorPosX(11);
		WidgetsUtility::IncrementCursorPosY(11);
		bool caretInts = WidgetsUtility::Caret("##matdraw_ints");
		ImGui::SameLine();
		ImGui::AlignTextToFramePadding();
		WidgetsUtility::IncrementCursorPosY(-5);
		ImGui::Text("Ints");
		ImGui::AlignTextToFramePadding();

		if (caretInts)
		{
			WidgetsUtility::IncrementCursorPosY(11);

			for (std::map<std::string, int>::iterator it = m_selectedMaterial->m_ints.begin(); it != m_selectedMaterial->m_ints.end(); ++it)
			{
				WidgetsUtility::FramePaddingX(4);
				ImGui::SetCursorPosX(cursorPosLabels);
				WidgetsUtility::AlignedText(it->first.c_str());
				ImGui::SameLine();
				ImGui::SetCursorPosX(cursorPosValues);
				ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 12 - ImGui::GetCursorPosX());
				std::string label = "##i" + it->first;
				ImGui::DragInt(label.c_str(), &it->second, 0.4f);
				WidgetsUtility::PopStyleVar();
			}
		}

		// Caret.
		WidgetsUtility::IncrementCursorPosX(11);
		WidgetsUtility::IncrementCursorPosY(11);
		bool caretBools = WidgetsUtility::Caret("##matdraw_bools");
		ImGui::SameLine();
		ImGui::AlignTextToFramePadding();
		WidgetsUtility::IncrementCursorPosY(-5);
		ImGui::Text("Bools");
		ImGui::AlignTextToFramePadding();

		if (caretBools)
		{
			WidgetsUtility::IncrementCursorPosY(11);

			for (std::map<std::string, bool>::iterator it = m_selectedMaterial->m_bools.begin(); it != m_selectedMaterial->m_bools.end(); ++it)
			{
				WidgetsUtility::FramePaddingX(4);
				ImGui::SetCursorPosX(cursorPosLabels);
				WidgetsUtility::AlignedText(it->first.c_str());
				ImGui::SameLine();
				ImGui::SetCursorPosX(cursorPosValues);
				ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 12 - ImGui::GetCursorPosX());
				std::string label = "##i" + it->first;
				ImGui::Checkbox(label.c_str(), &it->second);
				WidgetsUtility::PopStyleVar();
			}
		}

		// Caret.
		WidgetsUtility::IncrementCursorPosX(11);
		WidgetsUtility::IncrementCursorPosY(11);
		bool caretColors = WidgetsUtility::Caret("##matdraw_Colors");
		ImGui::SameLine();
		ImGui::AlignTextToFramePadding();
		WidgetsUtility::IncrementCursorPosY(-5);
		ImGui::Text("Colors");
		ImGui::AlignTextToFramePadding();

		if (caretColors)
		{
			WidgetsUtility::IncrementCursorPosY(11);

			for (std::map<std::string, LinaEngine::Color>::iterator it = m_selectedMaterial->m_colors.begin(); it != m_selectedMaterial->m_colors.end(); ++it)
			{
				WidgetsUtility::FramePaddingX(4);
				ImGui::SetCursorPosX(cursorPosLabels);
				WidgetsUtility::AlignedText(it->first.c_str());
				ImGui::SameLine();
				ImGui::SetCursorPosX(cursorPosValues);
				ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 12 - ImGui::GetCursorPosX());
				std::string label = "##c" + it->first;
				WidgetsUtility::ColorButton(label.c_str(), &it->second.r);
				WidgetsUtility::PopStyleVar();
			}
		}

		// Caret.
		WidgetsUtility::IncrementCursorPosX(11);
		WidgetsUtility::IncrementCursorPosY(11);
		bool caretVector2s = WidgetsUtility::Caret("##matdraw_Vector2s");
		ImGui::SameLine();
		ImGui::AlignTextToFramePadding();
		WidgetsUtility::IncrementCursorPosY(-5);
		ImGui::Text("Vector2s");
		ImGui::AlignTextToFramePadding();

		if (caretVector2s)
		{
			WidgetsUtility::IncrementCursorPosY(11);

			for (std::map<std::string, LinaEngine::Vector2>::iterator it = m_selectedMaterial->m_vector2s.begin(); it != m_selectedMaterial->m_vector2s.end(); ++it)
			{
				WidgetsUtility::FramePaddingX(4);
				ImGui::SetCursorPosX(cursorPosLabels);
				WidgetsUtility::AlignedText(it->first.c_str());
				ImGui::SameLine();
				ImGui::SetCursorPosX(cursorPosValues);
				ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 12 - ImGui::GetCursorPosX());
				std::string label = "##v2" + it->first;
				ImGui::DragFloat2(label.c_str(), &it->second.x);
				WidgetsUtility::PopStyleVar();
			}
		}


		// Caret.
		WidgetsUtility::IncrementCursorPosX(11);
		WidgetsUtility::IncrementCursorPosY(11);
		bool caretVector3s = WidgetsUtility::Caret("##matdraw_Vector3s");
		ImGui::SameLine();
		ImGui::AlignTextToFramePadding();
		WidgetsUtility::IncrementCursorPosY(-5);
		ImGui::Text("Vector3s");
		ImGui::AlignTextToFramePadding();

		if (caretVector3s)
		{
			WidgetsUtility::IncrementCursorPosY(11);

			for (std::map<std::string, LinaEngine::Vector3>::iterator it = m_selectedMaterial->m_vector3s.begin(); it != m_selectedMaterial->m_vector3s.end(); ++it)
			{
				WidgetsUtility::FramePaddingX(4);
				ImGui::SetCursorPosX(cursorPosLabels);
				WidgetsUtility::AlignedText(it->first.c_str());
				ImGui::SameLine();
				ImGui::SetCursorPosX(cursorPosValues);
				ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 12 - ImGui::GetCursorPosX());
				std::string label = "##v3" + it->first;
				ImGui::DragFloat3(label.c_str(), &it->second.x);
				WidgetsUtility::PopStyleVar();
			}
		}

		// Caret.
		WidgetsUtility::IncrementCursorPosX(11);
		WidgetsUtility::IncrementCursorPosY(11);
		bool caretVector4s = WidgetsUtility::Caret("##matdraw_Vector4s");
		ImGui::SameLine();
		ImGui::AlignTextToFramePadding();
		WidgetsUtility::IncrementCursorPosY(-5);
		ImGui::Text("Vector4s");
		ImGui::AlignTextToFramePadding();

		if (caretVector4s)
		{
			WidgetsUtility::IncrementCursorPosY(11);

			for (std::map<std::string, LinaEngine::Vector4>::iterator it = m_selectedMaterial->m_vector4s.begin(); it != m_selectedMaterial->m_vector4s.end(); ++it)
			{
				WidgetsUtility::FramePaddingX(4);
				ImGui::SetCursorPosX(cursorPosLabels);
				WidgetsUtility::AlignedText(it->first.c_str());
				ImGui::SameLine();
				ImGui::SetCursorPosX(cursorPosValues);
				ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 12 - ImGui::GetCursorPosX());
				std::string label = "##v4" + it->first;
				ImGui::DragFloat4(label.c_str(), &it->second.x);
				WidgetsUtility::PopStyleVar();
			}
		}

		// Caret.
		WidgetsUtility::IncrementCursorPosX(11);
		WidgetsUtility::IncrementCursorPosY(11);
		bool caretTextures = WidgetsUtility::Caret("##matdraw_textures");
		ImGui::SameLine();
		ImGui::AlignTextToFramePadding();
		WidgetsUtility::IncrementCursorPosY(-5);
		ImGui::Text("Textures");
		ImGui::AlignTextToFramePadding();

		if (caretTextures)
		{
			WidgetsUtility::IncrementCursorPosY(11);

			for (std::map<std::string, LinaEngine::Graphics::MaterialSampler2D>::iterator it = m_selectedMaterial->m_sampler2Ds.begin(); it != m_selectedMaterial->m_sampler2Ds.end(); ++it)
			{
				WidgetsUtility::FramePaddingX(4);
				WidgetsUtility::IncrementCursorPosX(30);
				WidgetsUtility::AlignedText(it->first.c_str());

				ImVec2 min = ImVec2(ImGui::GetWindowPos().x + ImGui::GetCursorPos().x + 175, ImGui::GetWindowPos().y + ImGui::GetCursorPos().y - 50);
				ImVec2 max = ImVec2(min.x + 75, min.y + 75);
				ImGui::GetWindowDrawList()->AddRectFilled(min, max, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_FrameBg)), 5);

				ImVec2 minTexture = ImVec2(ImGui::GetWindowPos().x + ImGui::GetCursorPos().x + 180, ImGui::GetWindowPos().y + ImGui::GetCursorPos().y - 45);
				ImVec2 maxTexture = ImVec2(minTexture.x + 65, minTexture.y + 65);

				if (it->second.m_boundTexture != nullptr)
					ImGui::GetWindowDrawList()->AddImage((void*)it->second.m_boundTexture->GetID(), minTexture, maxTexture, ImVec2(0, 1), ImVec2(1, 0));

				WidgetsUtility::IncrementCursorPosY(80);
				WidgetsUtility::PopStyleVar();

				// Dropped on another title, swap component orders.
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(RESOURCES_MOVETEXTURE_ID))
					{
						IM_ASSERT(payload->DataSize == sizeof(uint32));
						m_selectedMaterial->SetTexture(it->first, &LinaEngine::Application::GetRenderEngine().GetTexture(*(uint32*)payload->Data), it->second.m_bindMode);
					}
					ImGui::EndDragDropTarget();
				}
			}
		}

		WidgetsUtility::IncrementCursorPosX(11);
		WidgetsUtility::IncrementCursorPosY(11);
		if (ImGui::Button("Apply Changes"))
		{
			LinaEngine::Graphics::Material::SaveMaterialData(*m_selectedMaterial, m_selectedMaterial->GetPath());
		}
	}
}
