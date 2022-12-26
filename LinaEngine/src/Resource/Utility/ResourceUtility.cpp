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

#include "Resource/Utility/ResourceUtility.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "Resource/Core/ResourceManager.hpp"
#include "EventSystem/EventSystem.hpp"
#include "EventSystem/ResourceEvents.hpp"
#include "Utility/UtilityCommon.hpp"

namespace Lina::Resources
{
    Utility::Folder* ResourceUtility::s_rootFolder               = nullptr;
    String           ResourceUtility::s_workingDirectory         = "";
    String           ResourceUtility::s_workingDirectoryReplaced = "";

    void ResourceUtility::ScanFolder(Utility::Folder* root, bool recursive, int* totalFiles, bool isRescan)
    {
        for (const auto& entry : std::filesystem::directory_iterator(root->fullPath.c_str()))
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

                        if (outItem != nullptr && lastTime != outItem->lastWriteTime)
                        {
                            const StringID sid = TO_SID(replacedFullPath);
                            const TypeID   tid = outItem->typeID;
                            Resources::ResourceManager::Get()->Unload(tid, sid);
                            Event::EventSystem::Get()->Trigger<Event::ERequestResourceReload>(Event::ERequestResourceReload{replacedFullPath.c_str(), tid, sid});
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
                    const StringID sid = TO_SID(replacedFullPath);
                    const TypeID   tid = Resources::ResourceManager::Get()->GetTypeIDFromExtension(Utility::GetFileExtension(replacedFullPath));
                    Event::EventSystem::Get()->Trigger<Event::ERequestResourceReload>(Event::ERequestResourceReload{replacedFullPath.c_str(), tid, sid});
                }
            }

            if (entry.path().has_extension())
            {
                Utility::File* file = new Utility::File();
                root->files.push_back(file);

                file->fullName      = entry.path().filename().string().c_str();
                file->folderPath    = (entry.path().parent_path().string() + "/").c_str();
                file->fullPath      = replacedFullPath;
                file->extension     = file->fullName.substr(file->fullName.find(".") + 1);
                file->name          = Utility::GetFileWithoutExtension(file->fullName);
                file->parent        = root;
                file->typeID        = Resources::ResourceManager::Get()->GetTypeIDFromExtension(file->extension);
                file->lastWriteTime = std::filesystem::last_write_time(file->fullPath.c_str());
                const StringID sid  = TO_SID(file->fullPath);
                file->sid           = sid;

                if (totalFiles != nullptr)
                    (*totalFiles) = (*totalFiles) + 1;
            }
            else
            {
                Utility::Folder* folder = new Utility::Folder();
                root->folders.push_back(folder);
                folder->name          = entry.path().filename().string().c_str();
                folder->fullPath      = replacedFullPath;
                folder->parent        = root;
                folder->typeID        = 0;
                folder->lastWriteTime = std::filesystem::last_write_time(folder->fullPath.c_str());
                const StringID sid    = TO_SID(folder->fullPath);
                folder->sid           = sid;

                if (recursive)
                    ScanFolder(folder, recursive, totalFiles);
            }
        }
    }

    void ResourceUtility::DeleteResourceFile(Utility::File* file)
    {
        if (file->typeID != -1)
            Resources::ResourceManager::Get()->Unload(file->typeID, file->fullPath);

        for (Vector<Utility::File*>::iterator it = file->parent->files.begin(); it < file->parent->files.end(); it++)
        {
            if (*it == file)
            {
                file->parent->files.erase(it);
                break;
            }
        }

        Utility::DeleteFileInPath(file->fullPath);
        delete file;
        file = nullptr;
    }

    String ResourceUtility::SearchFolderForSID(Utility::Folder* folder, StringID sid)
    {
        String foundFile = "";
        bool   found     = false;
        for (Utility::File* file : folder->files)
        {
            if (file->sid == sid)
            {
                foundFile = file->fullPath;
                found     = true;
            }
        }

        if (found)
            return foundFile;

        for (Utility::Folder* sub : folder->folders)
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

        s_rootFolder           = new Utility::Folder();
        s_rootFolder->fullPath = "Resources/";
        s_rootFolder->name     = "Resources";
        int totalFiles         = 0;
        ResourceUtility::ScanFolder(s_rootFolder, true, &totalFiles);
    }

    void ResourceUtility::InitializeWorkingDirectory()
    {
        s_workingDirectory         = std::filesystem::current_path().string().c_str();
        s_workingDirectoryReplaced = s_workingDirectory;
        std::replace(s_workingDirectoryReplaced.begin(), s_workingDirectoryReplaced.end(), '\\', '/');
    }

    String ResourceUtility::PackageTypeToString(PackageType type)
    {
        switch (type)
        {
        case PackageType::Static:
            return "static";
        case PackageType::Custom:
            return "custom";
        case PackageType::Audio:
            return "audio";
        case PackageType::Level:
            return "levels";
        case PackageType::Graphics:
            return "graphics";
        case PackageType::Models:
            return "meshes";
        case PackageType::Physics:
            return "physics";
        case PackageType::Textures:
            return "textures";
        default:
            return "";
        }
    }
} // namespace Lina::Resources
