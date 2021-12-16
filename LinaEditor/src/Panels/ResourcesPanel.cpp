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
#include "Core/RenderEngineBackend.hpp"
#include "Core/InputMappings.hpp"
#include "Core/EditorCommon.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "Utility/EditorUtility.hpp"
#include "Rendering/Shader.hpp"
#include "IconsFontAwesome5.h"
#include "imgui/imgui.h"
#include "imgui/ImGuiFileDialogue/ImGuiFileDialog.h"
#include <filesystem>

#define ROOT_NAME "###Resources"

namespace Lina::Editor
{

	static int s_itemIDCounter = 0;
	static int s_selectedItem = -1;
	static EditorFolder* s_hoveredFolder;
	static EditorFile* s_selectedFile;
	static EditorFolder* s_selectedFolder;


	static ImVec4 s_highlightColor;
	static ImVec4 s_fileNameColor;
	static ImVec4 s_usedFileNameColor;;
	static bool s_highlightColorSet;
	static bool m_drawInternals = true;
	static float s_colorLerpTimestamp;
	static float s_colorLerpDuration = 1.0f;
	static float s_colorLerpItemID;

	void ResourcesPanel::Initialize()
	{
		Lina::Event::EventSystem::Get()->Connect<ETextureReimported, &ResourcesPanel::TextureReimported>(this);
		Lina::Event::EventSystem::Get()->Connect<EMaterialTextureSelected, &ResourcesPanel::MaterialTextureSelected>(this);
		s_highlightColor = ImGui::GetStyleColorVec4(ImGuiCol_Header);
		s_fileNameColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);
		s_usedFileNameColor = s_fileNameColor;

		Utility::Folder root;
		root.m_fullPath = "resources/";
		root.m_name = "resources";
		m_folders.push_back(root);
		Utility::ScanFolder(m_folders[0]);
	}


	void ResourcesPanel::ScanRoot()
	{

		return;
		// Build root.
		EditorFolder root;
		root.m_name = ROOT_NAME;
		root.m_path = "resources";
		m_resourceFolders.push_back(root);

		// Recursively fill in root.
		s_itemIDCounter = -1;
		std::string path = "resources";
		ScanFolder(m_resourceFolders[0]);

		// Load resources	
		LoadFolderResources(m_resourceFolders[0]);
		LoadFolderDependencies(m_resourceFolders[0]);
	}

	void ResourcesPanel::ScanFolder(EditorFolder& root)
	{
		for (const auto& entry : std::filesystem::directory_iterator(root.m_path))
		{
			if (entry.path().has_extension())
			{
				// Is a file
				EditorFile file;
				file.m_name = entry.path().filename().string();
				file.m_pathToFolder = entry.path().parent_path().string() + "/";
				std::string replacedPath = entry.path().relative_path().string();
				std::replace(replacedPath.begin(), replacedPath.end(), '\\', '/');
				file.m_path = replacedPath;
				file.m_extension = file.m_name.substr(file.m_name.find(".") + 1);
				file.m_type = GetFileType(file.m_extension);
				file.m_id = ++s_itemIDCounter;

				// Add to the folder data.
				root.m_files[file.m_id] = file;
			}
			else
			{
				// Is a folder
				EditorFolder folder;
				folder.m_name = entry.path().filename().string();
				std::string replacedPath = entry.path().relative_path().string();
				std::replace(replacedPath.begin(), replacedPath.end(), '\\', '/');
				folder.m_path = replacedPath;
				folder.m_id = ++s_itemIDCounter;
				folder.m_parent = &root;

				// Add to the sub folders.
				root.m_subFolders[folder.m_id] = folder;

				// Iterate recursively.
				ScanFolder(root.m_subFolders[folder.m_id]);
			}
		}
	}

	void ResourcesPanel::Draw()
	{
		if (m_show)
		{
			// Set window properties.
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImVec2 work_area_pos = viewport->GetWorkPos();
			ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
			ImGui::SetNextWindowBgAlpha(1.0f);


			ImGui::Begin(RESOURCES_ID, &m_show, flags);

			float windowWidth = ImGui::GetWindowWidth();
			float windowHeight = ImGui::GetWindowHeight();
			ImVec2 pos = ImGui::GetWindowPos();
			ImVec2 min = ImVec2(0,0);
			ImVec2 max = ImVec2(500, 500);

			ImGui::BeginChild("##res_sb", ImVec2(0, 10));

			ImGui::EndChild();
			//WidgetsUtility::HorizontalDivider(10);

			ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.05f, 0.05f, 0.05f, 1.0f));
			const float yPadding = 0.0f;
			const float xPadding = 0.0f;
			ImGui::SetNextWindowPos(ImVec2(pos.x + xPadding, pos.y + ImGui::GetCursorPosY() + yPadding));
			ImGui::BeginChild("folders", ImVec2((windowWidth * 0.2f) - (2.0f * xPadding), -yPadding * 2.0f), true);
			DrawFolderMenu(m_folders[0], 0.0f);

	

			ImGui::EndChild();
			ImGui::PopStyleColor();
			ImGui::SameLine();
			ImGui::BeginChild("files", ImVec2(windowWidth * 0.8f, windowHeight), true);
			//DrawContents();
			ImGui::EndChild();

			ImGui::End();
		}
	}



	void ResourcesPanel::DrawFolderMenu(Utility::Folder& folder, float offset)
	{
		static bool colorToggle = false;

		ImVec4 childBG= ImGui::GetStyleColorVec4(ImGuiCol_ChildBg);
		m_currentHoveredFolder = nullptr;
		WidgetsUtility::DrawTreeFolder(folder, m_currentSelectedFolder, m_currentHoveredFolder, 22, offset, childBG);
		
		if (folder.m_isOpen)
		{
			for (auto& f : folder.m_folders)
				DrawFolderMenu(f, offset + 15.0f);
		}
		
		return;

		//static ImGuiTreeNodeFlags folderFlagsNotSelected = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
		//static ImGuiTreeNodeFlags folderFlagsSelected = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Selected;
		//static ImGuiTreeNodeFlags fileNodeFlagsNotSelected = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth;
		//static ImGuiTreeNodeFlags fileNodeFlagsSelected = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Selected;
		//
		//ImGuiTreeNodeFlags folderFlags = m_currentSelectedFolder == &folder ? folderFlagsSelected : folderFlagsNotSelected;
		//
		//std::string id = "##" + folder.m_name;
		//bool nodeOpen = ImGui::TreeNodeEx(id.c_str(), folderFlags);
		//ImGui::SameLine();  WidgetsUtility::IncrementCursorPosY(5);
		//WidgetsUtility::Icon(ICON_FA_FOLDER, 0.7f, ImVec4(0.8f, 0.8f, 0.8f, 1.0f));
		//ImGui::SameLine(); WidgetsUtility::IncrementCursorPosX(3); WidgetsUtility::IncrementCursorPosY(-5);
		//ImGui::Text(folder.m_name.c_str());
		//
		//// Click
		//if (ImGui::IsItemClicked())
		//{
		//	m_currentSelectedFile = nullptr;
		//	m_currentSelectedFolder = &folder;
		//}
		//
		//
		//if (nodeOpen)
		//{
		//	//for (auto& f : folder.m_folders)
		//	//{
		//	//	DrawFolderMenu(f);
		//	//
		//	//}
		//	ImGui::TreePop();
		//
		//}
		
	}

	void ResourcesPanel::DrawContents(Utility::Folder& folder)
	{

	}


	void ResourcesPanel::DrawFile(Utility::File& file)
	{

	}



	void ResourcesPanel::DrawContextMenu()
	{
		std::string rootPath = s_hoveredFolder == nullptr ? "resources" : s_hoveredFolder->m_path;

		// Handle Right click popup.
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::BeginMenu("Create"))
			{
				// Build a folder.
				if (ImGui::MenuItem("Folder"))
				{
					std::string folderPath = rootPath + "/NewFolder" + std::to_string(++s_itemIDCounter);
					EditorUtility::CreateFolderInPath(folderPath);
					EditorFolder folder;
					folder.m_path = folderPath;
					folder.m_name = "NewFolder" + std::to_string(s_itemIDCounter);
					folder.m_id = s_itemIDCounter;

					if (s_hoveredFolder != nullptr)
						s_hoveredFolder->m_subFolders[folder.m_id] = folder;
					else
						m_resourceFolders[0].m_subFolders[folder.m_id] = folder;

				}

				ImGui::Separator();

				// Build a material.
				if (ImGui::MenuItem("Material"))
				{
					std::string name = "NewMaterial" + std::to_string(++s_itemIDCounter) + ".mat";
					std::string materialPath = rootPath + "/" + name;

					EditorFile file;
					file.m_path = materialPath;
					file.m_pathToFolder = rootPath + "/";
					file.m_name = name;
					file.m_extension = "mat";
					file.m_type = FileType::Material;
					file.m_id = ++s_itemIDCounter;

					Graphics::Material& m = Lina::Graphics::Material::CreateMaterial(Graphics::RenderEngineBackend::Get()->GetDefaultShader(), file.m_path);
					Graphics::Material::SaveMaterialData(m, materialPath);

					if (s_hoveredFolder != nullptr)
						s_hoveredFolder->m_files[file.m_id] = file;
					else
						m_resourceFolders[0].m_files[file.m_id] = file;
				}

				ImGui::EndMenu();
			}

			ImGui::EndPopup();
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


		// Draw folders.
		for (std::map<int, EditorFolder>::iterator it = folder.m_subFolders.begin(); it != folder.m_subFolders.end();)
		{
			// Skip drawing if internal folders.
			if (!m_drawInternals)
			{
				if (it->second.m_parent != nullptr && it->second.m_parent->m_name.compare(ROOT_NAME) == 0)
				{
					if (it->second.m_name.compare("engine") == 0 || it->second.m_name.compare("editor") == 0)
					{
						it++;
						continue;
					}
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

			if (it->second.m_markedForForceOpen)
			{
				ImGui::SetNextItemOpen(true);
				it->second.m_markedForForceOpen = false;
			}

			std::string id = "##" + (it->second).m_name;
			bool nodeOpen = ImGui::TreeNodeEx(id.c_str(), folderFlags);
			ImGui::SameLine();  WidgetsUtility::IncrementCursorPosY(5);
			WidgetsUtility::Icon(ICON_FA_FOLDER, 0.7f, ImVec4(0.9f, 0.83f, 0.0f, 1.0f));
			ImGui::SameLine(); WidgetsUtility::IncrementCursorPosX(3); WidgetsUtility::IncrementCursorPosY(-5);
			ImGui::Text((it->second).m_name.c_str());

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
			if (it->second.m_markedForErase)
			{
				// Delete directory.
				EditorUtility::DeleteDirectory(it->second.m_path);

				// Unload the resources & erase.
				UnloadFileResource(it->second);
				folder.m_files.erase(it++);
				continue;
			}

			ImGuiTreeNodeFlags fileFlags = it->second.m_id == s_selectedItem ? fileNodeFlagsSelected : fileNodeFlagsNotSelected;

			// Highlight.
			if (it->second.m_markedForHighlight)
			{
				it->second.m_markedForHighlight = false;
				s_usedFileNameColor = s_highlightColor;
				s_highlightColorSet = true;
				s_colorLerpItemID = it->second.m_id;
				s_colorLerpTimestamp = ImGui::GetTime();
			}

			if (s_highlightColorSet)
			{
				float t = ImGui::GetTime() - s_colorLerpTimestamp;
				float remapped = Math::Remap(t, 0.0f, s_colorLerpDuration, 0.0f, 1.0f);

				s_usedFileNameColor.x = Math::Lerp(s_highlightColor.x, s_fileNameColor.x, remapped);
				s_usedFileNameColor.y = Math::Lerp(s_highlightColor.y, s_fileNameColor.y, remapped);
				s_usedFileNameColor.z = Math::Lerp(s_highlightColor.z, s_fileNameColor.z, remapped);
				s_usedFileNameColor.w = Math::Lerp(s_highlightColor.w, s_fileNameColor.w, remapped);

				if (t > s_colorLerpDuration - 0.1f)
				{
					s_highlightColorSet = false;
					s_usedFileNameColor = s_fileNameColor;
				}
			}

			if (it->second.m_id == s_colorLerpItemID)
				ImGui::PushStyleColor(ImGuiCol_Text, s_usedFileNameColor);
			else
				ImGui::PushStyleColor(ImGuiCol_Text, s_fileNameColor);

			// Node
			bool nodeOpen = ImGui::TreeNodeEx(it->second.m_name.c_str(), fileFlags);

			// Drag drop
			if (it->second.m_type == FileType::Texture2D)
			{
				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
				{
					// Set payload to carry the texture;
					StringIDType id = Lina::Graphics::Texture::GetTexture(it->second.m_path).GetSID();
					ImGui::SetDragDropPayload(RESOURCES_MOVETEXTURE_ID, &id, sizeof(StringIDType));

					// Display preview 
					ImGui::Text("Assign ");
					ImGui::EndDragDropSource();
				}
			}
			if (it->second.m_type == FileType::HDRI)
			{
				// TODO: HDRI Support & auto capturing.
			}
			else if (it->second.m_type == FileType::Material)
			{
				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
				{
					// Set payload to carry the texture;
					StringIDType id = Lina::Graphics::Material::GetMaterial(it->second.m_path).GetID();
					ImGui::SetDragDropPayload(RESOURCES_MOVEMATERIAL_ID, &id, sizeof(StringIDType));

					// Display preview 
					ImGui::Text("Assign ");
					ImGui::EndDragDropSource();
				}
			}
			else if (it->second.m_type == FileType::Model)
			{
				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
				{
					// Set payload to carry the texture;
					StringIDType id = Lina::Graphics::Model::GetModel(it->second.m_path).GetID();
					ImGui::SetDragDropPayload(RESOURCES_MOVEMESH_ID, &id, sizeof(StringIDType));

					// Display preview 
					ImGui::Text("Assign ");
					ImGui::EndDragDropSource();
				}
			}
			else if (it->second.m_type == FileType::Shader)
			{
				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
				{
					// Set payload to carry the texture;
					StringIDType id = Lina::Graphics::Shader::GetShader(it->second.m_path).GetSID();
					ImGui::SetDragDropPayload(RESOURCES_MOVESHADER_ID, &id, sizeof(uint32));

					// Display preview 
					ImGui::Text("Assign ");
					ImGui::EndDragDropSource();
				}
			}


			ImGui::PopStyleColor();

			// Click.
			if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			{
				s_selectedItem = it->second.m_id;
				s_selectedFolder = nullptr;
				s_selectedFile = &it->second;
				
				// Notify properties panel of file selection.
				if (it->second.m_type == FileType::Texture2D)
					Lina::Event::EventSystem::Get()->Trigger<ETextureSelected>(ETextureSelected{ &Lina::Graphics::Texture::GetTexture(it->second.m_path) });
				else if (it->second.m_type == FileType::Model)
					Lina::Event::EventSystem::Get()->Trigger<EModelSelected>(EModelSelected{ &Lina::Graphics::Model::GetModel(it->second.m_path) });
				else if (it->second.m_type == FileType::Material)
					Lina::Event::EventSystem::Get()->Trigger<EMaterialSelected>(EMaterialSelected{ &it->second, &Lina::Graphics::Material::GetMaterial(it->second.m_path) });
			}

			if (nodeOpen)
				ImGui::TreePop();
			++it;

		}

		// Deselect.
		if (!ImGui::IsAnyItemHovered() && ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			Lina::Event::EventSystem::Get()->Trigger<EEntityUnselected>(EEntityUnselected{});
			s_selectedItem = -1;
		}

		// Delete item.
		if (ImGui::IsKeyPressed(Input::InputCode::Delete) && s_selectedItem != -1 && ImGui::IsWindowFocused())
		{
			if (s_selectedFolder != nullptr)
				s_selectedFolder->m_markedForErase = true;
			if (s_selectedFile != nullptr)
				s_selectedFile->m_markedForErase = true;

			// Deselect
			Lina::Event::EventSystem::Get()->Trigger<EEntityUnselected>(EEntityUnselected{});
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

			if (file.m_type == FileType::Texture2D)
			{
				bool textureExists = Lina::Graphics::Texture::TextureExists(file.m_path);

				if (!textureExists)
				{
					//Lina::Graphics::SamplerParameters samplerParams;
					//std::string samplerParamsPath = file.m_pathToFolder + EditorUtility::RemoveExtensionFromFilename(file.m_name) + ".samplerparams";
					//
					//if (Lina::Utility::FileExists(samplerParamsPath))
					//	samplerParams = Lina::Graphics::Texture::LoadParameters(samplerParamsPath);
					//
					//Lina::Graphics::Texture::CreateTexture2D(file.m_path, samplerParams, false, false, samplerParamsPath);
					//
					//Lina::Graphics::Texture::SaveParameters(samplerParamsPath, samplerParams);
				}
			}
			if (file.m_type == FileType::HDRI)
			{
				bool textureExists = Lina::Graphics::Texture::TextureExists(file.m_path);

				if (!textureExists)
					Lina::Graphics::Texture::CreateTextureHDRI(file.m_path);
			}
			if (file.m_type == FileType::Material)
			{
				//bool materialExists = Lina::Graphics::Material::MaterialExists(file.m_path);
				//if (!materialExists)
				//	Lina::Graphics::Material::LoadMaterialFromFile(file.m_path);

			}
			else if (file.m_type == FileType::Model)
			{
				bool meshExists = Lina::Graphics::Model::ModelExists(file.m_path);

				if (!meshExists)
				{
					//Lina::Graphics::ModelParameters meshParams;
					//std::string meshParamsPath = file.m_pathToFolder + EditorUtility::RemoveExtensionFromFilename(file.m_name) + ".modelparams";
					//
					//if (Lina::Utility::FileExists(meshParamsPath))
					//	meshParams = Lina::Graphics::Model::LoadParameters(meshParamsPath);
					//
					//Lina::Graphics::Model::CreateModel(file.m_path, meshParams, -1, meshParamsPath);

					//Lina::Graphics::Model::SaveParameters(meshParamsPath, meshParams);
				}
			}
		}

		// Recursively load subfolders.
		for (std::map<int, EditorFolder>::iterator it = folder.m_subFolders.begin(); it != folder.m_subFolders.end(); ++it)
			LoadFolderResources(it->second);
	}

	void ResourcesPanel::LoadFolderDependencies(EditorFolder& folder)
	{
		Lina::Graphics::RenderEngineBackend* renderEngine = Lina::Graphics::RenderEngineBackend::Get();

		// Load files.
		for (std::map<int, EditorFile>::iterator it = folder.m_files.begin(); it != folder.m_files.end(); ++it)
		{
			EditorFile& file = it->second;

			// SKIP FOR NOW BC WE NEED TO MAKE SURE WE HANDLE BOTH ENGINE CREATION & EDITOR CREATION
			if (file.m_type == FileType::Texture2D)
			{

			}
			if (file.m_type == FileType::Material)
			{
				// Lina::Graphics::Material& mat = Lina::Graphics::Material::GetMaterial(file.m_path);
				// mat.PostLoadMaterialData();
			}
			else if (file.m_type == FileType::Model)
			{

			}
		}

		// Recursively load subfolders.
		for (std::map<int, EditorFolder>::iterator it = folder.m_subFolders.begin(); it != folder.m_subFolders.end(); ++it)
			LoadFolderDependencies(it->second);
	}

	void ResourcesPanel::UnloadFileResource(EditorFile& file)
	{
		if (file.m_type == FileType::Texture2D)
			Lina::Graphics::Texture::UnloadTextureResource(Lina::Graphics::Texture::GetTexture(file.m_path).GetSID());
		else if(file.m_type == FileType::HDRI)
			Lina::Graphics::Texture::UnloadTextureResource(Lina::Graphics::Texture::GetTexture(file.m_path).GetSID());
		else if (file.m_type == FileType::Model)
			Lina::Graphics::Model::UnloadModel(Lina::Graphics::Model::GetModel(file.m_path).GetID());
		else if (file.m_type == FileType::Material)
			Lina::Graphics::Material::UnloadMaterialResource(Lina::Graphics::Material::GetMaterial(file.m_path).GetID());
	}

	void ResourcesPanel::UnloadFileResourcesInFolder(EditorFolder& folder)
	{
		for (std::map<int, EditorFile>::iterator it = folder.m_files.begin(); it != folder.m_files.end(); ++it)
			UnloadFileResource(it->second);
	}

	bool ResourcesPanel::ExpandFileResource(EditorFolder& folder, const std::string& path, FileType type)
	{
		for (auto& file : folder.m_files)
		{
			if (type == FileType::Unknown || (type != FileType::Unknown && file.second.m_type == type))
			{
				if (file.second.m_path.compare(path) == 0)
				{
					file.second.m_markedForHighlight = true;
					folder.m_markedForForceOpen = true;

					EditorFolder* parent = folder.m_parent;
					while (parent != nullptr)
					{
						parent->m_markedForForceOpen = true;

						parent = parent->m_parent;
					}

					return true;
				}
			}
		}

		for (auto& subFolder : folder.m_subFolders)
		{
			if (ExpandFileResource(subFolder.second, path, type))
				break;
		}
	}

	FileType ResourcesPanel::GetFileType(std::string& extension)
	{
		if (extension.compare("jpg") == 0 || extension.compare("jpeg") == 0 || extension.compare("png") == 0 || extension.compare("tga") == 0)
			return FileType::Texture2D;
		else if (extension.compare("hdr") == 0)
			return FileType::HDRI;
		else if (extension.compare("ttf") == 0)
			return FileType::Font;
		else if (extension.compare("obj") == 0 || extension.compare("fbx") == 0 || extension.compare("3ds") == 0)
			return FileType::Model;
		else if (extension.compare("mat") == 0)
			return FileType::Material;
		else if (extension.compare("glsl") == 0)
			return FileType::Shader;
		else
			return FileType::Unknown;
	}

	void ResourcesPanel::MaterialTextureSelected(EMaterialTextureSelected ev)
	{
		ExpandFileResource(m_resourceFolders[0], ev.m_texture->GetPath(), FileType::Texture2D);
	}

	void ResourcesPanel::TextureReimported(ETextureReimported ev)
	{
		VerifyMaterialFiles(m_resourceFolders[0], ev);
	}

	bool ResourcesPanel::VerifyMaterialFiles(EditorFolder& folder, ETextureReimported ev)
	{
		// Iterate the files in this folder first & see if there is a match.
		for (auto file : folder.m_files)
		{
			if (file.second.m_type == FileType::Material)
			{
				Lina::Graphics::Material& mat = Lina::Graphics::Material::GetMaterial(file.second.m_path);
				for (auto sampler : mat.m_sampler2Ds)
				{
					if (sampler.second.m_boundTexture == ev.m_selected)
					{
						mat.SetTexture(sampler.first, ev.m_reimported, sampler.second.m_bindMode);
						return true;
					}
				}

			}
		}

		// Iterate subfolders.
		for (auto folder : folder.m_subFolders)
		{
			if (VerifyMaterialFiles(folder.second, ev))
				return true;
		}

		return false;
	}
}