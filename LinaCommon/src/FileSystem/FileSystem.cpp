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
#include <chrono>
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
					outData.push_back(FixPath(path));
				}
				else
				{
					String fullpath = entry.path().string().c_str();
					if (GetFileExtension(fullpath).compare(extensionFilter))
					{
						outData.push_back(FixPath(fullpath));
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
			outData.push_back(FixPath(entry.path().string().c_str()));
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

	String FileSystem::GetLastModifiedDate(const String& path)
	{
		std::filesystem::file_time_type ftime  = std::filesystem::last_write_time(path);
		auto							sctp   = std::chrono::time_point_cast<std::chrono::system_clock::duration>(ftime - decltype(ftime)::clock::now() + std::chrono::system_clock::now());
		std::time_t						cftime = std::chrono::system_clock::to_time_t(sctp);
		return std::asctime(std::localtime(&cftime));
	}

	String FileSystem::GetFilePath(const String& fileName)
	{
		const String usedFilename = FixPath(fileName);
		const char*	 cstr		  = usedFilename.c_str();
		unsigned int strLength	  = (unsigned int)usedFilename.length();
		unsigned int end		  = strLength - 1;

		while (end != 0)
		{
			if (cstr[end] == '/')
				break;

			end--;
		}

		if (end == 0)
			return usedFilename;

		else
		{
			unsigned int start = 0;
			end				   = end + 1;
			return usedFilename.substr(start, end - start);
		}
	}

	String FileSystem::RemoveExtensionFromPath(const String& fileName)
	{
		const size_t lastindex = fileName.find_last_of(".");
		return FixPath(fileName.substr(0, lastindex));
	}

	String FileSystem::GetFilenameAndExtensionFromPath(const String& fileName)
	{
		return FixPath(fileName.substr(fileName.find_last_of("/\\") + 1));
	}

	String FileSystem::GetFileExtension(const String& file)
	{
		return FixPath(file.substr(file.find_last_of(".") + 1));
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
		try
		{
			auto path = std::filesystem::current_path();
			return path.string();
		}
		catch (std::exception e)
		{
			int a = 5;
			return "";
		}
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

	namespace
	{
		void LinaCopyFile(const std::filesystem::path& source, const std::filesystem::path& destination)
		{
			try
			{
				std::filesystem::copy_file(source, destination, std::filesystem::copy_options::overwrite_existing);
			}
			catch (std::filesystem::filesystem_error& e)
			{
				LINA_ERR("Error duplicating file! {0}", e.what());
			}
		}

		void LinaCopyDirectory(const std::filesystem::path& source, const std::filesystem::path& destination)
		{
			try
			{
				std::filesystem::create_directory(destination);
				for (const auto& entry : std::filesystem::recursive_directory_iterator(source))
				{
					const auto& path			= entry.path();
					auto		relativePathStr = path.lexically_relative(source).string();
					std::filesystem::copy(path, destination / relativePathStr, std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
				}
			}
			catch (std::filesystem::filesystem_error& e)
			{
				LINA_ERR("Error duplicating directory! {0}", e.what());
			}
		}
	} // namespace

	String FileSystem::Duplicate(const String& path)
	{
		try
		{
			if (std::filesystem::exists(path))
			{
				const bool	 isDirectory   = FileSystem::IsDirectory(path);
				const String correctedPath = isDirectory ? path : FileSystem::RemoveExtensionFromPath(path);
				String		 finalPath	   = correctedPath + " (Copy)";

				size_t insertBeforeExtension = finalPath.length();
				if (!isDirectory)
				{
					finalPath += "." + FileSystem::GetFileExtension(path);
				}

				while (FileSystem::FileOrPathExists(finalPath))
				{
					finalPath.insert(insertBeforeExtension, " (Copy)");
					insertBeforeExtension += 7;
				}

				std::filesystem::path destination = finalPath;

				if (std::filesystem::is_regular_file(path))
				{
					LinaCopyFile(path, destination);
					return finalPath;
				}
				else if (std::filesystem::is_directory(path))
				{
					LinaCopyDirectory(path, destination);
					return finalPath;
				}
				else
				{
					LINA_ERR("Unsupported file type! {0}", path);
				}
			}
			else
			{
				LINA_ERR("Path doesn't exist! {0}", path);
			}
		}
		catch (std::filesystem::filesystem_error&)
		{
			LINA_ERR("Exception processing path! {0}", path);
		}

		return "";
	}

	String FileSystem::GetRelative(const String& src, const String& target)
	{
		std::filesystem::path sourcePath(src);
		std::filesystem::path targetPath(target);
		std::filesystem::path relativePath = std::filesystem::relative(targetPath, sourcePath);
		return relativePath.string();
	}

	void FileSystem::PerformMove(const String& targetFile, const String& targetDirectory)
	{
		try
		{
			// Create the directory if it doesn't exist
			if (!std::filesystem::exists(targetDirectory))
			{
				LINA_ERR("Target directory does not exist! {0}", targetDirectory);
				return;
			}

			// Construct the new path for the file
			std::filesystem::path file		  = std::filesystem::path(targetFile);
			std::filesystem::path destination = std::filesystem::path(targetDirectory) / file.filename();

			// Move (or rename) the file to the new directory
			std::filesystem::rename(targetFile, destination);
		}
		catch (const std::filesystem::filesystem_error& e)
		{
			LINA_ERR("::PerformMove error {0}", e.what());
		}
	}

	String FileSystem::GetSystemTimeStr()
	{
		std::time_t		   now		 = std::time(nullptr);
		std::tm*		   localTime = std::localtime(&now);
		std::ostringstream oss;
		oss << std::setw(2) << std::setfill('0') << localTime->tm_hour << ":" << std::setw(2) << std::setfill('0') << localTime->tm_min << ":" << std::setw(2) << std::setfill('0') << localTime->tm_sec;
		return oss.str();
	}

	void FileSystem::GetSystemTimeInts(int32& hours, int32& minutes, int32& seconds)
	{
		std::time_t now		  = std::time(nullptr);
		std::tm*	localTime = std::localtime(&now);
		hours				  = localTime->tm_hour;
		minutes				  = localTime->tm_min;
		seconds				  = localTime->tm_sec;
	}

	String FileSystem::GetTimeStrFromMicroseconds(int64 microseconds)
	{
		int64_t totalSeconds = microseconds / 1000000;

		int hours	= (totalSeconds / 3600) % 24; // Wrap-around using modulo 24
		int minutes = (totalSeconds / 60) % 60;
		int seconds = totalSeconds % 60;

		std::ostringstream oss;
		oss << std::setw(2) << std::setfill('0') << hours << ":" << std::setw(2) << std::setfill('0') << minutes << ":" << std::setw(2) << std::setfill('0') << seconds;

		return oss.str();
	}

	void FileSystem::CopyDirectory(const String& directory, const String& targetParentFolder)
	{
		namespace fs = std::filesystem;

		try
		{
			// Ensure the source directory exists
			fs::path source(directory);
			if (!fs::exists(source))
			{
				throw std::runtime_error("Source directory does not exist or is not a directory.");
			}

			// Create the destination path
			fs::path destination = fs::path(targetParentFolder) / source.filename();

			// Check if the destination already exists
			if (fs::exists(destination))
				fs::remove_all(destination);

			// Recursively copy the directory and its contents
			fs::create_directories(destination); // Create the target folder
			for (const auto& entry : fs::recursive_directory_iterator(source))
			{
				const auto& sourcePath		= entry.path();
				auto		relativePath	= fs::relative(sourcePath, source);
				auto		destinationPath = destination / relativePath;

				if (fs::is_directory(sourcePath))
					fs::create_directory(destinationPath);
				else if (fs::is_regular_file(sourcePath))
					fs::copy_file(sourcePath, destinationPath, fs::copy_options::overwrite_existing);
			}
		}
		catch (const std::exception& ex)
		{
			LINA_ERR("Error while copying directory {0}", ex.what());
		}
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
