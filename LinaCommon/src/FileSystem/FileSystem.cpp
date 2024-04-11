/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

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

#include "Common/FileSystem/FileSystem.hpp"
#include "Common/Data/CommonData.hpp"
#include "Common/Data/Vector.hpp"
#include "Common/Platform/PlatformInclude.hpp"

#ifdef LINA_PLATFORM_APPLE
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif

#ifdef LINA_PLATFORM_WINDOWS
#include <shlobj.h>
#endif

namespace Lina
{
	bool FileSystem::DeleteFileInPath(const String& path)
	{
		return remove(path.c_str());
	}

	bool FileSystem::CreateFolderInPath(const String& path)
	{
		const Vector<String> directories = UtilStr::SplitBy(path, "/");
		String				 currentPath = "";
		for (const auto& dir : directories)
		{
			currentPath += dir + "/";

			if (!FileOrPathExists(currentPath))
			{
				bool success = std::filesystem::create_directory(currentPath.c_str());
				if (!success)
				{
					LINA_ERR("Could not create directory: {0}", currentPath);
					return false;
				}
			}
		}
		return true;
	}

	bool FileSystem::DeleteDirectory(const String& path)
	{
		try
		{
			bool success = std::filesystem::remove_all(path.c_str());
		}
		catch (const std::exception& err)
		{
			LINA_ERR("Could not delete directory: {0}, {1}", path, err.what());
			return false;
		}

		return true;
	}

	void FileSystem::GetFilesInDirectory(const String& path, Vector<String>& outData, String extensionFilter)
	{
		outData.clear();
		for (const auto& entry : std::filesystem::directory_iterator(path.c_str()))
		{
			if (!entry.is_directory())
			{
				if (extensionFilter.empty())
				{
					String path = entry.path().string().c_str();
					linatl::replace(path.begin(), path.end(), '\\', '/');
					outData.push_back(path);
				}
				else
				{
					String fullpath = entry.path().string().c_str();
					if (GetFileExtension(fullpath).compare(extensionFilter))
					{
						linatl::replace(fullpath.begin(), fullpath.end(), '\\', '/');
						outData.push_back(fullpath);
					}
				}
			}
		}
	}

	void FileSystem::GetFilesAndFoldersInDirectory(const String& path, Vector<String>& outData)
	{
		outData.clear();
		for (const auto& entry : std::filesystem::directory_iterator(path.c_str()))
		{
			outData.push_back(entry.path().string().c_str());
		}
	}

	bool FileSystem::IsDirectory(const String& path)
	{
		return std::filesystem::is_directory(path);
	}
	bool FileSystem::ChangeDirectoryName(const String& oldPath, const String& newPath)
	{

		/*	Deletes the file if exists */
		if (std::rename(oldPath.c_str(), newPath.c_str()) != 0)
		{
			LINA_ERR("Failed to rename directory! Old Name: {0}, New Name: {1}", oldPath, newPath);
			return false;
		}

		return true;
	}

	bool FileSystem::FileOrPathExists(const String& path)
	{
		return std::filesystem::exists(path.c_str());
	}

	String FileSystem::GetFilePath(const String& fileName)
	{
		const char*	 cstr	   = fileName.c_str();
		unsigned int strLength = (unsigned int)fileName.length();
		unsigned int end	   = strLength - 1;

		while (end != 0)
		{
			if (cstr[end] == '/')
				break;

			end--;
		}

		if (end == 0)
			return fileName;

		else
		{
			unsigned int start = 0;
			end				   = end + 1;
			return fileName.substr(start, end - start);
		}
	}

	String FileSystem::RemoveExtensionFromPath(const String& fileName)
	{
		size_t lastindex = fileName.find_last_of(".");
		return fileName.substr(0, lastindex);
	}

	String FileSystem::GetFilenameAndExtensionFromPath(const String& fileName)
	{
		return fileName.substr(fileName.find_last_of("/\\") + 1);
	}

	String FileSystem::GetFileExtension(const String& file)
	{
		return file.substr(file.find_last_of(".") + 1);
	}

	String FileSystem::GetFilenameOnlyFromPath(const String& file)
	{
		return RemoveExtensionFromPath(GetFilenameAndExtensionFromPath(file));
	}

	String FileSystem::GetLastFolderFromPath(const String& path)
	{

		String		 fixedPath = FixPath(path);
		const size_t lastSlash = fixedPath.find_last_of("/\\");

		if (lastSlash == fixedPath.size() || lastSlash == fixedPath.size() - 1)
			fixedPath = fixedPath.substr(0, lastSlash);

		const size_t actualLast = fixedPath.find_last_of("/\\");
		if (actualLast != String::npos)
			fixedPath = fixedPath.substr(actualLast + 1, fixedPath.size());
		return fixedPath;
	}

	String FileSystem::ReadFileContentsAsString(const String& filePath)
	{
		std::ifstream ifs(filePath.c_str());
		auto		  a = std::istreambuf_iterator<char>(ifs);
		auto		  b = (std::istreambuf_iterator<char>());
		return std::string(a, b);
	}

	void FileSystem::ReadFileContentsToVector(const String& filePath, Vector<char>& vec)
	{
		std::ifstream file(filePath.c_str(), std::ios::binary);
		if (!file)
		{
			LINA_ERR("[Font] -> Could not open file! {0}", filePath.c_str());
			return;
		}

		// Size
		file.seekg(0, std::ios::end);
		std::streampos length = file.tellg();
		file.seekg(0, std::ios::beg);

		// Into vec
		vec = Vector<char>(length);
		file.read(&vec[0], length);
	}

	String FileSystem::GetRunningDirectory()
	{
		return std::filesystem::current_path().string();
	}

	String FileSystem::RemoveWhitespaces(const String& str)
	{
		std::string result;
		for (char c : str)
		{
			if (!std::isspace(static_cast<unsigned char>(c)))
			{
				result.push_back(c);
			}
		}
		return result;
	}

	String FileSystem::GetUserDataFolder()
	{
#ifdef LINA_PLATFORM_WINDOWS
		char path[MAX_PATH];
		if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, path)))
		{
			String pp = String(path);
			return pp + "/LinaEngine/";
		}
		return std::string();
#elif defined LINA_PLATFORM_APPLE
		struct passwd* pw	= getpwuid(getuid());
		String		   path = String(pw->pw_dir);
		return path + "/Library/Application Support/LinaEngine/";
#endif
		return "";
	}

	String FileSystem::FixPath(const String& str)
	{
		String result = UtilStr::ReplaceAll(str, "\\", "/");
		result		  = UtilStr::ReplaceAll(result, "\\\\", "/");
		// result = UtilStr::ReplaceAll(result, "\\", "/");
		return result;
	}
	bool FileSystem::FolderContainsDirectory(Folder* root, const String& path, DirectoryItem*& outItem)
	{
		bool contains = false;

		for (auto* folder : root->folders)
		{
			if (folder->fullPath.compare(path) == 0)
			{
				contains = true;
				outItem	 = folder;
				break;
			}
		}

		if (!contains)
		{
			for (auto* file : root->files)
			{
				if (file->fullPath.compare(path) == 0)
				{
					contains = true;
					outItem	 = file;
					break;
				}
			}
		}

		return contains;
	}

	void FileSystem::GetFolderHierarchToRoot(Folder* folder, Vector<Folder*>& hierarchy)
	{
		if (folder->parent != nullptr)
		{
			hierarchy.push_back(folder->parent);
			GetFolderHierarchToRoot(folder->parent, hierarchy);
		}
	}

	void FileSystem::CreateNewSubfolder(Folder* parent)
	{
		const String newFolderName = GetUniqueDirectoryName(parent, "NewFolder", "");

		Folder* folder	 = new Folder();
		folder->parent	 = parent;
		folder->name	 = newFolderName;
		folder->fullPath = parent->fullPath + "/" + folder->name;
		parent->folders.push_back(folder);

		CreateFolderInPath(folder->fullPath);
	}

	void FileSystem::DeleteFolder(Folder* folder)
	{
		for (auto* subfolder : folder->folders)
			DeleteFolder(subfolder);

		for (Vector<Folder*>::iterator it = folder->parent->folders.begin(); it < folder->parent->folders.end(); it++)
		{
			if (*it == folder)
			{
				folder->parent->folders.erase(it);
				break;
			}
		}

		DeleteDirectory(folder->fullPath);
		delete folder;
		folder = nullptr;
	}

	void FileSystem::ParentPathUpdated(Folder* folder)
	{
		const String oldPath = folder->fullPath;
		folder->fullPath	 = folder->parent->fullPath + "/" + folder->name;

		for (auto* subfolder : folder->folders)
			ParentPathUpdated(subfolder);

		for (auto* file : folder->files)
			ParentPathUpdated(file);
	}

	void FileSystem::ParentPathUpdated(File* file)
	{
		const StringID sidBefore = TO_SID(file->fullPath);
		const String   oldPath	 = file->fullPath;
		file->fullPath			 = file->parent->fullPath + "/" + file->fullName;
		const StringID sidNow	 = TO_SID(file->fullPath);
		file->sid				 = sidNow;
		// Resource path updated event.
	}

	void FileSystem::ChangeFileName(File* file, const String& newName)
	{
		const String oldPath = file->fullPath;
		file->name			 = newName;
		file->fullName		 = newName + "." + file->extension;
		ParentPathUpdated(file);
		ChangeDirectoryName(oldPath, file->fullPath);
	}

	void FileSystem::ChangeFolderName(Folder* folder, const String& newName)
	{
		const String oldPath = folder->fullPath;
		folder->fullPath	 = folder->parent->fullPath + "/" + newName;
		folder->name		 = newName;

		for (auto* subfolder : folder->folders)
			ParentPathUpdated(subfolder);

		for (auto* file : folder->files)
			ParentPathUpdated(file);

		ChangeDirectoryName(oldPath, folder->fullPath);
	}

	void FileSystem::RewriteFileContents(File* file, const String& newContent)
	{
		std::ofstream newFile;
		newFile.open(file->fullPath.c_str(), std::ofstream::out | std::ofstream::trunc);
		newFile << newContent.c_str() << std::endl;
		newFile.close();
	}

	bool FileSystem::FolderContainsFilter(const Folder* folder, const String& filter)
	{
		const String filterLowercase = ToLower(filter);
		const String folderLower	 = ToLower(folder->name);
		const bool	 folderContains	 = folderLower.find(filterLowercase) != String::npos;
		return folderContains;
	}

	bool FileSystem::SubfoldersContainFilter(const Folder* folder, const String& filter)
	{
		bool subFoldersContain = false;

		for (auto* folder : folder->folders)
		{
			if (FolderContainsFilter(folder, filter))
			{
				subFoldersContain = true;
				break;
			}

			if (SubfoldersContainFilter(folder, filter))
			{
				subFoldersContain = true;
				break;
			}
		}

		return subFoldersContain;
	}

	bool FileSystem::FileContainsFilter(const File& file, const String& filter)
	{
		const String filterLowercase = ToLower(filter);
		const String fileLower		 = ToLower(file.fullName);
		return fileLower.find(filterLowercase) != String::npos;
	}

	File* FileSystem::FindFile(Folder* root, const String& path)
	{
		for (auto* subfolder : root->folders)
		{
			File* f = FindFile(subfolder, path);

			if (f != nullptr)
				return f;
		}

		for (auto* file : root->files)
		{
			if (file->fullPath.compare(path) == 0)
				return file;
		}

		return nullptr;
	}

	String FileSystem::GetUniqueDirectoryName(Folder* parent, const String& prefix, const String& extension)
	{
		static int uniqueNameCounter = 0;
		String	   newName			 = prefix + "_" + TO_STRING(uniqueNameCounter).c_str();
		bool	   newNameExists	 = true;
		int		   timeOutCounter	 = 0;

		// Loop through the sub-folders and make sure no folder with the same name exists.
		while (newNameExists && timeOutCounter < 1000)
		{
			bool found = false;

			// Check sub-folders
			for (auto* folder : parent->folders)
			{
				if (folder->name.compare(newName) == 0)
				{
					found = true;
					break;
				}
			}

			// Check files.
			if (!found)
			{
				for (auto* file : parent->files)
				{
					if (file->fullName.compare(newName + extension) == 0)
					{
						found = true;
						break;
					}
				}
			}

			if (!found)
				newNameExists = false;
			else
			{
				uniqueNameCounter++;
				newName = prefix + "_" + TO_STRING(uniqueNameCounter).c_str();
			}

			timeOutCounter++;
		}

		LINA_ASSERT(!newNameExists, "Could not get a unique name for the file/folder!");

		return newName;
	}

	char* FileSystem::WCharToChar(const wchar_t* input)
	{
		// Count required buffer size (plus one for null-terminator).
		size_t size	  = (wcslen(input) + 1) * sizeof(wchar_t);
		char*  buffer = new char[size];

#ifdef __STDC_LIB_EXT1__
		// wcstombs_s is only guaranteed to be available if __STDC_LIB_EXT1__ is defined
		size_t convertedSize;
		std::wcstombs_s(&convertedSize, buffer, size, input, size);
#else
#pragma warning(disable : 4996)
		std::wcstombs(buffer, input, size);
#endif
		return buffer;
	}

	String FileSystem::ToLower(const String& input)
	{
		String data = input;

		std::for_each(data.begin(), data.end(), [](char& c) { c = ::tolower(c); });

		return data;
	}

	wchar_t* FileSystem::CharToWChar(const char* input)
	{
#ifdef LINA_PLATFORM_WINDOWS

		int		 bufferLength = MultiByteToWideChar(CP_UTF8, 0, input, -1, NULL, 0);
		wchar_t* wideBuf	  = new wchar_t[bufferLength];
		MultiByteToWideChar(CP_UTF8, 0, input, -1, wideBuf, bufferLength);
		return wideBuf;
#else
		size_t	 inputLength = strlen(input) + 1;
		wchar_t* wideBuf	 = new wchar_t[inputLength];
		mbstowcs(wideBuf, input, inputLength);
		return wideBuf;
#endif
		return nullptr;
	}

	String FileSystem::ToUpper(const String& input)
	{
		String data = input;

		std::for_each(data.begin(), data.end(), [](char& c) { c = ::toupper(c); });

		return data;
	}

	Vector<String> FileSystem::Split(const String& s, char delim)
	{
		Vector<String> elems;

		const char*	 cstr	   = s.c_str();
		unsigned int strLength = (unsigned int)s.length();
		unsigned int start	   = 0;
		unsigned int end	   = 0;

		while (end <= strLength)
		{
			while (end <= strLength)
			{
				if (cstr[end] == delim)
					break;

				end++;
			}

			elems.push_back(s.substr(start, end - start));
			start = end + 1;
			end	  = start;
		}

		return elems;
	}

} // namespace Lina
