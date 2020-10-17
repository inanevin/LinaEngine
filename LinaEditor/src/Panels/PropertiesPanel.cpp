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

Class: PropertiesPanel
Timestamp: 6/7/2020 5:13:42 PM

*/


#include "Panels/PropertiesPanel.hpp"
#include "Core/GUILayer.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/Sampler.hpp"
#include "Rendering/Material.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "IconsFontAwesome5.h"
#include "Drawers/EntityDrawer.hpp"
#include "Rendering/RenderEngine.hpp"
#include "Rendering/Mesh.hpp"

namespace LinaEditor
{
	using namespace LinaEngine::ECS;
	using namespace LinaEngine;
	static bool openCompExistsModal;

	void PropertiesPanel::Setup()
	{
		m_ecs = m_guiLayer->GetECS();
		m_renderEngine = m_guiLayer->GetRenderEngine();

		m_textureDrawer.Setup(m_renderEngine);
		m_entityDrawer.Setup(m_ecs);

	}

	void PropertiesPanel::EntitySelected(LinaEngine::ECS::ECSEntity selectedEntity)
	{
		m_currentDrawType = DrawType::ENTITIES;
		m_entityDrawer.SetSelectedEntity(selectedEntity);
	}

	void PropertiesPanel::Texture2DSelected(LinaEngine::Graphics::Texture* texture, int id, std::string& path)
	{
		m_currentDrawType = DrawType::TEXTURE2D;
		m_textureDrawer.SetSelectedTexture(texture);
	}

	void PropertiesPanel::MeshSelected(LinaEngine::Graphics::Mesh* mesh, int id, std::string& path)
	{
		m_selectedMesh = mesh;
		m_currentDrawType = DrawType::MESH;
		m_selectedMeshID = id;
		m_selectedMeshPath = path;
		LinaEngine::Graphics::MeshParameters& params = mesh->GetParameters();
		m_currentMeshParams = params;
	}

	void PropertiesPanel::Draw(float frameTime)
	{
		if (m_show)
		{

			// Set window properties.
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImVec2 work_area_pos = viewport->GetWorkPos();
			ImVec2 panelSize = ImVec2(m_size.x, m_size.y);
			ImGui::SetNextWindowSize(panelSize, ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowBgAlpha(1.0f);
			ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;


			// window.
			ImGui::Begin("Properties", &m_show, flags);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(ImGui::GetStyle().FramePadding.x, 0));

			// Shadow.
			WidgetsUtility::DrawShadowedLine(5);

			// Draw the selected item.
			if (m_currentDrawType == DrawType::ENTITIES)
			{
				m_entityDrawer.DrawSelectedEntity();
			}
			else if (m_currentDrawType == DrawType::TEXTURE2D)
			{
				m_textureDrawer.DrawSelectedTexture();
			}
			else if (m_currentDrawType == DrawType::MESH)
				DrawMeshProperties();
			else if (m_currentDrawType == DrawType::MATERIAL)
				DrawMaterialProperties();

			ImGui::PopStyleVar();
			ImGui::End();

		}
	}



	void PropertiesPanel::DrawMeshProperties()
	{
		Graphics::MeshParameters params = m_selectedMesh->GetParameters();

		ImGui::Checkbox("Triangulate", &m_currentMeshParams.triangulate);
		ImGui::Checkbox("Generate Smooth Normals", &m_currentMeshParams.smoothNormals);
		ImGui::Checkbox("Calculate Tangent Space", &m_currentMeshParams.calculateTangentSpace);

		if (ImGui::Button("Apply"))
		{
			Graphics::MeshParameters params = m_currentMeshParams;
			m_renderEngine->UnloadMeshResource(m_selectedMeshID);
			m_selectedMesh = &m_renderEngine->CreateMesh(m_selectedMeshID, m_selectedMeshPath, params);
		}
	}
	void PropertiesPanel::DrawMaterialProperties()
	{
		ImGui::Text("test");
	}
}