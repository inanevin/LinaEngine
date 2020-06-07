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
Timestamp: 6/5/2020 12:54:52 AM

*/
#pragma once

#ifndef ResourcesPanel_HPP
#define ResourcesPanel_HPP

#include "Panels/EditorPanel.hpp"
#include "Utility/EditorUtility.hpp"

namespace LinaEditor 
{
	class ResourcesPanel : public EditorPanel
	{

	public:

		enum class FileType
		{
			UNKNOWN,
			TEXTURE,
			FONT,
			MATERIAL
		};

		struct EditorFile
		{
			std::string fileName;
			std::string fileExtension;
			std::string filePath;
			FileType fileType;
		};

		struct EditorFolder
		{
			std::string path;
			std::string folderName;
			std::vector<EditorFolder> subFolders;
			std::vector<EditorFile> files;
		};
		
		
		ResourcesPanel(Vector2 position, Vector2 size, class GUILayer& guiLayer) :EditorPanel(position, size, guiLayer) {};
		virtual ~ResourcesPanel() {};
	
		virtual void Draw() override;
		void Setup();

	private:
	
		void ScanFilesAndFolders(EditorFolder& folder);
		FileType GetFileType(std::string& extension);
	private:

		std::vector<EditorFolder> m_ResourceFolders;
	};
}

#endif
