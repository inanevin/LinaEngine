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
#include "Rendering/Material.hpp"
#include "Input/InputMappings.hpp"
#include "imgui.h"
#include "imgui/ImGuiFileDialogue/ImGuiFileDialog.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <filesystem>


namespace LinaEditor
{


	static int itemIDCounter = 0;
	static int selectedItem = -1;
	static EditorFolder* hoveredFolder;
	static EditorFile* selectedFile;
	static EditorFolder* selectedFolder;

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

			DrawContent();
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

	void ResourcesPanel::DrawContent()
	{
		std::string rootPath = hoveredFolder == nullptr ? "resources" : hoveredFolder->path;

		// Handle Right click popup.
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::BeginMenu("Create"))
			{
				// Create a folder.
				if (ImGui::MenuItem("Folder"))
				{
					std::string folderPath = rootPath + "/NewFolder"+ std::to_string(++itemIDCounter);
					EditorUtility::CreateFolderInPath(folderPath);
					EditorFolder folder;
					folder.path = folderPath;
					folder.name = "NewFolder" + std::to_string(itemIDCounter);
					folder.id = itemIDCounter;
					
					if (hoveredFolder != nullptr)
						hoveredFolder->subFolders[folder.id] = folder;
					else
						m_ResourceFolders[0].subFolders[folder.id] = folder;

				}

				ImGui::Separator();

				// Create a material.
				if (ImGui::MenuItem("Material"))
				{
					std::string name = "NewMaterial" + std::to_string(++itemIDCounter) + ".mat";
					std::string materialPath = rootPath + "/" + name;
					
					EditorFile file;
					file.path = materialPath;
					file.name = name;
					file.extension = "mat";
					file.type = FileType::MATERIAL;
					file.id = ++itemIDCounter;

					Graphics::Material& m = m_RenderEngine->CreateMaterial(file.id, Graphics::Shaders::PBR_LIT);
					EditorUtility::SerializeMaterial(materialPath, m);


					if (hoveredFolder != nullptr)
						hoveredFolder->files[file.id] = file;
					else
						m_ResourceFolders[0].files[file.id] = file;
				}

				ImGui::EndMenu();
			}

			ImGui::EndPopup();
		}
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
				root.files[file.id] = file;
			}
			else
			{
				// Is a folder
				EditorFolder folder;
				folder.name = entry.path().filename().string();
				folder.path = entry.path().relative_path().string();
				folder.id = ++itemIDCounter;

				// Add to the sub folders.
				root.subFolders[folder.id] = folder;

				// Iterate recursively.
				ScanFolder(root.subFolders[folder.id]);
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
		for (std::map<int, EditorFolder>::iterator it = folder.subFolders.begin(); it != folder.subFolders.end();)
		{
			if (it->second.markedForErase)
			{
				// Delete directory.
				EditorUtility::DeleteDirectory(it->second.path);

				// Nullout reference.
				if (hoveredFolder == &it->second)
					hoveredFolder = nullptr;

				// Unload the contained resources & erase.
				UnloadFileResourcesInFolder(it->second);
				folder.subFolders.erase(it++);
				continue;
			}
			
			ImGuiTreeNodeFlags folderFlags = (it->second).id == selectedItem ? folderFlagsSelected : folderFlagsNotSelected;
			bool nodeOpen = ImGui::TreeNodeEx((it->second).name.c_str(), folderFlags);

			// Click
			if (ImGui::IsItemClicked())
			{
				selectedItem = (it->second).id;
				selectedFile = nullptr;
				selectedFolder = &(it->second);
			}

			// Hover.
			if (ImGui::IsWindowHovered() && ImGui::IsItemHovered())
				hoveredFolder = &(it->second);

			if (nodeOpen)
			{
				DrawFolder(it->second);
				ImGui::TreePop();
			}
			++it;

		}

		// Draw files.
		for (std::map<int, EditorFile>::iterator it = folder.files.begin(); it != folder.files.end();)
		{
			if (it->second.markedForErase)
			{
				// Delete directory.
				EditorUtility::DeleteDirectory(it->second.path);

				// Unload the resources & erase.
				UnloadFileResource(it->second);
				folder.files.erase(it++);
				continue;
			}

			ImGuiTreeNodeFlags fileFlags = it->second.id == selectedItem ? fileNodeFlagsSelected : fileNodeFlagsNotSelected;
			bool nodeOpen = ImGui::TreeNodeEx(it->second.name.c_str(), fileFlags);

			// Click.
			if (ImGui::IsItemClicked())
			{
				selectedItem = it->second.id;
				selectedFolder = nullptr;
				selectedFile = &it->second;

				// Notify properties panel of file selection.
				if (it->second.type == FileType::TEXTURE2D)
					m_PropertiesPanel->Texture2DSelected(&m_RenderEngine->GetTexture(it->second.id), it->second.id, it->second.path);
				else if (it->second.type == FileType::MESH)
					m_PropertiesPanel->MeshSelected(&m_RenderEngine->GetMesh(it->second.id), it->second.id, it->second.path);
			}

			if (nodeOpen)
				ImGui::TreePop();
			++it;

		}


		// Deselect.
		if (!ImGui::IsAnyItemHovered() && ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			m_PropertiesPanel->Unselect();
			selectedItem = -1;
		}

		// Delete item.
		if (ImGui::IsKeyPressed(Input::InputCode::Delete) && selectedItem != -1)
		{
			if (selectedFolder != nullptr)
				selectedFolder->markedForErase = true;
			if (selectedFile != nullptr)
				selectedFile->markedForErase = true;
			// Deselect
			m_PropertiesPanel->Unselect();
			selectedItem = -1;
		}

		if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered())
			hoveredFolder = nullptr;
	}

	void ResourcesPanel::LoadFolderResources(EditorFolder& folder)
	{
		// Load files.
		for (std::map<int, EditorFile>::iterator it = folder.files.begin(); it != folder.files.end(); ++it)
		{
			EditorFile& file = it->second;

			if (file.type == FileType::TEXTURE2D)
				m_RenderEngine->CreateTexture2D(file.id, file.path);
			else if (file.type == FileType::MESH)
				m_RenderEngine->CreateMesh(file.id, file.path);
		}

		// Recursively load subfolders.
		for (std::map<int, EditorFolder>::iterator it = folder.subFolders.begin(); it != folder.subFolders.end(); ++it)
			LoadFolderResources(it->second);
	}

	void ResourcesPanel::UnloadFileResource(EditorFile& file)
	{
		if (file.type == FileType::TEXTURE2D)
			m_RenderEngine->UnloadTextureResource(file.id);
		else if (file.type == FileType::MESH)
			m_RenderEngine->UnloadMeshResource(file.id);
		else if (file.type == FileType::MATERIAL)
			m_RenderEngine->UnloadMaterialResource(file.id);
	}

	void ResourcesPanel::UnloadFileResourcesInFolder(EditorFolder& folder)
	{
		for (std::map<int, EditorFile>::iterator it = folder.files.begin(); it != folder.files.end(); ++it)
			UnloadFileResource(it->second);
	}

	FileType ResourcesPanel::GetFileType(std::string& extension)
	{
		if (extension.compare("jpg") == 0 || extension.compare("jpeg") == 0 || extension.compare("png") == 0 || extension.compare("tga") == 0)
			return FileType::TEXTURE2D;
		else if (extension.compare("ttf") == 0)
			return FileType::FONT;
		else if (extension.compare("obj") == 0 || extension.compare("fbx") == 0 || extension.compare("3ds") == 0)
			return FileType::MESH;
		else
			return FileType::UNKNOWN;
	}
}