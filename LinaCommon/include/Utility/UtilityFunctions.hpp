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

/*
Class: UtilityFunctions

Various utility functions for general purpose use.

Timestamp: 1/5/2019 12:42:58 AM
*/

#ifndef UtilityFunctions_HPP
#define UtilityFunctions_HPP

#include "Core/CommonUtility.hpp"

#include "Data/Map.hpp"

namespace Lina
{
    namespace Utility
    {
        static int s_uniqueID = 100;

        // Get char* from a WCHAR
        extern char* WCharToChar(const wchar_t* input);

        // Scans the given folder & fills it's child folder data
        extern void ScanFolder(Folder* root, bool recursive = true, int* totalFiles = nullptr, bool isRescan = false);

        // Returns true if a subfolder or a file with the given path exists within the root.
        extern bool FolderContainsDirectory(Folder* root, const String& path, DirectoryItem*& outItem);

        // Fills the given array with the parent folders up to the top most.
        extern void GetFolderHierarchToRoot(Folder* folder, Vector<Folder*>& hierarchy);

        // Removes file from OS.
        extern bool DeleteFileInPath(const String& path);

        // Iterates through the sub-folders and files of the parent and returns a name that is unique within the given folder.
        extern String GetUniqueDirectoryName(Folder* parent, const String& prefix, const String& extension);

        // Checks the sub-folders and finds a folder name unique within the given parent's hierarchy.
        extern void CreateNewSubfolder(Folder* parent);

        // Deletes the folder and it's whole hierarchy of sub-folders & files.
        extern void DeleteFolder(Folder* folder);

        // Deletes the file and removes any resource tied with it.
        extern void DeleteResourceFile(File* file);

        // Creates a new folder in path.
        extern bool CreateFolderInPath(const String& path);

        // Removes all files & folders in directory.
        extern bool DeleteDirectory(const String& path);

        // Updates a folder's path according to a change on it's parent.
        extern void ParentPathUpdated(Folder* folder);

        // Updates the file's path according to a change on it's parent.
        extern void ParentPathUpdated(File* file);
        
        // Replaces the folder name, updates it's path and makes sure the children's path are updated as well.
        extern void ChangeFolderName(Folder* folder, const String& newName);

        // Replaces the file name, updates the path & notifies event listeners.
        extern void ChangeFileName(File* file, const String& newName);

        // Replaces given filename.
        extern bool ChangeDirectoryName(const String& oldPath, const String& newPath);

        // Erases the old content & rewrites it with the given content.
        extern void RewriteFileContents(File* file, const String& newContent);

        // Creates a GUID.
        extern int GetUniqueID();

        // Return CPU's clock time.
        extern double GetCPUTime();

        // Check if given file exists.
        extern bool FileExists(const String& path);

        // Splits string at deliminitor.
        extern Vector<String> Split(const String& s, char delim);

        // Returns the path until the specified file, no file name nor extension.
        extern String GetFilePath(const String& fileName);

        // Removes the extension and returns the string as is.
        extern String GetFileWithoutExtension(const String& filename);

        // Returns the file name with it's extension, no path.
        extern String GetFileNameOnly(const String& filename);

        // Returns only the file extension, no path or no dots.
        extern String GetFileExtension(const String& file);

        // Checks if the given folder contain the given filter within the folder name.
        extern bool FolderContainsFilter(const Folder* folder, const String& filter);

        // Checks if the folder's sub-folders contain the given filter as folder name.
        extern bool SubfoldersContainFilter(const Folder* folder, const String& filter);

        // Checks if the given file contains the given filter within the file name, including extension.
        extern bool FileContainsFilter(const File& file, const String& filter);

        // Finds the file with the given path if exists & returns, else returns nullptr
        extern File* FindFile(Folder* root, const String& path);

        // Returns the whole file content as string
        extern String GetFileContents(const String& filePath);

        // Return the full directory (user-specific) that the engine executable is running on.
        extern String GetRunningDirectory();

        // Converts all string characters to lowercase
        extern String ToLower(const String& input);

        // Converts all string characters to uppercase
        extern String ToUpper(const String& input);

        extern String CharArrayToString(char* arr, int size);

    } // namespace Utility
} // namespace Lina

#endif
