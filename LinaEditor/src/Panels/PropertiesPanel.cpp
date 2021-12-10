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
#include "Core/Application.hpp"
#include "Rendering/Model.hpp"
#include "Core/EditorApplication.hpp"
#include "Utility/EditorUtility.hpp"
#include "IconsFontAwesome5.h"

namespace Lina::Editor
{
	using namespace Lina::ECS;
	using namespace Lina;
	static bool s_openCompExistsModal;


	void PropertiesPanel::Setup()
	{
		EditorApplication::GetEditorDispatcher().SubscribeAction<Lina::ECS::ECSEntity>("##lina_propsPanel_entity", Lina::Action::ActionType::EntitySelected,
			std::bind(&PropertiesPanel::EntitySelected, this, std::placeholders::_1));

		EditorApplication::GetEditorDispatcher().SubscribeAction<Lina::Graphics::Model*>("##lina_propsPanel_mesh", Lina::Action::ActionType::MeshSelected,
			std::bind(&PropertiesPanel::MeshSelected, this, std::placeholders::_1));

		EditorApplication::GetEditorDispatcher().SubscribeAction<std::pair<EditorFile*, Lina::Graphics::Material*>>("##lina_propsPanel_material", Lina::Action::ActionType::MaterialSelected,
			std::bind(&PropertiesPanel::MaterialSelected, this, std::placeholders::_1));


		EditorApplication::GetEditorDispatcher().SubscribeAction<Lina::Graphics::Texture*>("##lina_propsPanel_texture", Lina::Action::ActionType::TextureSelected,
			std::bind(&PropertiesPanel::TextureSelected, this, std::placeholders::_1));

		EditorApplication::GetEditorDispatcher().SubscribeAction<void*>("##lina_propsPanel_unselect", Lina::Action::ActionType::Unselect,
			std::bind(&PropertiesPanel::Unselect, this));

		m_entityDrawer.Setup();
	}

	void PropertiesPanel::EntitySelected(Lina::ECS::ECSEntity selectedEntity)
	{
		m_entityDrawer.SetSelectedEntity(selectedEntity);
		m_currentDrawType = DrawType::Entities;
	}

	void PropertiesPanel::TextureSelected(Lina::Graphics::Texture* texture)
	{
		m_textureDrawer.SetSelectedTexture(texture);
		m_currentDrawType = DrawType::Texture2D;
	}

	void PropertiesPanel::MaterialSelected(std::pair<EditorFile*, Lina::Graphics::Material*> pair)
	{
		m_materialDrawer.SetSelectedMaterial(pair.first, *pair.second);
		m_currentDrawType = DrawType::Material;
	}
	void PropertiesPanel::MeshSelected(Lina::Graphics::Model* mesh)
	{
		m_meshDrawer.SetSelectedMesh(*mesh);
		m_currentDrawType = DrawType::Model;
	}


	void PropertiesPanel::Draw()
	{
		if (m_show)
		{

			// Set window properties.
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImVec2 work_area_pos = viewport->GetWorkPos();
			ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;
			ImGui::SetNextWindowBgAlpha(1.0f);


			// window.
			ImGui::Begin(PROPERTIES_ID, &m_show, flags);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(ImGui::GetStyle().FramePadding.x, 0));

			// Shadow.
			WidgetsUtility::DrawShadowedLine(5);

			// Draw the selected item.
			if (m_currentDrawType == DrawType::Entities)
				m_entityDrawer.DrawSelectedEntity();
			else if (m_currentDrawType == DrawType::Texture2D)
				m_textureDrawer.DrawSelectedTexture();
			else if (m_currentDrawType == DrawType::Model)
				m_meshDrawer.DrawSelectedMesh();
			else if (m_currentDrawType == DrawType::Material)
				m_materialDrawer.DrawSelectedMaterial();

			ImGui::PopStyleVar();
			ImGui::End();

		}
	}


}