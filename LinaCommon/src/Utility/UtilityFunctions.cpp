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
#include "Resources/ResourceStorage.hpp"
#include "EventSystem/ResourceEvents.hpp"
#include "EventSystem/EventSystem.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

#ifdef LINA_PLATFORM_WINDOWS
#include <iostream>
#include <stdlib.h>
#include <string>
#include <windows.h>
double g_timerFrequency;
bool   g_isTimerInitialized = false;
#endif

namespace Lina
{
    namespace Utility
    {
        double GetCPUTime()
        {
#ifdef LINA_WINDOWS
            if (!g_isTimerInitialized)
            {
                LARGE_INTEGER li;
                if (!QueryPerformanceFrequency(&li))
                    LINA_ERR("Initialization -> QueryPerformanceFrequency failed!");

                g_timerFrequency     = double(li.QuadPart);
                g_isTimerInitialized = true;
            }

            LARGE_INTEGER li;
            if (!QueryPerformanceCounter(&li))
                LINA_ERR("GetTime -> QueryPerformanceCounter failed in get time!");

            return double(li.QuadPart) / g_timerFrequency;
#endif

#ifdef LINA_LINUX
            timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts);
            return (double)(((long)ts.tv_sec * NANOSECONDS_PER_SECOND) + ts.tv_nsec) / ((double)(NANOSECONDS_PER_SECOND));
#endif

#ifdef LINA_UNKNOWN_PLATFORM
            LINA_ERR("Uknown platform!");
            return 0.0f;
#endif
        }

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

        bool FileExists(const std::string& path)
        {
            struct stat buffer;
            return (stat(path.c_str(), &buffer) == 0);
        }

        void ScanFolder(Folder* root, bool recursive, int* totalFiles, bool isRescan)
        {
            for (const auto& entry : std::filesystem::directory_iterator(root->m_fullPath))
            {
                std::string replacedFullPath = entry.path().string();
                std::replace(replacedFullPath.begin(), replacedFullPath.end(), '\\', '/');

                if (isRescan)
                {
                    DirectoryItem* outItem = nullptr;
                    // If the scanned folder already contains the target path and if the path is a subfolder
                    // continue, if not, check the file's write timestamp & perform a reload if necessary.
                    if (FolderContainsDirectory(root, replacedFullPath, outItem))
                    {
                        if (entry.path().has_extension())
                        {
                            // Check for reload.
                            auto lastTime = std::filesystem::last_write_time(replacedFullPath);

                            if (outItem != nullptr && lastTime != outItem->m_lastWriteTime)
                            {
                                const StringIDType sid = StringID(replacedFullPath.c_str()).value();
                                const TypeID       tid = outItem->m_typeID;
                                Resources::ResourceStorage::Get()->Unload(tid, sid);
                                Event::EventSystem::Get()->Trigger<Event::ERequestResourceReload>(Event::ERequestResourceReload{replacedFullPath, tid, sid});
                            }
                            continue;
                        }
                        else
                        {
                            if (recursive)
                                ScanFolder(static_cast<Folder*>(outItem), recursive, totalFiles);
                            continue;
                        }
                    }
                    else
                    {
                        const StringIDType sid = StringID(replacedFullPath.c_str()).value();
                        const TypeID       tid = Resources::ResourceStorage::Get()->GetTypeIDFromExtension(GetFileExtension(replacedFullPath));
                        Event::EventSystem::Get()->Trigger<Event::ERequestResourceReload>(Event::ERequestResourceReload{replacedFullPath, tid, sid});
                    }
                }

                if (entry.path().has_extension())
                {
                    File* file = new File();
                    root->m_files.push_back(file);

                    file->m_fullName       = entry.path().filename().string();
                    file->m_folderPath     = entry.path().parent_path().string() + "/";
                    file->m_fullPath       = replacedFullPath;
                    file->m_extension      = file->m_fullName.substr(file->m_fullName.find(".") + 1);
                    file->m_name           = GetFileWithoutExtension(file->m_fullName);
                    file->m_parent         = root;
                    file->m_typeID         = Resources::ResourceStorage::Get()->GetTypeIDFromExtension(file->m_extension);
                    file->m_lastWriteTime  = std::filesystem::last_write_time(file->m_fullPath);
                    const StringIDType sid = StringID(file->m_fullPath.c_str()).value();
                    file->m_sid            = sid;

                    if (totalFiles != nullptr)
                        (*totalFiles) = (*totalFiles) + 1;
                }
                else
                {
                    Folder* folder = new Folder();
                    root->m_folders.push_back(folder);
                    folder->m_name          = entry.path().filename().string();
                    folder->m_fullPath      = replacedFullPath;
                    folder->m_parent        = root;
                    folder->m_typeID        = 0;
                    folder->m_lastWriteTime = std::filesystem::last_write_time(folder->m_fullPath);
                    const StringIDType sid  = StringID(folder->m_fullPath.c_str()).value();
                    folder->m_sid           = sid;

                    if (recursive)
                        ScanFolder(folder, recursive, totalFiles);
                }
            }
        }

        bool FolderContainsDirectory(Folder* root, const std::string& path, DirectoryItem*& outItem)
        {
            bool contains = false;

            for (auto* folder : root->m_folders)
            {
                if (folder->m_fullPath.compare(path) == 0)
                {
                    contains = true;
                    outItem  = folder;
                    break;
                }
            }

            if (!contains)
            {
                for (auto* file : root->m_files)
                {
                    if (file->m_fullPath.compare(path) == 0)
                    {
                        contains = true;
                        outItem  = file;
                        break;
                    }
                }
            }

            return contains;
        }

        void GetFolderHierarchToRoot(Folder* folder, std::vector<Folder*>& hierarchy)
        {
            if (folder->m_parent != nullptr)
            {
                hierarchy.push_back(folder->m_parent);
                GetFolderHierarchToRoot(folder->m_parent, hierarchy);
            }
        }

        bool DeleteFileInPath(const std::string& path)
        {
            return remove(path.c_str());
        }

        std::string GetUniqueDirectoryName(Folder* parent, const std::string& prefix, const std::string& extension)
        {
            static int  uniqueNameCounter = 0;
            std::string newName           = prefix + "_" + std::to_string(uniqueNameCounter);
            bool        newNameExists     = true;
            int         timeOutCounter    = 0;

            // Loop through the sub-folders and make sure no folder with the same name exists.
            while (newNameExists && timeOutCounter < 1000)
            {
                bool found = false;

                // Check sub-folders
                for (auto* folder : parent->m_folders)
                {
                    if (folder->m_name.compare(newName) == 0)
                    {
                        found = true;
                        break;
                    }
                }

                // Check files.
                if (!found)
                {
                    for (auto* file : parent->m_files)
                    {
                        if (file->m_fullName.compare(newName + extension) == 0)
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
                    newName = prefix + "_" + std::to_string(uniqueNameCounter);
                }

                timeOutCounter++;
            }

            LINA_ASSERT(!newNameExists, "Could not get a unique name for the file/folder!");

            return newName;
        }

        void CreateNewSubfolder(Folder* parent)
        {
            const std::string newFolderName = GetUniqueDirectoryName(parent, "NewFolder", "");

            Folder* folder     = new Folder();
            folder->m_parent   = parent;
            folder->m_name     = newFolderName;
            folder->m_fullPath = parent->m_fullPath + "/" + folder->m_name;
            parent->m_folders.push_back(folder);

            CreateFolderInPath(folder->m_fullPath);
        }

        void DeleteFolder(Folder* folder)
        {
            for (auto* file : folder->m_files)
                DeleteResourceFile(file);

            for (auto* subfolder : folder->m_folders)
                DeleteFolder(subfolder);

            for (std::vector<Folder*>::iterator it = folder->m_parent->m_folders.begin(); it < folder->m_parent->m_folders.end(); it++)
            {
                if (*it == folder)
                {
                    folder->m_parent->m_folders.erase(it);
                    break;
                }
            }

            DeleteDirectory(folder->m_fullPath);
            delete folder;
            folder = nullptr;
        }

        void DeleteResourceFile(File* file)
        {
            if (file->m_typeID != -1)
                Resources::ResourceStorage::Get()->Unload(file->m_typeID, file->m_fullPath);

            for (std::vector<File*>::iterator it = file->m_parent->m_files.begin(); it < file->m_parent->m_files.end(); it++)
            {
                if (*it == file)
                {
                    file->m_parent->m_files.erase(it);
                    break;
                }
            }

            DeleteFileInPath(file->m_fullPath);
            delete file;
            file = nullptr;
        }

        bool CreateFolderInPath(const std::string& path)
        {
            bool success = std::filesystem::create_directory(path);

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

        void ParentPathUpdated(Folder* folder)
        {
            const std::string oldPath = folder->m_fullPath;
            folder->m_fullPath        = folder->m_parent->m_fullPath + "/" + folder->m_name;

            for (auto* subfolder : folder->m_folders)
                ParentPathUpdated(subfolder);

            for (auto* file : folder->m_files)
                ParentPathUpdated(file);
        }

        void ParentPathUpdated(File* file)
        {
            const StringIDType sidBefore = StringID(file->m_fullPath.c_str()).value();
            const std::string oldPath = file->m_fullPath;
            file->m_fullPath             = file->m_parent->m_fullPath + "/" + file->m_fullName;
            const StringIDType sidNow    = StringID(file->m_fullPath.c_str()).value();
            file->m_sid                  = sidNow;
            Event::EventSystem::Get()->Trigger<Event::EResourcePathUpdated>(Event::EResourcePathUpdated{sidBefore, sidNow, oldPath, file->m_fullPath});
        }

        void ChangeFileName(File* file, const std::string& newName)
        {
            const std::string oldPath = file->m_fullPath;
            file->m_name              = newName;
            file->m_fullName          = newName + "." + file->m_extension;
            ParentPathUpdated(file);
            ChangeDirectoryName(oldPath, file->m_fullPath);
        }

        void ChangeFolderName(Folder* folder, const std::string& newName)
        {
            const std::string oldPath = folder->m_fullPath;
            folder->m_fullPath        = folder->m_parent->m_fullPath + "/" + newName;
            folder->m_name            = newName;

            for (auto* subfolder : folder->m_folders)
                ParentPathUpdated(subfolder);

            for (auto* file : folder->m_files)
                ParentPathUpdated(file);

            ChangeDirectoryName(oldPath, folder->m_fullPath);
        }

        bool ChangeDirectoryName(const std::string& oldPath, const std::string& newPath)
        {

            /*	Deletes the file if exists */
            if (std::rename(oldPath.c_str(), newPath.c_str()) != 0)
            {
                LINA_ERR("Failed to rename directory! Old Name: {1}, New Name: {2}", oldPath, newPath);
                return false;
            }

            return true;
        }

        void RewriteFileContents(File* file, const std::string& newContent)
        {
            std::ofstream newFile;
            newFile.open(file->m_fullPath, std::ofstream::out | std::ofstream::trunc);
            newFile << newContent << std::endl;
            newFile.close();
        }

        int GetUniqueID()
        {
            return ++s_uniqueID;
        }

        std::string GetUniqueIDString()
        {
            return std::to_string(++s_uniqueID);
        }

        size_t StringToHash(const std::string& str)
        {
            std::hash<std::string> hasher;
            return hasher(str);
        }

        std::vector<std::string> Split(const std::string& s, char delim)
        {
            std::vector<std::string> elems;

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

        std::string GetFilePath(const std::string& fileName)
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

        std::string GetFileExtension(const std::string& file)
        {
            return file.substr(file.find_last_of(".") + 1);
        }

        std::string GetFileNameOnly(const std::string& fileName)
        {
            return fileName.substr(fileName.find_last_of("/\\") + 1);
        }

        std::string GetFileWithoutExtension(const std::string& fileName)
        {
            size_t lastindex = fileName.find_last_of(".");
            return fileName.substr(0, lastindex);
        }

        bool FileContainsFilter(const File& file, const std::string& filter)
        {
            const std::string filterLowercase = Utility::ToLower(filter);
            const std::string fileLower       = Utility::ToLower(file.m_fullName);
            return fileLower.find(filterLowercase) != std::string::npos;
        }

        bool FolderContainsFilter(const Folder* folder, const std::string& filter)
        {
            const std::string filterLowercase = Utility::ToLower(filter);
            const std::string folderLower     = Utility::ToLower(folder->m_name);
            const bool        folderContains  = folderLower.find(filterLowercase) != std::string::npos;
            return folderContains;
        }

        bool SubfoldersContainFilter(const Folder* folder, const std::string& filter)
        {
            bool subFoldersContain = false;

            for (auto* folder : folder->m_folders)
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

        File* FindFile(Folder* root, const std::string& path)
        {
            for (auto* subfolder : root->m_folders)
            {
                File* f = FindFile(subfolder, path);

                if (f != nullptr)
                    return f;
            }

            for (auto* file : root->m_files)
            {
                if (file->m_fullPath.compare(path) == 0)
                    return file;
            }

            return nullptr;
        }

        std::string GetFileContents(const std::string& filePath)
        {
            std::ifstream ifs(filePath);
            std::string   content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
            return content;
        }

        std::string GetRunningDirectory()
        {
#ifdef LINA_PLATFORM_WINDOWS
            TCHAR buffer[MAX_PATH] = {0};
            GetModuleFileName(NULL, buffer, MAX_PATH);
            std::string exeFilename      = std::string(buffer);
            std::string runningDirectory = exeFilename.substr(0, exeFilename.find_last_of("\\/"));
            return runningDirectory;
#else
            LINA_ERR("GetRunningDirectory() implementation missing for other platforms!");
#endif
            return "";
        }
        std::string ToLower(const std::string& input)
        {
            std::string data = input;

            std::for_each(data.begin(), data.end(), [](char& c) { c = ::tolower(c); });

            return data;
        }

        std::string ToUpper(const std::string& input)
        {
            std::string data = input;

            std::for_each(data.begin(), data.end(), [](char& c) { c = ::toupper(c); });

            return data;
        }
    } // namespace Utility
} // namespace Lina
