/* 
This file is a part of: Lina AudioEngine
https://github.com/inanevin/Lina

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
Class: FileUtility

Functions for searching directories, moving & deleting files.

Timestamp: 12/19/2020 3:20:01 PM
*/

#pragma once

#ifndef FileUtility_HPP
#define FileUtility_HPP

// Headers here.
#include <string>
#include <vector>

namespace Lina::FileUtility
{

	struct File
	{
		std::string m_fullPath = "";	// folder + purename + extension
		std::string m_folderPath = "";
		std::string m_pureName = "";	// name without extension
		std::string m_extension = "";
		std::string m_fullName = "";	// name with extension
	};

	struct Folder
	{
		std::string m_fullPath = "";
		std::string m_name = "";
		std::vector<File> m_files;
		std::vector<Folder> m_folders;
	};

	extern std::string GetFileExtension(const std::string& path);
	extern void ScanFolder(Folder& root, bool recursive = true);
	extern bool FileExists(const std::string& path);
	extern bool DeleteFileInPath(const std::string& path);
	extern bool CreateFolderInPath(const std::string& path);
	extern bool DeleteDirectory(const std::string& path);
	extern bool ChangeFileName(const std::string& folderPath, const std::string& oldName, const std::string& newName);
	extern std::string RemoveExtensionFromFileName(const std::string& filename);
	extern std::string OpenFile(const std::string& filter, void* window);
	extern std::string SaveFile(const std::string& filter, void* window);
}

#endif
