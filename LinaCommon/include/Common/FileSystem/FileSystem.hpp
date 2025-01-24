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

#pragma once

#include "Common/Data/Vector.hpp"

namespace Lina
{
	class FileSystem
	{
	public:
		// Path
		static bool	  DeleteFileInPath(const String& path);
		static bool	  CreateFolderInPath(const String& path);
		static bool	  DeleteDirectory(const String& path);
		static void	  GetFilesInDirectory(const String& path, Vector<String>& outData, String extensionFilter = "");
		static void	  GetFilesAndFoldersInDirectory(const String& path, Vector<String>& outData);
		static bool	  IsDirectory(const String& path);
		static bool	  ChangeDirectoryName(const String& oldPath, const String& newPath);
		static bool	  FileOrPathExists(const String& path);
		static String GetLastModifiedDate(const String& path);
		static String GetFilePath(const String& fileName);
		static String RemoveExtensionFromPath(const String& filename);
		static String GetFilenameAndExtensionFromPath(const String& filename);
		static String GetFileExtension(const String& file);
		static String GetFilenameOnlyFromPath(const String& file);
		static String GetLastFolderFromPath(const String& path);
		static String ReadFileContentsAsString(const String& filePath);
		static void	  ReadFileContentsToVector(const String& filePath, Vector<char>& vec);
		static String GetRunningDirectory();
		static String GetUserDataFolder();
		static String FixPath(const String& str);
		static String Duplicate(const String& path);
		static String GetRelative(const String& src, const String& target);
		static void	  PerformMove(const String& targetFile, const String& targetDirectory);
		static String GetSystemTimeStr();
		static void	  GetSystemTimeInts(int32& hours, int32& minutes, int32& seconds);
		static String GetTimeStrFromMicroseconds(int64 microseconds);
		static void	  CopyDirectory(const String& directory, const String& targetParentFolder);

		// Utility

		/// <summary>
		/// ! You are responsible for deleting returned buffer!
		/// </summary>
		/// <param name="input"></param>
		/// <returns></returns>
		static char* WCharToChar(const wchar_t* input);

		/// <summary>
		/// ! You are responsible for deleting returned buffer!
		/// </summary>
		/// <param name="input"></param>
		/// <returns></returns>
		static wchar_t* CharToWChar(const char* input);

		static Vector<String> Split(const String& s, char delim);
	};

} // namespace Lina
