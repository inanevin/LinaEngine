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

/*
Class: ResourcesPanel

Displays the project folders & files, creates functionality to edit, add, remove them.

Timestamp: 6/5/2020 12:54:52 AM
*/

#pragma once

#ifndef ResourcesPanel_HPP
#define ResourcesPanel_HPP

#include "Panels/EditorPanel.hpp"
#include "Utility/EditorUtility.hpp"
#include <map>

namespace LinaEngine
{
	namespace Graphics
	{
		class RenderEngine;
	}
}


namespace LinaEditor 
{

	enum class FileType
	{
		UNKNOWN,
		TEXTURE2D,
		MESH,
		FONT,
		MATERIAL
	};


	struct EditorFile
	{
		std::string name;
		std::string extension;
		std::string path;
		FileType type;
		int id;
		bool markedForErase = false;
	};


	class EditorFolder
	{
	public:
		EditorFolder() {};
		~EditorFolder() {};
		std::string m_path = "";
		std::string name = "";
		std::map<int, EditorFolder> m_subFolders;
		std::map<int, EditorFile> m_files;
		int m_id = 0;
		bool m_markedForErase = false;
		EditorFolder* m_parent = nullptr;
	};

	class GUILayer;
	class PropertiesPanel;

	class ResourcesPanel : public EditorPanel
	{

	public:

		
		ResourcesPanel(LinaEngine::Vector2 position, LinaEngine::Vector2 size, GUILayer& guiLayer) :EditorPanel(position, size, guiLayer) {};
		virtual ~ResourcesPanel() {};
	
		virtual void Draw(float frameTime) override;
		void Setup();

	private:
	

		void DrawContent();
		void ScanRoot();
		void ScanFolder(EditorFolder& folder);
		void DrawFolder(EditorFolder& folder, bool isRoot = false);
		void LoadFolderResources(EditorFolder& folder);
		void UnloadFileResource(EditorFile& file);
		void UnloadFileResourcesInFolder(EditorFolder& folder);
		FileType GetFileType(std::string& extension);

	private:

		LinaEngine::Graphics::RenderEngine* m_RenderEngine = nullptr;
		PropertiesPanel* m_PropertiesPanel = nullptr;
		std::vector<EditorFolder> m_resourceFolders;
	};
}

#endif
