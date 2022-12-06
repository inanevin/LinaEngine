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

#include "Utility/UtilityFunctions.hpp"
#include "Core/PlatformMacros.hpp"
#include "Log/Log.hpp"
#include "EventSystem/ResourceEvents.hpp"
#include "EventSystem/EventSystem.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <Data/String.hpp>

#ifdef LINA_PLATFORM_WINDOWS
#include <windows.h>
#endif

namespace Lina
{
    namespace Utility
    {

        char* WCharToChar(const wchar_t* input)
        {
            // Count required buffer size (plus one for null-terminator).
            size_t size   = (wcslen(input) + 1) * sizeof(wchar_t);
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

        bool FileExists(const String& path)
        {
            struct stat buffer;
            return (stat(path.c_str(), &buffer) == 0);
        }

        bool FolderContainsDirectory(Folder* root, const String& path, DirectoryItem*& outItem)
        {
            bool contains = false;

            for (auto* folder : root->folders)
            {
                if (folder->fullPath.compare(path) == 0)
                {
                    contains = true;
                    outItem  = folder;
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
                        outItem  = file;
                        break;
                    }
                }
            }

            return contains;
        }

        void GetFolderHierarchToRoot(Folder* folder, Vector<Folder*>& hierarchy)
        {
            if (folder->parent != nullptr)
            {
                hierarchy.push_back(folder->parent);
                GetFolderHierarchToRoot(folder->parent, hierarchy);
            }
        }

        bool DeleteFileInPath(const String& path)
        {
            return remove(path.c_str());
        }

        String GetUniqueDirectoryName(Folder* parent, const String& prefix, const String& extension)
        {
            static int uniqueNameCounter = 0;
            String     newName           = prefix + "_" + TO_STRING(uniqueNameCounter).c_str();
            bool       newNameExists     = true;
            int        timeOutCounter    = 0;

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

        void CreateNewSubfolder(Folder* parent)
        {
            const String newFolderName = GetUniqueDirectoryName(parent, "NewFolder", "");

            Folder* folder   = new Folder();
            folder->parent   = parent;
            folder->name     = newFolderName;
            folder->fullPath = parent->fullPath + "/" + folder->name;
            parent->folders.push_back(folder);

            CreateFolderInPath(folder->fullPath);
        }

        void DeleteFolder(Folder* folder)
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

        bool CreateFolderInPath(const String& path)
        {
            bool success = std::filesystem::create_directory(path.c_str());

            if (!success)
                LINA_ERR("Could not create directory. {0}", path);

            return success;
        }

        bool DeleteDirectory(const String& path)
        {
            try
            {
                bool success = std::filesystem::remove_all(path.c_str());
            }
            catch (const std::exception& err)
            {
                LINA_ERR("Could not delete directory. {0} {1}", path, err.what());
                return false;
            }

            return true;
        }

        Vector<String> GetFolderContents(const String& path, String extensionFilter)
        {
            Vector<String> paths;
            for (const auto& entry : std::filesystem::directory_iterator(path.c_str()))
            {
                if (!entry.is_directory())
                {
                    if (extensionFilter.empty())
                        paths.push_back(entry.path().string().c_str());
                    else
                    {
                        String fullpath = entry.path().string().c_str();
                        if (GetFileExtension(fullpath).compare(extensionFilter))
                            paths.push_back(fullpath);
                    }
                }
            }
            return paths;
        }

        Vector<String> GetFolderContentsAndDirs(const String& path)
        {
            Vector<String> paths;
            for (const auto& entry : std::filesystem::directory_iterator(path.c_str()))
            {
                paths.push_back(entry.path().string().c_str());
            }
            return paths;
        }

        void ParentPathUpdated(Folder* folder)
        {
            const String oldPath = folder->fullPath;
            folder->fullPath     = folder->parent->fullPath + "/" + folder->name;

            for (auto* subfolder : folder->folders)
                ParentPathUpdated(subfolder);

            for (auto* file : folder->files)
                ParentPathUpdated(file);
        }

        void ParentPathUpdated(File* file)
        {
            const StringID sidBefore = TO_SID(file->fullPath);
            const String   oldPath   = file->fullPath;
            file->fullPath           = file->parent->fullPath + "/" + file->fullName;
            const StringID sidNow    = TO_SID(file->fullPath);
            file->sid                = sidNow;
            // Resource path updated event.
        }

        void ChangeFileName(File* file, const String& newName)
        {
            const String oldPath = file->fullPath;
            file->name           = newName;
            file->fullName       = newName + "." + file->extension;
            ParentPathUpdated(file);
            ChangeDirectoryName(oldPath, file->fullPath);
        }

        void ChangeFolderName(Folder* folder, const String& newName)
        {
            const String oldPath = folder->fullPath;
            folder->fullPath     = folder->parent->fullPath + "/" + newName;
            folder->name         = newName;

            for (auto* subfolder : folder->folders)
                ParentPathUpdated(subfolder);

            for (auto* file : folder->files)
                ParentPathUpdated(file);

            ChangeDirectoryName(oldPath, folder->fullPath);
        }

        bool ChangeDirectoryName(const String& oldPath, const String& newPath)
        {

            /*	Deletes the file if exists */
            if (std::rename(oldPath.c_str(), newPath.c_str()) != 0)
            {
                LINA_ERR("Failed to rename directory! Old Name: {1}, New Name: {2}", oldPath, newPath);
                return false;
            }

            return true;
        }

        void RewriteFileContents(File* file, const String& newContent)
        {
            std::ofstream newFile;
            newFile.open(file->fullPath.c_str(), std::ofstream::out | std::ofstream::trunc);
            newFile << newContent.c_str() << std::endl;
            newFile.close();
        }

        int GetUniqueID()
        {
            return ++s_uniqueID;
        }

        Vector<String> Split(const String& s, char delim)
        {
            Vector<String> elems;

            const char*  cstr      = s.c_str();
            unsigned int strLength = (unsigned int)s.length();
            unsigned int start     = 0;
            unsigned int end       = 0;

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
                end   = start;
            }

            return elems;
        }

        String GetFilePath(const String& fileName)
        {
            const char*  cstr      = fileName.c_str();
            unsigned int strLength = (unsigned int)fileName.length();
            unsigned int end       = strLength - 1;

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
                end                = end + 1;
                return fileName.substr(start, end - start);
            }
        }

        String GetFileExtension(const String& file)
        {
            return file.substr(file.find_last_of(".") + 1);
        }

        String GetFileNameOnly(const String& fileName)
        {
            return fileName.substr(fileName.find_last_of("/\\") + 1);
        }

        String GetFileWithoutExtension(const String& fileName)
        {
            size_t lastindex = fileName.find_last_of(".");
            return fileName.substr(0, lastindex);
        }

        bool FileContainsFilter(const File& file, const String& filter)
        {
            const String filterLowercase = Utility::ToLower(filter);
            const String fileLower       = Utility::ToLower(file.fullName);
            return fileLower.find(filterLowercase) != String::npos;
        }

        bool FolderContainsFilter(const Folder* folder, const String& filter)
        {
            const String filterLowercase = Utility::ToLower(filter);
            const String folderLower     = Utility::ToLower(folder->name);
            const bool   folderContains  = folderLower.find(filterLowercase) != String::npos;
            return folderContains;
        }

        bool SubfoldersContainFilter(const Folder* folder, const String& filter)
        {
            bool subFoldersContain = false;

            for (auto* folder : folder->folders)
            {
                if (Utility::FolderContainsFilter(folder, filter))
                {
                    subFoldersContain = true;
                    break;
                }

                if (Utility::SubfoldersContainFilter(folder, filter))
                {
                    subFoldersContain = true;
                    break;
                }
            }

            return subFoldersContain;
        }

        File* FindFile(Folder* root, const String& path)
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

        String GetFileContents(const String& filePath)
        {
            std::ifstream ifs(filePath.c_str());
            auto          a = std::istreambuf_iterator<char>(ifs);
            auto          b = (std::istreambuf_iterator<char>());
            std::string   content(a, b);
            return content.c_str();
        }

        String GetRunningDirectory()
        {
#ifdef LINA_PLATFORM_WINDOWS
            TCHAR buffer[MAX_PATH] = {0};
            GetModuleFileName(NULL, buffer, MAX_PATH);
            String exeFilename      = String(buffer);
            String runningDirectory = exeFilename.substr(0, exeFilename.find_last_of("\\/"));
            return runningDirectory;
#else
            LINA_ERR("GetRunningDirectory() implementation missing for other platforms!");
#endif
            return "";
        }
        String ToLower(const String& input)
        {
            String data = input;

            std::for_each(data.begin(), data.end(), [](char& c) { c = ::tolower(c); });

            return data;
        }

        String ToUpper(const String& input)
        {
            String data = input;

            std::for_each(data.begin(), data.end(), [](char& c) { c = ::toupper(c); });

            return data;
        }
    } // namespace Utility
} // namespace Lina
