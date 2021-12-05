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
Class: EditorUtility

Various utility functions for the editor.

Timestamp: 5/9/2020 1:22:23 
*/

#pragma once

#ifndef EditorUtility_HPP
#define EditorUtility_HPP

#include <string>
#include <vector>
#include <map>

namespace LinaEngine
{
	namespace Graphics
	{
		class Material;
	}
}

struct ImVec4;

namespace LinaEditor
{


	enum class FileType
	{
		Unknown,
		Texture2D,
		Mesh,
		Font,
		Material,
		Shader
	};


	struct EditorFile
	{
		std::string m_name;
		std::string m_extension;
		std::string m_path;
		std::string m_pathToFolder;
		FileType m_type;
		int m_id;
		bool m_markedForErase = false;
		bool m_markedForHighlight = false;
	};


	class EditorFolder
	{
	public:

		EditorFolder() {};
		~EditorFolder() {};
		std::string m_path = "";
		std::string m_name = "";
		std::map<int, EditorFolder> m_subFolders;
		std::map<int, EditorFile> m_files;
		int m_id = 0;
		bool m_markedForErase = false;
		bool m_markedForForceOpen = false;
		EditorFolder* m_parent = nullptr;
	};


	class EditorUtility
	{

	public:

		EditorUtility();
		~EditorUtility();

		static bool CreateFolderInPath(const std::string& path);
		static bool DeleteDirectory(const std::string& path);
		static std::string RemoveExtensionFromFilename(const std::string& filename);
		static std::string OpenFile(const char* filter, void* window);
		static std::string SaveFile(const char* filter, void* window);
		static bool ChangeFilename(const char* folderPath, const char* oldName, const char* newName);

	private:

	};
}


#endif
