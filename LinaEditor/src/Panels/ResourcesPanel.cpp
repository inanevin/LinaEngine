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
#include "imgui.h"
#include "imgui/ImGuiFileDialogue/ImGuiFileDialog.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <filesystem>

namespace LinaEditor
{

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

			if (ImGui::Begin("Resources", &m_Show, flags))
			{

			}

			ImGui::End();
		}
	}

	void ResourcesPanel::Setup()
	{
		// Create root.
		EditorFolder root;
		root.folderName = "Resources";
		root.path = "resources";
		m_ResourceFolders.push_back(root);

		// Recursively fill in root.
		std::string path = "resources";
		ScanFilesAndFolders(m_ResourceFolders[0]);

	}

	void ResourcesPanel::ScanFilesAndFolders(EditorFolder& root)
	{
		for (const auto& entry : std::filesystem::directory_iterator(root.path))
		{
			if (entry.path().has_extension())
			{
				// Is a file
				EditorFile file;
				file.fileName = entry.path().filename().string();
				file.filePath = entry.path().relative_path().string();
				file.fileExtension = file.fileName.substr(file.fileName.find(".") + 1);
				file.fileType = GetFileType(file.fileExtension);
				
				// Add to the folder data.
				root.files.push_back(file);
			}
			else
			{
				// Is a folder
				EditorFolder folder;
				folder.folderName = entry.path().filename().string();
				folder.path = entry.path().relative_path().string();

				// Add to the sub folders.
				root.subFolders.push_back(folder);

				// Iterate recursively.
				ScanFilesAndFolders(root.subFolders.back());
			}
		}

	}

	ResourcesPanel::FileType ResourcesPanel::GetFileType(std::string& extension)
	{
		if (extension.compare(".jpg") == 0 || extension.compare(".jpeg") == 0 || extension.compare(".png") == 0 || extension.compare(".tga") == 0)
			return FileType::TEXTURE;
		else if (extension.compare(".ttf") == 0)
			return FileType::FONT;
	}
}