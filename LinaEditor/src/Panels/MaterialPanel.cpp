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

#include "Panels/MaterialPanel.hpp"
#include "Core/GUILayer.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/RenderEngine.hpp"
#include "imgui/imgui.h"
#include "imgui/ImGuiFileDialogue/ImGuiFileDialog.h"


namespace LinaEditor
{
	static std::string chosenMapID;

	void MaterialPanel::Draw(float frameTime)
	{
		if (m_show)
		{
			// Set window properties.
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImVec2 work_area_pos = viewport->GetWorkPos();
			ImVec2 panelSize = ImVec2(m_size.x, m_size.y);
			ImGui::SetNextWindowSize(panelSize, ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowBgAlpha(0.2f);
			if (ImGui::Begin("Material Panel", &m_show))
			{

				if (m_currentSelectedMaterial != nullptr)
				{
					if (m_currentSelectedMaterial->GetShaderType() == Graphics::Shaders::PBR_LIT)
					{
						// 8 ? textures

						Graphics::Texture* albedo = m_currentSelectedMaterial->GetTexture(MAT_TEXTURE2D_ALBEDOMAP);
						Graphics::Texture* normal = m_currentSelectedMaterial->GetTexture(MAT_TEXTURE2D_NORMALMAP);
						Graphics::Texture* roughness = m_currentSelectedMaterial->GetTexture(MAT_TEXTURE2D_ROUGHNESSMAP);
						Graphics::Texture* metallic = m_currentSelectedMaterial->GetTexture(MAT_TEXTURE2D_METALLICMAP);
						Graphics::Texture* ao = m_currentSelectedMaterial->GetTexture(MAT_TEXTURE2D_AOMAP);

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
						float m = m_currentSelectedMaterial->GetFloat(MAT_METALLICMULTIPLIER);
						float r = m_currentSelectedMaterial->GetFloat(MAT_ROUGHNESSMULTIPLIER);
						ImGui::DragFloat("Metallic", &m);
						ImGui::DragFloat("Roughness", &r);
						m_currentSelectedMaterial->SetFloat(MAT_METALLICMULTIPLIER, m);
						m_currentSelectedMaterial->SetFloat(MAT_ROUGHNESSMULTIPLIER, r);

						// Workflow
						bool isMetallic = m_currentSelectedMaterial->GetInt(MAT_WORKFLOW) == 1 ? true : false;
						ImGui::Checkbox("Metalilc Workflow", &isMetallic);
						m_currentSelectedMaterial->SetInt(MAT_WORKFLOW, isMetallic ? 1 : 0);

						// Tiling
						Vector2 tiling = m_currentSelectedMaterial->GetVector2(MAT_TILING);
						float tilingF[2] = { tiling.x, tiling.y };
						ImGui::InputFloat2("Tiling", tilingF);
						m_currentSelectedMaterial->SetVector2(MAT_TILING, Vector2(tilingF[0], tilingF[1]));

					}
				}
			}

			ImGui::End();
		}
	}

	void MaterialPanel::Setup()
	{
		m_renderEngine = m_guiLayer->GetRenderEngine();
	}
}