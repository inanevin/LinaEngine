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


	void PropertiesPanel::Initialize()
	{
		Lina::Event::EventSystem::Get()->Connect<EEntitySelected, &PropertiesPanel::EntitySelected>(this);
		Lina::Event::EventSystem::Get()->Connect<EModelSelected, &PropertiesPanel::ModelSelected>(this);
		Lina::Event::EventSystem::Get()->Connect<EMaterialSelected, &PropertiesPanel::MaterialSelected>(this);
		Lina::Event::EventSystem::Get()->Connect<ETextureSelected, &PropertiesPanel::TextureSelected>(this);
		Lina::Event::EventSystem::Get()->Connect<EEntityUnselected, &PropertiesPanel::Unselect>(this);

		m_entityDrawer.Initialize();
	}

	void PropertiesPanel::EntitySelected(EEntitySelected ev)
	{
		m_entityDrawer.SetSelectedEntity(ev.m_entity);
		m_currentDrawType = DrawType::Entities;
	}

	void PropertiesPanel::TextureSelected(ETextureSelected ev)
	{
		m_textureDrawer.SetSelectedTexture(ev.m_texture);
		m_currentDrawType = DrawType::Texture2D;
	}

	void PropertiesPanel::MaterialSelected(EMaterialSelected ev)
	{
		m_materialDrawer.SetSelectedMaterial(ev.m_file, *ev.m_material);
		m_currentDrawType = DrawType::Material;
	}
	void PropertiesPanel::ModelSelected(EModelSelected ev)
	{
		m_modelDrawer.SetSelectedModel(*ev.m_model);
		m_currentDrawType = DrawType::Model;
	}


	void PropertiesPanel::Draw()
	{
		if (m_show)
		{

			// Set window properties.
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImVec2 work_area_pos = viewport->WorkPos;
			ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;
			ImGui::SetNextWindowBgAlpha(1.0f);


			// window.
			ImGui::Begin(PROPERTIES_ID, NULL, flags);
			WidgetsUtility::CloseWindowTabPopup(&m_show);

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(ImGui::GetStyle().FramePadding.x, 0));

			// Shadow.

			// Draw the selected item.
			if (m_currentDrawType == DrawType::Entities)
				m_entityDrawer.DrawSelectedEntity();
			else if (m_currentDrawType == DrawType::Texture2D)
				m_textureDrawer.DrawSelectedTexture();
			else if (m_currentDrawType == DrawType::Model)
				m_modelDrawer.DrawSelectedMesh();
			else if (m_currentDrawType == DrawType::Material)
				m_materialDrawer.DrawSelectedMaterial();

			ImGui::PopStyleVar();
			ImGui::End();

		}
	}


}