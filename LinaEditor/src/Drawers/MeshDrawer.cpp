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

#include "Drawers/MeshDrawer.hpp"
#include "Rendering/Mesh.hpp"
#include "Core/Application.hpp"
#include "Rendering/RenderEngine.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "imgui/imgui.h"

namespace LinaEditor
{

#define CURSORPOS_X_LABELS 12
#define CURSORPOS_XPERC_VALUES 0.55f

	void MeshDrawer::SetSelectedMesh(LinaEngine::Graphics::Mesh& mesh)
	{
		m_selectedMesh = &mesh;
		m_selectedParams = m_selectedMesh->GetParameters();
	}

	void MeshDrawer::DrawSelectedMesh()
	{
		WidgetsUtility::IncrementCursorPosY(12);

		float cursorPosValues = ImGui::GetWindowSize().x * CURSORPOS_XPERC_VALUES;
		float cursorPosLabels = CURSORPOS_X_LABELS;

		ImGui::SetCursorPosX(cursorPosLabels);
		WidgetsUtility::AlignedText("Triangulate");
		ImGui::SameLine();
		ImGui::SetCursorPosX(cursorPosValues);
		ImGui::Checkbox("##triangulate", &m_selectedParams.m_triangulate);

		ImGui::SetCursorPosX(cursorPosLabels);
		WidgetsUtility::AlignedText("Smooth Normals");
		ImGui::SameLine();
		ImGui::SetCursorPosX(cursorPosValues);
		ImGui::Checkbox("##smoothNormals", &m_selectedParams.m_smoothNormals);

		ImGui::SetCursorPosX(cursorPosLabels);
		WidgetsUtility::AlignedText("Calculate Tangents");
		ImGui::SameLine();
		ImGui::SetCursorPosX(cursorPosValues);
		ImGui::Checkbox("##tangentSpace", &m_selectedParams.m_calculateTangentSpace);

		ImGui::SetCursorPosX(cursorPosLabels);

		if (ImGui::Button("Apply"))
		{
			int id = m_selectedMesh->GetID();
			LinaEngine::Graphics::RenderEngine& renderEngine = LinaEngine::Application::GetRenderEngine();
			std::string filePath = m_selectedMesh->GetPath();
			std::string paramsPath = m_selectedMesh->GetParamsPath();
			renderEngine.UnloadMeshResource(id);
			m_selectedMesh = &renderEngine.CreateMesh(filePath, Graphics::MeshParameters(m_selectedParams), id);
			LINA_CORE_TRACE("File: {0} Params: {1}", filePath, paramsPath);
			//LinaEngine::Graphics::Mesh::SaveParameters(paramsPath, m_selectedParams);

		}

	}
}