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

#include "Panels/ResourcesPanel.hpp"
#include "Core/GUILayer.hpp"
#include "Panels/PropertiesPanel.hpp"
#include "Core/Application.hpp"
#include "Core/EditorApplication.hpp"
#include "Rendering/RenderEngine.hpp"
#include "Input/InputMappings.hpp"
#include "Core/EditorCommon.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "Utility/EditorUtility.hpp"
#include "IconsFontAwesome5.h"
#include "imgui/imgui.h"
#include "imgui/ImGuiFileDialogue/ImGuiFileDialog.h"
#include <filesystem>

#define ROOT_NAME "###Resources"

namespace LinaEditor
{

	static int s_itemIDCounter = 0;
	static int s_selectedItem = -1;
	static EditorFolder* s_hoveredFolder;
	static EditorFile* s_selectedFile;
	static EditorFolder* s_selectedFolder;

	void ResourcesPanel::Setup()
	{
		ScanRoot();
	}

	void ResourcesPanel::Draw(float frameTime)
	{
		if (m_show)
		{
			// Set window properties.
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImVec2 work_area_pos = viewport->GetWorkPos();
			ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
			ImGui::SetNextWindowBgAlpha(1.0f);

			ImGui::Begin("Resources", &m_show, flags);
			WidgetsUtility::DrawShadowedLine(5);
			DrawContent();
			DrawFolder(m_resourceFolders[0], true);

			ImGui::End();
		}
	}


	void ResourcesPanel::DrawContent()
	{
		std::string rootPath = s_hoveredFolder == nullptr ? "resources" : s_hoveredFolder->m_path;

		// Handle Right click popup.
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::BeginMenu("Create"))
			{
				// Create a folder.
				if (ImGui::MenuItem("Folder"))
				{
					std::string folderPath = rootPath + "/NewFolder" + std::to_string(++s_itemIDCounter);
					EditorUtility::CreateFolderInPath(folderPath);
					EditorFolder folder;
					folder.m_path = folderPath;
					folder.name = "NewFolder" + std::to_string(s_itemIDCounter);
					folder.m_id = s_itemIDCounter;

					if (s_hoveredFolder != nullptr)
						s_hoveredFolder->m_subFolders[folder.m_id] = folder;
					else
						m_resourceFolders[0].m_subFolders[folder.m_id] = folder;

				}

				ImGui::Separator();

				// Create a material.
				if (ImGui::MenuItem("Material"))
				{
					std::string name = "NewMaterial" + std::to_string(++s_itemIDCounter) + ".mat";
					std::string materialPath = rootPath + "/" + name;

					EditorFile file;
					file.path = materialPath;
					file.name = name;
					file.extension = "mat";
					file.type = FileType::Material;
					file.id = ++s_itemIDCounter;

					Graphics::Material& m = LinaEngine::Application::GetRenderEngine().CreateMaterial(file.id, Graphics::Shaders::PBR_LIT);
					EditorUtility::SerializeMaterial(materialPath, m);

					if (s_hoveredFolder != nullptr)
						s_hoveredFolder->m_files[file.id] = file;
					else
						m_resourceFolders[0].m_files[file.id] = file;
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
		root.name = ROOT_NAME;
		root.m_path = "resources";
		m_resourceFolders.push_back(root);

		// Recursively fill in root.
		s_itemIDCounter = -1;
		std::string path = "resources";
		ScanFolder(m_resourceFolders[0]);

		// Load resources	
		LoadFolderResources(m_resourceFolders[0]);
	}

	void ResourcesPanel::ScanFolder(EditorFolder& root)
	{
		for (const auto& entry : std::filesystem::directory_iterator(root.m_path))
		{
			if (entry.path().has_extension())
			{
				// Is a file
				EditorFile file;
				file.name = entry.path().filename().string();
				file.path = entry.path().relative_path().string();
				file.extension = file.name.substr(file.name.find(".") + 1);
				file.type = GetFileType(file.extension);
				file.id = ++s_itemIDCounter;

				// Add to the folder data.
				root.m_files[file.id] = file;
			}
			else
			{
				// Is a folder
				EditorFolder folder;
				folder.name = entry.path().filename().string();
				folder.m_path = entry.path().relative_path().string();
				folder.m_id = ++s_itemIDCounter;
				folder.m_parent = &root;

				// Add to the sub folders.
				root.m_subFolders[folder.m_id] = folder;

				// Iterate recursively.
				ScanFolder(root.m_subFolders[folder.m_id]);
			}
		}
	}

	void ResourcesPanel::DrawFolder(EditorFolder& folder, bool isRoot)
	{
		static ImGuiTreeNodeFlags folderFlagsNotSelected = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
		static ImGuiTreeNodeFlags folderFlagsSelected = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Selected;
		static ImGuiTreeNodeFlags fileNodeFlagsNotSelected = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth;
		static ImGuiTreeNodeFlags fileNodeFlagsSelected = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Selected;


		WidgetsUtility::IncrementCursorPosY(11);
		WidgetsUtility::ItemSpacingX(0);

		if (!isRoot)
			WidgetsUtility::IncrementCursorPosY(-11);
		else
		{


		}

		// Draw folders.
		for (std::map<int, EditorFolder>::iterator it = folder.m_subFolders.begin(); it != folder.m_subFolders.end();)
		{
			// Skip drawing if internal folders.
			if (it->second.m_parent != nullptr && it->second.m_parent->name.compare(ROOT_NAME) == 0)
			{
				if (it->second.name.compare("engine") == 0 || it->second.name.compare("editor") == 0)
				{
					it++;
					continue;
				}				
			}
			
			WidgetsUtility::IncrementCursorPosX(4);

			if (it->second.m_markedForErase)
			{
				// Delete directory.
				EditorUtility::DeleteDirectory(it->second.m_path);

				// Nullout reference.
				if (s_hoveredFolder == &it->second)
					s_hoveredFolder = nullptr;

				// Unload the contained resources & erase.
				UnloadFileResourcesInFolder(it->second);
				folder.m_subFolders.erase(it++);
				continue;
			}

			ImGuiTreeNodeFlags folderFlags = (it->second).m_id == s_selectedItem ? folderFlagsSelected : folderFlagsNotSelected;
			std::string id = "##" + (it->second).name;
			bool nodeOpen = ImGui::TreeNodeEx(id.c_str(), folderFlags);
			ImGui::SameLine();  WidgetsUtility::IncrementCursorPosY(5);
			WidgetsUtility::Icon(ICON_FA_FOLDER, 0.7f, ImVec4(0.9f, 0.83f, 0.0f, 1.0f));
			ImGui::SameLine(); WidgetsUtility::IncrementCursorPosX(3); WidgetsUtility::IncrementCursorPosY(-5);
			ImGui::Text((it->second).name.c_str());

			// Click
			if (ImGui::IsItemClicked())
			{
				s_selectedItem = (it->second).m_id;
				s_selectedFile = nullptr;
				s_selectedFolder = &(it->second);
			}

			// Hover.
			if (ImGui::IsWindowHovered() && ImGui::IsItemHovered())
				s_hoveredFolder = &(it->second);

			if (nodeOpen)
			{
				DrawFolder(it->second);
				ImGui::TreePop();
			}
			++it;

		}
		WidgetsUtility::PopStyleVar();


		// Draw files.
		for (std::map<int, EditorFile>::iterator it = folder.m_files.begin(); it != folder.m_files.end();)
		{
			WidgetsUtility::IncrementCursorPosX(-9);
			if (it->second.markedForErase)
			{
				// Delete directory.
				EditorUtility::DeleteDirectory(it->second.path);

				// Unload the resources & erase.
				UnloadFileResource(it->second);
				folder.m_files.erase(it++);
				continue;
			}

			ImGuiTreeNodeFlags fileFlags = it->second.id == s_selectedItem ? fileNodeFlagsSelected : fileNodeFlagsNotSelected;
			bool nodeOpen = ImGui::TreeNodeEx(it->second.name.c_str(), fileFlags);

			// Click.
			if (ImGui::IsItemClicked())
			{
				s_selectedItem = it->second.id;
				s_selectedFolder = nullptr;
				s_selectedFile = &it->second;

				// Notify properties panel of file selection.
				if (it->second.type == FileType::Texture2D)
					EditorApplication::GetEditorDispatcher().DispatchAction<LinaEngine::Graphics::Texture*>(LinaEngine::Action::ActionType::TextureSelected, &LinaEngine::Application::GetRenderEngine().GetTexture(it->second.path));
				else if (it->second.type == FileType::Mesh)
					EditorApplication::GetEditorDispatcher().DispatchAction<LinaEngine::Graphics::Mesh*>(LinaEngine::Action::ActionType::MeshSelected, &LinaEngine::Application::GetRenderEngine().GetMesh(it->second.path));
				else if (it->second.type == FileType::Material)
					EditorApplication::GetEditorDispatcher().DispatchAction<LinaEngine::Graphics::Material*>(LinaEngine::Action::ActionType::MaterialSelected, &LinaEngine::Application::GetRenderEngine().GetMaterial(it->second.path));
			}

			if (nodeOpen)
				ImGui::TreePop();
			++it;

		}


		// Deselect.
		if (!ImGui::IsAnyItemHovered() && ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			EditorApplication::GetEditorDispatcher().DispatchAction<void*>(LinaEngine::Action::ActionType::Unselect, 0);
			s_selectedItem = -1;
		}

		// Delete item.
		if (ImGui::IsKeyPressed(Input::InputCode::Delete) && s_selectedItem != -1)
		{
			if (s_selectedFolder != nullptr)
				s_selectedFolder->m_markedForErase = true;
			if (s_selectedFile != nullptr)
				s_selectedFile->markedForErase = true;

			// Deselect
			EditorApplication::GetEditorDispatcher().DispatchAction<void*>(LinaEngine::Action::ActionType::Unselect, 0);
			s_selectedItem = -1;
		}

		if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered())
			s_hoveredFolder = nullptr;
	}

	void ResourcesPanel::LoadFolderResources(EditorFolder& folder)
	{
		// Load files.
		for (std::map<int, EditorFile>::iterator it = folder.m_files.begin(); it != folder.m_files.end(); ++it)
		{
			EditorFile& file = it->second;

			// SKIP FOR NOW BC WE NEED TO MAKE SURE WE HANDLE BOTH ENGINE CREATION & EDITOR CREATION
			if (file.type == FileType::Texture2D)
				LinaEngine::Application::GetRenderEngine().CreateTexture2D(file.path);
			else if (file.type == FileType::Mesh)
				LinaEngine::Application::GetRenderEngine().CreateMesh(file.path);
		}

		// Recursively load subfolders.
		for (std::map<int, EditorFolder>::iterator it = folder.m_subFolders.begin(); it != folder.m_subFolders.end(); ++it)
			LoadFolderResources(it->second);
	}

	void ResourcesPanel::UnloadFileResource(EditorFile& file)
	{
		if (file.type == FileType::Texture2D)
			LinaEngine::Application::GetRenderEngine().UnloadTextureResource(file.id);
		else if (file.type == FileType::Mesh)
			LinaEngine::Application::GetRenderEngine().UnloadMeshResource(file.id);
		else if (file.type == FileType::Material)
			LinaEngine::Application::GetRenderEngine().UnloadMaterialResource(file.id);
	}

	void ResourcesPanel::UnloadFileResourcesInFolder(EditorFolder& folder)
	{
		for (std::map<int, EditorFile>::iterator it = folder.m_files.begin(); it != folder.m_files.end(); ++it)
			UnloadFileResource(it->second);
	}

	FileType ResourcesPanel::GetFileType(std::string& extension)
	{
		if (extension.compare("jpg") == 0 || extension.compare("jpeg") == 0 || extension.compare("png") == 0 || extension.compare("tga") == 0)
			return FileType::Texture2D;
		else if (extension.compare("ttf") == 0)
			return FileType::Font;
		else if (extension.compare("obj") == 0 || extension.compare("fbx") == 0 || extension.compare("3ds") == 0)
			return FileType::Mesh;
		else
			return FileType::Unknown;
	}
}