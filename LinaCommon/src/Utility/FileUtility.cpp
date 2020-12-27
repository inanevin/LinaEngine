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

#include "Utility/FileUtility.hpp"
#include "Core/Log.hpp"
#include "Core/MacroDetection.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>

#ifdef LINA_WINDOWS
#include <Shlwapi.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_INCLUDE_NONE
#define GLFW_DLL
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"

#include <commdlg.h>
#endif

namespace Lina::FileUtility
{
	std::string GetFileExtension(const std::string& path)
	{
		std::string ext = path.substr(path.find(".") + 1);
		return ext;
	}

	void ScanFolder(Folder& root, bool recursive)
	{
		for (const auto& entry : std::filesystem::directory_iterator(root.m_fullPath))
		{
			if (entry.path().has_extension())
			{
				root.m_files.push_back(File());
				File& file = root.m_files.back();

				file.m_fullName = entry.path().filename().string();
				file.m_folderPath = entry.path().parent_path().string() + "/";
				std::string replacedPath = entry.path().relative_path().string();
				std::replace(replacedPath.begin(), replacedPath.end(), '\\', '/');
				file.m_fullPath = replacedPath;
				file.m_extension = file.m_fullName.substr(file.m_fullName.find(".") + 1);
				file.m_pureName = RemoveExtensionFromFileName(file.m_fullName);
			}
			else
			{
				root.m_folders.push_back(Folder());
				Folder& folder = root.m_folders.back();
				folder.m_name = entry.path().filename().string();
				std::string replacedPath = entry.path().relative_path().string();
				std::replace(replacedPath.begin(), replacedPath.end(), '\\', '/');
				folder.m_fullPath = replacedPath;

				if (recursive)
					ScanFolder(folder, recursive);
			}
		}

	}


	bool FileExists(const std::string& path)
	{
		struct stat buffer;
		return (stat(path.c_str(), &buffer) == 0);
	}

	bool DeleteFileInPath(const std::string& path)
	{
		return remove(path.c_str());
	}
	bool CreateFolderInPath(const std::string& path)
	{
		bool success = std::filesystem::create_directory(path);;

		if (!success)
			LINA_ERR("Could not create directory. {0}", path);

		return success;
	}

	bool DeleteDirectory(const std::string& path)
	{
		bool success = std::filesystem::remove_all(path);

		if (!success)
			LINA_ERR("Could not delete directory. {0}", path);

		return success;
	}

	bool ChangeFileName(const std::string& folderPath, const std::string& oldName, const std::string& newName)
	{
		std::string oldPathStr = std::string(folderPath) + std::string(oldName);
		std::string newPathStr = std::string(folderPath) + std::string(newName);

		/*	Deletes the file if exists */
		if (std::rename(oldPathStr.c_str(), newPathStr.c_str()) != 0)
		{
			LINA_ERR("Can not rename file! Folder Path: {0}, Old Name: {1}, New Name: {2}", folderPath, oldName, newName);
			return false;
		}

		return true;
	}

	std::string RemoveExtensionFromFileName(const std::string& filename)
	{
		size_t lastindex = filename.find_last_of(".");
		return filename.substr(0, lastindex);
	}

	std::string OpenFile(const std::string& filter, void* window)
	{
#ifdef LINA_WINDOWS
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)window);
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter.c_str();
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetOpenFileNameA(&ofn) == TRUE)
		{
			std::string replacedPath = ofn.lpstrFile;
			std::replace(replacedPath.begin(), replacedPath.end(), '\\', '/');
			return replacedPath;
		}

#endif
		return std::string();
	}

	std::string SaveFile(const std::string& filter, void* window)
	{
#ifdef LINA_WINDOWS
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)window);
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter.c_str();
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		if (GetSaveFileNameA(&ofn) == TRUE)
		{
			std::string replacedPath = ofn.lpstrFile;
			std::replace(replacedPath.begin(), replacedPath.end(), '\\', '/');
			return replacedPath;
		}
#endif
		return std::string();
	}


}