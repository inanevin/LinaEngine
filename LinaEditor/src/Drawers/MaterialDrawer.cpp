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
#include "Core/EditorApplication.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/RenderEngine.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "Rendering/RenderingCommon.hpp"
#include "Core/EditorCommon.hpp"
#include "Modals/SelectShaderModal.hpp"
#include "Rendering/Shader.hpp"
#include "Utility/EditorUtility.hpp"
#include "PackageManager/Generic/GenericMemory.hpp"
#include "imgui/imgui.h"
#include "IconsFontAwesome5.h"

namespace LinaEditor
{

#define CURSORPOS_X_LABELS 30
#define CURSORPOS_XPERC_VALUES 0.52f

	const char* excludeProperties[]
	{
		"material.irradianceMap",
		"material.prefilterMap",
		"material.brdfLUTMap",
		"material.shadowMap",
		"material.pointShadowDepthMaps"
	};


	void MaterialDrawer::SetSelectedMaterial(EditorFile* file, LinaEngine::Graphics::Material& mat)
	{
		m_selectedMaterial = &mat;
		m_selectedFile = file;
		m_shouldCopyMatName = true;
	}

	void MaterialDrawer::DrawSelectedMaterial()
	{
		float cursorPosValues = ImGui::GetWindowSize().x * CURSORPOS_XPERC_VALUES;
		float cursorPosLabels = CURSORPOS_X_LABELS;

		WidgetsUtility::IncrementCursorPosY(12);
		ImGui::SetCursorPosX(12);

		// Setup char.
		static char materialName[64] = "";
		if (m_shouldCopyMatName)
		{
			m_shouldCopyMatName = false;
			memset(materialName, 0, sizeof materialName);
			std::string extensionlessStr = EditorUtility::RemoveExtensionFromFilename(m_selectedFile->m_name);
			std::copy(extensionlessStr.begin(), extensionlessStr.end(), materialName);
		}

		// Entity name input text.
		WidgetsUtility::FramePaddingX(5);
		WidgetsUtility::IncrementCursorPosY(-5);
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - ImGui::GetCursorPosX() - 56);
		ImGui::InputText("##matname", materialName, IM_ARRAYSIZE(materialName));
		if (EditorUtility::RemoveExtensionFromFilename(m_selectedFile->m_name).compare(materialName) != 0)
		{
			if (std::string(materialName).find(".") == std::string::npos)
			{
				std::string newName = std::string(materialName) + ".mat";
				EditorUtility::ChangeFilename(m_selectedFile->m_pathToFolder.c_str(), m_selectedFile->m_name.c_str(), newName.c_str());
				m_selectedFile->m_name = newName;
				
				GenericMemory::memset(materialName, 0, 64);
				std::string extensionlessStr = EditorUtility::RemoveExtensionFromFilename(m_selectedFile->m_name);
				std::copy(extensionlessStr.begin(), extensionlessStr.end(), materialName);

				m_selectedFile->m_path = m_selectedFile->m_pathToFolder + m_selectedFile->m_name;
				m_selectedMaterial->m_path = m_selectedFile->m_path;
			}
		}
		WidgetsUtility::PopStyleVar();

		WidgetsUtility::IncrementCursorPosY(12);
		ImGui::SetCursorPosX(12);

		// Caret
		bool caretGeneral = WidgetsUtility::Caret("##matdraw_general");
		ImGui::SameLine();
		ImGui::AlignTextToFramePadding();
		WidgetsUtility::IncrementCursorPosY(-5);
		ImGui::Text("General Settings");
		if (ImGui::IsItemClicked())
			WidgetsUtility::s_carets["##matdraw_general"] = !WidgetsUtility::s_carets["##matdraw_general"];
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

			// Material selection.
			char shaderPathC[128] = "";
			strcpy(shaderPathC, m_selectedMaterial->m_shaderPath.c_str());

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Material");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 35 - ImGui::GetCursorPosX());
			ImGui::InputText("##selectedShader", shaderPathC, IM_ARRAYSIZE(shaderPathC), ImGuiInputTextFlags_ReadOnly);
			ImGui::SameLine();
			WidgetsUtility::IncrementCursorPosY(5);


			// Material drag & drop.
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(RESOURCES_MOVESHADER_ID))
				{
					IM_ASSERT(payload->DataSize == sizeof(uint32));
					Graphics::Material::SetMaterialShader(*m_selectedMaterial, LinaEngine::Graphics::Shader::GetShader(*(uint32*)payload->m_data));

				}
				ImGui::EndDragDropTarget();
			}

			if (WidgetsUtility::IconButton("##selectshader", ICON_FA_MINUS_SQUARE, 0.0f, .7f, ImVec4(1, 1, 1, 0.8f), ImVec4(1, 1, 1, 1), ImGui::GetStyleColorVec4(ImGuiCol_Header)))
			{

				Graphics::Material::SetMaterialShader(*m_selectedMaterial, LinaEngine::Graphics::RenderEngine::GetDefaultShader());

			}
		}

		if (m_selectedMaterial->m_floats.size() > 0)
		{
			// Caret.
			WidgetsUtility::IncrementCursorPosX(11);
			WidgetsUtility::IncrementCursorPosY(11);
			bool caretFloats = WidgetsUtility::Caret("##matdraw_floats");
			ImGui::SameLine();
			ImGui::AlignTextToFramePadding();
			WidgetsUtility::IncrementCursorPosY(-5);
			ImGui::Text("Floats");
			if (ImGui::IsItemClicked())
				WidgetsUtility::s_carets["##matdraw_floats"] = !WidgetsUtility::s_carets["##matdraw_floats"];
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
					ImGui::DragFloat(label.c_str(), &it->second, 0.08f);
					WidgetsUtility::PopStyleVar();
				}
			}
		}
	
		if (m_selectedMaterial->m_ints.size() > 0)
		{
			// Caret.
			WidgetsUtility::IncrementCursorPosX(11);
			WidgetsUtility::IncrementCursorPosY(11);
			bool caretInts = WidgetsUtility::Caret("##matdraw_ints");
			ImGui::SameLine();
			ImGui::AlignTextToFramePadding();
			WidgetsUtility::IncrementCursorPosY(-5);
			ImGui::Text("Ints");
			if (ImGui::IsItemClicked())
				WidgetsUtility::s_carets["##matdraw_ints"] = !WidgetsUtility::s_carets["##matdraw_ints"];
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

		}

		if (m_selectedMaterial->m_bools.size() > 0)
		{
			// Caret.
			WidgetsUtility::IncrementCursorPosX(11);
			WidgetsUtility::IncrementCursorPosY(11);
			bool caretBools = WidgetsUtility::Caret("##matdraw_bools");
			ImGui::SameLine();
			ImGui::AlignTextToFramePadding();
			WidgetsUtility::IncrementCursorPosY(-5);
			ImGui::Text("Bools");
			if (ImGui::IsItemClicked())
				WidgetsUtility::s_carets["##matdraw_bools"] = !WidgetsUtility::s_carets["##matdraw_bools"];
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
		}
	

		if (m_selectedMaterial->m_colors.size() > 0)
		{
			// Caret.
			WidgetsUtility::IncrementCursorPosX(11);
			WidgetsUtility::IncrementCursorPosY(11);
			bool caretColors = WidgetsUtility::Caret("##matdraw_Colors");
			ImGui::SameLine();
			ImGui::AlignTextToFramePadding();
			WidgetsUtility::IncrementCursorPosY(-5);
			ImGui::Text("Colors");
			if (ImGui::IsItemClicked())
				WidgetsUtility::s_carets["##matdraw_Colors"] = !WidgetsUtility::s_carets["##matdraw_Colors"];
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
		}

		

		if (m_selectedMaterial->m_vector2s.size() > 0)
		{
			// Caret.
			WidgetsUtility::IncrementCursorPosX(11);
			WidgetsUtility::IncrementCursorPosY(11);
			bool caretVector2s = WidgetsUtility::Caret("##matdraw_Vector2s");
			ImGui::SameLine();
			ImGui::AlignTextToFramePadding();
			WidgetsUtility::IncrementCursorPosY(-5);
			ImGui::Text("Vector2s");
			if (ImGui::IsItemClicked())
				WidgetsUtility::s_carets["##matdraw_Vector2s"] = !WidgetsUtility::s_carets["##matdraw_Vector2s"];
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

		}
		
		if (m_selectedMaterial->m_vector3s.size() > 0)
		{
			// Caret.
			WidgetsUtility::IncrementCursorPosX(11);
			WidgetsUtility::IncrementCursorPosY(11);
			bool caretVector3s = WidgetsUtility::Caret("##matdraw_Vector3s");
			ImGui::SameLine();
			ImGui::AlignTextToFramePadding();
			WidgetsUtility::IncrementCursorPosY(-5);
			ImGui::Text("Vector3s");
			if (ImGui::IsItemClicked())
				WidgetsUtility::s_carets["##matdraw_Vector3s"] = !WidgetsUtility::s_carets["##matdraw_Vector3s"];
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

		}
	
		if (m_selectedMaterial->m_vector4s.size() > 0)
		{
			// Caret.
			WidgetsUtility::IncrementCursorPosX(11);
			WidgetsUtility::IncrementCursorPosY(11);
			bool caretVector4s = WidgetsUtility::Caret("##matdraw_Vector4s");
			ImGui::SameLine();
			ImGui::AlignTextToFramePadding();
			WidgetsUtility::IncrementCursorPosY(-5);
			ImGui::Text("Vector4s");
			if (ImGui::IsItemClicked())
				WidgetsUtility::s_carets["##matdraw_Vector4s"] = !WidgetsUtility::s_carets["##matdraw_Vector4s"];
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
		}
	

		if (m_selectedMaterial->m_sampler2Ds.size() > 0)
		{
			// Caret.
			WidgetsUtility::IncrementCursorPosX(11);
			WidgetsUtility::IncrementCursorPosY(11);
			bool caretTextures = WidgetsUtility::Caret("##matdraw_textures");
			ImGui::SameLine();
			ImGui::AlignTextToFramePadding();
			WidgetsUtility::IncrementCursorPosY(-5);
			ImGui::Text("Textures");
			if (ImGui::IsItemClicked())
				WidgetsUtility::s_carets["##matdraw_textures"] = !WidgetsUtility::s_carets["##matdraw_textures"];
			ImGui::AlignTextToFramePadding();

			if (caretTextures)
			{
				WidgetsUtility::IncrementCursorPosY(11);

				for (const auto& it : m_selectedMaterial->m_sampler2Ds)
				{
					if (ShouldExcludeProperty(it.first))
						continue;

					WidgetsUtility::FramePaddingX(4);
					WidgetsUtility::IncrementCursorPosX(30);
					WidgetsUtility::AlignedText(it.first.c_str());

					ImVec2 min = ImVec2(ImGui::GetWindowPos().x + ImGui::GetCursorPos().x + 175, ImGui::GetWindowPos().y + ImGui::GetCursorScreenPos().y - 80);
					ImVec2 max = ImVec2(min.x + 75, min.y + 75);
					ImGui::GetWindowDrawList()->AddRectFilled(min, max, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_FrameBg)), 5);

					ImVec2 minTexture = ImVec2(ImGui::GetWindowPos().x + ImGui::GetCursorPos().x + 180, ImGui::GetWindowPos().y + ImGui::GetCursorScreenPos().y - 75);
					ImVec2 maxTexture = ImVec2(minTexture.x + 65, minTexture.y + 65);

					if (it.second.m_boundTexture != nullptr)
					{
						ImGui::GetWindowDrawList()->AddImage((void*)it.second.m_boundTexture->GetID(), minTexture, maxTexture, ImVec2(0, 1), ImVec2(1, 0));

						if (ImGui::IsMouseHoveringRect(minTexture, maxTexture) && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
						{
							LinaEditor::EditorApplication::GetEditorDispatcher().DispatchAction<LinaEngine::Graphics::Texture*>(LinaEngine::Action::ActionType::MaterialTextureSelected, it.second.m_boundTexture);
						}
					}

					WidgetsUtility::IncrementCursorPosY(80);
					WidgetsUtility::PopStyleVar();

					// Dropped on another title, swap component orders.
					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(RESOURCES_MOVETEXTURE_ID))
						{
							IM_ASSERT(payload->DataSize == sizeof(uint32));
							m_selectedMaterial->SetTexture(it.first, &LinaEngine::Graphics::Texture::GetTexture(*(uint32*)payload->m_data), it.second.m_bindMode);
						}


						ImGui::EndDragDropTarget();
					}
				}
			}
		}
	

		WidgetsUtility::IncrementCursorPosX(11);
		WidgetsUtility::IncrementCursorPosY(11);
		if (ImGui::Button("Apply Changes", ImVec2(90,30)))
		{
			LinaEngine::Graphics::Material::SaveMaterialData(*m_selectedMaterial, m_selectedMaterial->GetPath());
			LinaEngine::Application::GetRenderEngine().MaterialUpdated(*m_selectedMaterial);
		}

		ImGui::SameLine();
		WidgetsUtility::IncrementCursorPosX(11);

		if (ImGui::Button("Reset", ImVec2(57, 30)))
		{
			LinaEngine::Graphics::Material::SetMaterialShader(*m_selectedMaterial, LinaEngine::Graphics::RenderEngine::GetDefaultShader());
		}



		ImGui::SameLine();
		WidgetsUtility::IncrementCursorPosX(11);
	}

	bool MaterialDrawer::ShouldExcludeProperty(const std::string& property)
	{
		for (auto& a : excludeProperties)
		{
			if (property.compare(a) == 0 || property.find(a) != std::string::npos)
				return true;
		}

		return false;
	}

}
