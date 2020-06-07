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

Class: ResourcesPanel
Timestamp: 6/5/2020 12:55:10 AM

*/

#include "Panels/ResourcesPanel.hpp"
#include "Core/GUILayer.hpp"
#include "Panels/PropertiesPanel.hpp"
#include "Rendering/RenderEngine.hpp"
#include "imgui.h"
#include "imgui/ImGuiFileDialogue/ImGuiFileDialog.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <filesystem>

namespace LinaEditor
{
	static int itemIDCounter = 0;
	static int selectedItem = -1;

	void ResourcesPanel::Draw()
	{
		if (m_Show)
		{
			// Set window properties.
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImVec2 work_area_pos = viewport->GetWorkPos();
			ImVec2 panelSize = ImVec2(m_Size.x, m_Size.y);
			ImGui::SetNextWindowSize(panelSize, ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowBgAlpha(1.0f);
			ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;


			ImGui::Begin("Resources", &m_Show, flags);

			DrawFolder(m_ResourceFolders[0]);

			ImGui::End();
		}
	}

	void ResourcesPanel::Setup()
	{
		m_PropertiesPanel = m_GUILayer->GetPropertiesPanel();
		m_RenderEngine = m_GUILayer->GetRenderEngine();
		
		// Scan root resources folder.
		ScanRoot();
	}

	void ResourcesPanel::ScanRoot()
	{
		// Create root.
		EditorFolder root;
		root.name = "Resources";
		root.path = "resources";
		m_ResourceFolders.push_back(root);

		// Recursively fill in root.
		itemIDCounter = -1;
		std::string path = "resources";
		ScanFolder(m_ResourceFolders[0]);

		// Load resources
		LoadFolderResources(m_ResourceFolders[0]);
	}

	void ResourcesPanel::ScanFolder(EditorFolder& root)
	{
		for (const auto& entry : std::filesystem::directory_iterator(root.path))
		{
			if (entry.path().has_extension())
			{
				// Is a file
				EditorFile file;
				file.name = entry.path().filename().string();
				file.path = entry.path().relative_path().string();
				file.extension = file.name.substr(file.name.find(".") + 1);
				file.type = GetFileType(file.extension);
				file.id = ++itemIDCounter;

				// Add to the folder data.
				root.files.push_back(file);
			}
			else
			{
				// Is a folder
				EditorFolder folder;
				folder.name = entry.path().filename().string();
				folder.path = entry.path().relative_path().string();
				folder.id = ++itemIDCounter;

				// Add to the sub folders.
				root.subFolders.push_back(folder);

				// Iterate recursively.
				ScanFolder(root.subFolders.back());
			}
		}

	}

	void ResourcesPanel::DrawFolder(EditorFolder& folder)
	{
		static ImGuiTreeNodeFlags folderFlagsNotSelected = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
		static ImGuiTreeNodeFlags folderFlagsSelected = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Selected;
		static ImGuiTreeNodeFlags fileNodeFlagsNotSelected = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth;
		static ImGuiTreeNodeFlags fileNodeFlagsSelected = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Selected;

		// Draw folders.
		for (int i = 0; i < folder.subFolders.size(); i++)
		{
			ImGuiTreeNodeFlags folderFlags = folder.subFolders[i].id == selectedItem ? folderFlagsSelected : folderFlagsNotSelected;
			bool nodeOpen = ImGui::TreeNodeEx(folder.subFolders[i].name.c_str(), folderFlags);

			if (ImGui::IsItemClicked())
				selectedItem = folder.subFolders[i].id;

			if (nodeOpen)
			{
				DrawFolder(folder.subFolders[i]);
				ImGui::TreePop();
			}

		}

		// Draw files.
		for (int i = 0; i < folder.files.size(); i++)
		{
			ImGuiTreeNodeFlags fileFlags = folder.files[i].id == selectedItem ? fileNodeFlagsSelected : fileNodeFlagsNotSelected;
			bool nodeOpen = ImGui::TreeNodeEx(folder.files[i].name.c_str(), fileFlags);

			if (ImGui::IsItemClicked())
			{
				selectedItem = folder.files[i].id;
				//m_PropertiesPanel->FileSelected(folder.files[i].type);
			}

			if (nodeOpen)
				ImGui::TreePop();
		}
	}

	void ResourcesPanel::LoadFolderResources(EditorFolder& folder)
	{
		// Load files.
		for (int i = 0; i < folder.files.size(); i++)
		{
			EditorFile& file = folder.files[i];

			if (file.type == FileType::TEXTURE2D)
			{
				m_RenderEngine->CreateTexture2D(file.id, file.path);
			}
		}

		// Recursively load subfolders.
		for (int i = 0; i < folder.subFolders.size(); i++)
			LoadFolderResources(folder.subFolders[i]);
	}

	ResourcesPanel::FileType ResourcesPanel::GetFileType(std::string& extension)
	{
		if (extension.compare("jpg") == 0 || extension.compare("jpeg") == 0 || extension.compare("png") == 0 || extension.compare("tga") == 0)
			return FileType::TEXTURE2D;
		else if (extension.compare("ttf") == 0)
			return FileType::FONT;
		else
			return FileType::UNKNOWN;
	}
}