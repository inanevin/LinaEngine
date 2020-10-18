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

#include "Panels/PropertiesPanel.hpp"
#include "Core/GUILayer.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "Drawers/EntityDrawer.hpp"
#include "Rendering/RenderEngine.hpp"
#include "Rendering/Mesh.hpp"
#include "Core/EditorApplication.hpp"
#include "IconsFontAwesome5.h"

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

		EditorApplication::GetEditorDispatcher().SubscribeAction<LinaEngine::ECS::ECSEntity>("##lina_propsPanel_entity", LinaEngine::Action::ActionType::EntitySelected,
			std::bind(&PropertiesPanel::EntitySelected, this, std::placeholders::_1));
	}

	void PropertiesPanel::EntitySelected(LinaEngine::ECS::ECSEntity selectedEntity)
	{
		m_currentDrawType = DrawType::Entities;
		m_entityDrawer.SetSelectedEntity(selectedEntity);
	}

	void PropertiesPanel::Texture2DSelected(LinaEngine::Graphics::Texture* texture, int id, std::string& path)
	{
		m_currentDrawType = DrawType::Texture2D;
		m_textureDrawer.SetSelectedTexture(texture);
	}

	void PropertiesPanel::MeshSelected(LinaEngine::Graphics::Mesh* mesh, int id, std::string& path)
	{
		m_selectedMesh = mesh;
		m_currentDrawType = DrawType::Mesh;
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
			if (m_currentDrawType == DrawType::Entities)
			{
				m_entityDrawer.DrawSelectedEntity();
			}
			else if (m_currentDrawType == DrawType::Texture2D)
			{
				m_textureDrawer.DrawSelectedTexture();
			}
			else if (m_currentDrawType == DrawType::Mesh)
				DrawMeshProperties();
			else if (m_currentDrawType == DrawType::Material)
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