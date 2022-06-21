#include "Utility/UtilityFunctions.hpp"

#include "Core/PlatformMacros.hpp"
#include "Log/Log.hpp"
#include "EventSystem/ResourceEvents.hpp"
#include "EventSystem/EventSystem.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

#ifdef LINA_PLATFORM_WINDOWS
#include <iostream>
#include <stdlib.h>
#include <Data/String.hpp>
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

        bool FileExists(const String& path)
        {
            struct stat buffer;
            return (stat(path.c_str(), &buffer) == 0);
        }

        bool FolderContainsDirectory(Folder* root, const String& path, DirectoryItem*& outItem)
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

        void GetFolderHierarchToRoot(Folder* folder, Vector<Folder*>& hierarchy)
        {
            if (folder->m_parent != nullptr)
            {
                hierarchy.push_back(folder->m_parent);
                GetFolderHierarchToRoot(folder->m_parent, hierarchy);
            }
        }

        bool DeleteFileInPath(const String& path)
        {
            return remove(path.c_str());
        }

        String GetUniqueDirectoryName(Folder* parent, const String& prefix, const String& extension)
        {
            static int  uniqueNameCounter = 0;
            String newName           = prefix + "_" + TO_STRING(uniqueNameCounter).c_str();
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

            Folder* folder     = new Folder();
            folder->m_parent   = parent;
            folder->m_name     = newFolderName;
            folder->m_fullPath = parent->m_fullPath + "/" + folder->m_name;
            parent->m_folders.push_back(folder);

            CreateFolderInPath(folder->m_fullPath);
        }

        void DeleteFolder(Folder* folder)
        {
            for (auto* subfolder : folder->m_folders)
                DeleteFolder(subfolder);

            for (Vector<Folder*>::iterator it = folder->m_parent->m_folders.begin(); it < folder->m_parent->m_folders.end(); it++)
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

       
        bool CreateFolderInPath(const String& path)
        {
            bool success = std::filesystem::create_directory(path.c_str());

            if (!success)
                LINA_ERR("Could not create directory. {0}", path);

            return success;
        }

        bool DeleteDirectory(const String& path)
        {
            bool success = std::filesystem::remove_all(path.c_str());

            if (!success)
                LINA_ERR("Could not delete directory. {0}", path);

            return success;
        }

        void ParentPathUpdated(Folder* folder)
        {
            const String oldPath = folder->m_fullPath;
            folder->m_fullPath        = folder->m_parent->m_fullPath + "/" + folder->m_name;

            for (auto* subfolder : folder->m_folders)
                ParentPathUpdated(subfolder);

            for (auto* file : folder->m_files)
                ParentPathUpdated(file);
        }

        void ParentPathUpdated(File* file)
        {
            const StringIDType sidBefore = StringID(file->m_fullPath.c_str()).value();
            const String oldPath = file->m_fullPath;
            file->m_fullPath             = file->m_parent->m_fullPath + "/" + file->m_fullName;
            const StringIDType sidNow    = StringID(file->m_fullPath.c_str()).value();
            file->m_sid                  = sidNow;
            Event::EventSystem::Get()->Trigger<Event::EResourcePathUpdated>(Event::EResourcePathUpdated{sidBefore, sidNow, oldPath, file->m_fullPath});
        }

        void ChangeFileName(File* file, const String& newName)
        {
            const String oldPath = file->m_fullPath;
            file->m_name              = newName;
            file->m_fullName          = newName + "." + file->m_extension;
            ParentPathUpdated(file);
            ChangeDirectoryName(oldPath, file->m_fullPath);
        }

        void ChangeFolderName(Folder* folder, const String& newName)
        {
            const String oldPath = folder->m_fullPath;
            folder->m_fullPath        = folder->m_parent->m_fullPath + "/" + newName;
            folder->m_name            = newName;

            for (auto* subfolder : folder->m_folders)
                ParentPathUpdated(subfolder);

            for (auto* file : folder->m_files)
                ParentPathUpdated(file);

            ChangeDirectoryName(oldPath, folder->m_fullPath);
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
            newFile.open(file->m_fullPath.c_str(), std::ofstream::out | std::ofstream::trunc);
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
            const String fileLower       = Utility::ToLower(file.m_fullName);
            return fileLower.find(filterLowercase) != String::npos;
        }

        bool FolderContainsFilter(const Folder* folder, const String& filter)
        {
            const String filterLowercase = Utility::ToLower(filter);
            const String folderLower     = Utility::ToLower(folder->m_name);
            const bool        folderContains  = folderLower.find(filterLowercase) != String::npos;
            return folderContains;
        }

        bool SubfoldersContainFilter(const Folder* folder, const String& filter)
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

        File* FindFile(Folder* root, const String& path)
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

        String GetFileContents(const String& filePath)
        {
            std::ifstream ifs(filePath.c_str());
            auto a = std::istreambuf_iterator<char>(ifs);
            auto b = (std::istreambuf_iterator<char>());
            std::string   content(a,b);
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
