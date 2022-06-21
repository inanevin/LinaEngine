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

#include "Utility/ResourceUtility.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "Core/ResourceStorage.hpp"
#include "EventSystem/EventSystem.hpp"
#include "EventSystem/ResourceEvents.hpp"
#include "Core/CommonUtility.hpp"

namespace Lina::Resources
{
    ResourceProgressData ResourceUtility::s_currentProgressData;
    Utility::Folder*     ResourceUtility::s_rootFolder               = nullptr;
    String               ResourceUtility::s_workingDirectory         = "";
    String               ResourceUtility::s_workingDirectoryReplaced = "";

    void ResourceUtility::ScanFolder(Utility::Folder* root, bool recursive, int* totalFiles, bool isRescan)
    {
        for (const auto& entry : std::filesystem::directory_iterator(root->m_fullPath.c_str()))
        {
            String replacedFullPath = entry.path().string().c_str();
            std::replace(replacedFullPath.begin(), replacedFullPath.end(), '\\', '/');

            if (isRescan)
            {
                Utility::DirectoryItem* outItem = nullptr;
                // If the scanned folder already contains the target path and if the path is a subfolder
                // continue, if not, check the file's write timestamp & perform a reload if necessary.
                if (Utility::FolderContainsDirectory(root, replacedFullPath, outItem))
                {
                    if (entry.path().has_extension())
                    {
                        // Check for reload.
                        auto lastTime = std::filesystem::last_write_time(replacedFullPath.c_str());

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
                            ScanFolder(static_cast<Utility::Folder*>(outItem), recursive, totalFiles);
                        continue;
                    }
                }
                else
                {
                    const StringIDType sid = StringID(replacedFullPath.c_str()).value();
                    const TypeID       tid = Resources::ResourceStorage::Get()->GetTypeIDFromExtension(Utility::GetFileExtension(replacedFullPath));
                    Event::EventSystem::Get()->Trigger<Event::ERequestResourceReload>(Event::ERequestResourceReload{replacedFullPath, tid, sid});
                }
            }

            if (entry.path().has_extension())
            {
                Utility::File* file = new Utility::File();
                root->m_files.push_back(file);

                file->m_fullName       = entry.path().filename().string().c_str();
                file->m_folderPath     = (entry.path().parent_path().string() + "/").c_str();
                file->m_fullPath       = replacedFullPath;
                file->m_extension      = file->m_fullName.substr(file->m_fullName.find(".") + 1);
                file->m_name           = Utility::GetFileWithoutExtension(file->m_fullName);
                file->m_parent         = root;
                file->m_typeID         = Resources::ResourceStorage::Get()->GetTypeIDFromExtension(file->m_extension);
                file->m_lastWriteTime  = std::filesystem::last_write_time(file->m_fullPath.c_str());
                const StringIDType sid = StringID(file->m_fullPath.c_str()).value();
                file->m_sid            = sid;

                if (totalFiles != nullptr)
                    (*totalFiles) = (*totalFiles) + 1;
            }
            else
            {
                Utility::Folder* folder = new Utility::Folder();
                root->m_folders.push_back(folder);
                folder->m_name          = entry.path().filename().string().c_str();
                folder->m_fullPath      = replacedFullPath;
                folder->m_parent        = root;
                folder->m_typeID        = 0;
                folder->m_lastWriteTime = std::filesystem::last_write_time(folder->m_fullPath.c_str());
                const StringIDType sid  = StringID(folder->m_fullPath.c_str()).value();
                folder->m_sid           = sid;

                if (recursive)
                    ScanFolder(folder, recursive, totalFiles);
            }
        }
    }

    void ResourceUtility::DeleteResourceFile(Utility::File* file)
    {
        if (file->m_typeID != -1)
            Resources::ResourceStorage::Get()->Unload(file->m_typeID, file->m_fullPath);

        for (Vector<Utility::File*>::iterator it = file->m_parent->m_files.begin(); it < file->m_parent->m_files.end(); it++)
        {
            if (*it == file)
            {
                file->m_parent->m_files.erase(it);
                break;
            }
        }

        Utility::DeleteFileInPath(file->m_fullPath);
        delete file;
        file = nullptr;
    }

    String ResourceUtility::SearchFolderForSID(Utility::Folder* folder, StringIDType sid)
    {
        String foundFile = "";
        bool   found     = false;
        for (Utility::File* file : folder->m_files)
        {
            if (file->m_sid == sid)
            {
                foundFile = file->m_fullPath;
                found     = true;
            }
        }

        if (found)
            return foundFile;

        for (Utility::Folder* sub : folder->m_folders)
        {
            String found = SearchFolderForSID(sub, sid);

            if (found.compare("") != 0)
                return found;
        }

        return "";
    }

    void ResourceUtility::ScanRootFolder()
    {
        if (s_rootFolder != nullptr)
            delete s_rootFolder;

        s_rootFolder             = new Utility::Folder();
        s_rootFolder->m_fullPath = "Resources/";
        s_rootFolder->m_name     = "Resources";
        ResourceUtility::ScanFolder(s_rootFolder, true, &s_currentProgressData.m_currentTotalFiles);
    }

} // namespace Lina::Resources
