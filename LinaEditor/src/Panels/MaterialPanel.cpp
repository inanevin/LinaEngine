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

Class: MaterialPanel
Timestamp: 6/4/2020 8:35:30 PM

*/

#include "Panels/MaterialPanel.hpp"
#include "Core/GUILayer.hpp"
#include "imgui.h"
#include "imgui/ImGuiFileDialogue/ImGuiFileDialog.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Rendering/Texture.hpp"
#include "Rendering/RenderEngine.hpp"


namespace LinaEditor
{
	static std::string chosenMapID;

	void MaterialPanel::Draw()
	{
		if (m_Show)
		{
			// Set window properties.
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImVec2 work_area_pos = viewport->GetWorkPos();
			ImVec2 panelSize = ImVec2(m_Size.x, m_Size.y);
			ImGui::SetNextWindowSize(panelSize, ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowBgAlpha(0.2f);
			if (ImGui::Begin("Material Panel", &m_Show))
			{
				if (m_CurrentSelectedMaterial != nullptr)
				{
					if (m_CurrentSelectedMaterial->GetShaderType() == Graphics::Shaders::PBR_LIT)
					{
						// 8 ? textures

						Graphics::Texture* albedo = m_CurrentSelectedMaterial->GetTexture(MAT_TEXTURE2D_ALBEDOMAP);
						Graphics::Texture* normal = m_CurrentSelectedMaterial->GetTexture(MAT_TEXTURE2D_NORMALMAP);
						Graphics::Texture* roughness = m_CurrentSelectedMaterial->GetTexture(MAT_TEXTURE2D_ROUGHNESSMAP);
						Graphics::Texture* metallic = m_CurrentSelectedMaterial->GetTexture(MAT_TEXTURE2D_METALLICMAP);
						Graphics::Texture* ao = m_CurrentSelectedMaterial->GetTexture(MAT_TEXTURE2D_AOMAP);

						ImVec2 imageButtonSize = ImVec2(50, 50);
						std::string fileDialogueID = "ChooseMap";
						// Albedo Map
						ImGui::Text("Albedo Map");
						ImGui::SameLine();
						if (ImGui::ImageButton(albedo != nullptr ? (void*)albedo->GetID() : 0, imageButtonSize))
						{
							chosenMapID = MAT_TEXTURE2D_ALBEDOMAP;
							igfd::ImGuiFileDialog::Instance()->OpenDialog(fileDialogueID, "Choose File", ".png\0.jpg\0.jpeg\0.tga\0", ".");
						}

						// Normal Map
						ImGui::Text("Normal Map");
						ImGui::SameLine();
						if (ImGui::ImageButton(normal != nullptr ? (void*)normal->GetID() : 0, imageButtonSize))
						{
							chosenMapID = MAT_TEXTURE2D_NORMALMAP;
							igfd::ImGuiFileDialog::Instance()->OpenDialog(fileDialogueID, "Choose File", ".png\0.jpg\0.jpeg\0.tga\0", ".");
						}

						// Roughness Map
						ImGui::Text("Roughness Map");
						ImGui::SameLine();
						if (ImGui::ImageButton(roughness != nullptr ? (void*)roughness->GetID() : 0, imageButtonSize))
						{
							chosenMapID = MAT_TEXTURE2D_ROUGHNESSMAP;
							igfd::ImGuiFileDialog::Instance()->OpenDialog(fileDialogueID, "Choose File", ".png\0.jpg\0.jpeg\0.tga\0", ".");
						}

						// Metalic Map
						ImGui::Text("Metallic Map");
						ImGui::SameLine();
						if (ImGui::ImageButton(metallic != nullptr ? (void*)metallic->GetID() : 0, imageButtonSize))
						{
							chosenMapID = MAT_TEXTURE2D_METALLICMAP;
							igfd::ImGuiFileDialog::Instance()->OpenDialog(fileDialogueID, "Choose File", ".png\0.jpg\0.jpeg\0.tga\0", ".");
						}

						// AO Map
						ImGui::Text("AO Map");
						ImGui::SameLine();
						if (ImGui::ImageButton(ao != nullptr ? (void*)ao->GetID() : 0, imageButtonSize))
						{
							chosenMapID = MAT_TEXTURE2D_AOMAP;
							igfd::ImGuiFileDialog::Instance()->OpenDialog(fileDialogueID, "Choose File", ".png\0.jpg\0.jpeg\0.tga\0", ".");
						}
							

						// display
						if (igfd::ImGuiFileDialog::Instance()->FileDialog(fileDialogueID))
						{
							// action if OK
							if (igfd::ImGuiFileDialog::Instance()->IsOk == true)
							{
								std::string filePathName = igfd::ImGuiFileDialog::Instance()->GetFilepathName();
								std::string filePath = igfd::ImGuiFileDialog::Instance()->GetCurrentPath();
								std::string fileName = igfd::ImGuiFileDialog::Instance()->GetCurrentFileName();
							
								//m_CurrentSelectedMaterial->SetTexture(chosenMapID, &m_RenderEngine->CreateTexture2D(filePathName));


							
								igfd::ImGuiFileDialog::Instance()->CloseDialog(fileDialogueID);
							}
							// close
							igfd::ImGuiFileDialog::Instance()->CloseDialog(fileDialogueID);
						}

						// Metallic & specular multipliers
						float m = m_CurrentSelectedMaterial->GetFloat(MAT_METALLICMULTIPLIER);
						float r = m_CurrentSelectedMaterial->GetFloat(MAT_ROUGHNESSMULTIPLIER);
						ImGui::DragFloat("Metallic", &m);
						ImGui::DragFloat("Roughness", &r);
						m_CurrentSelectedMaterial->SetFloat(MAT_METALLICMULTIPLIER, m);
						m_CurrentSelectedMaterial->SetFloat(MAT_ROUGHNESSMULTIPLIER, r);

						// Workflow
						bool isMetallic = m_CurrentSelectedMaterial->GetInt(MAT_WORKFLOW) == 1 ? true : false;
						ImGui::Checkbox("Metalilc Workflow", &isMetallic);
						m_CurrentSelectedMaterial->SetInt(MAT_WORKFLOW, isMetallic ? 1 : 0);

						// Tiling
						Vector2 tiling = m_CurrentSelectedMaterial->GetVector2(MAT_TILING);
						float tilingF[2] = { tiling.x, tiling.y };
						ImGui::InputFloat2("Tiling", tilingF);
						m_CurrentSelectedMaterial->SetVector2(MAT_TILING, Vector2(tilingF[0], tilingF[1]));

					}
				}
			}

			ImGui::End();
		}
	}

	void MaterialPanel::Setup()
	{
		m_RenderEngine = m_GUILayer->GetRenderEngine();
	}
}