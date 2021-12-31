/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

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

/*
Class: ResourceBundle

Represents a bundle, containing various packages for resource categories. Raw packages contain RAW data loaded from .linapackage files.

Timestamp: 12/22/2020 12:26:55 AM
*/

#pragma once

#ifndef ResourceBundle_HPP
#define ResourceBundle_HPP

// Headers here.
#include "Core/CommonResources.hpp"
#include "Core/CommonUtility.hpp"

#include <queue>

namespace Lina
{
    namespace Event
    {
        class EventSystem;
    }
} // namespace Lina

namespace Lina::Resources
{
    class MemoryEntry
    {
    public:
        MemoryEntry() = default;
        ~MemoryEntry();

        MemoryEntry(int priority, ResourceType type, const std::string& path, std::vector<unsigned char>& data, std::string& paramsExtension)
        {
            m_priority        = priority;
            m_type            = type;
            m_path            = path;
            m_data            = data;
            m_paramsExtension = paramsExtension;
        }

        ResourceType               m_type            = ResourceType::Unknown;
        int                        m_priority        = 100;
        std::string                m_path            = "";
        std::string                m_paramsExtension = "";
        std::vector<unsigned char> m_data;
    };

    struct CompareMemEntry
    {
        bool operator()(MemoryEntry const& p1, MemoryEntry const& p2)
        {
            // return "true" if "p1" is ordered
            // before "p2", for example:
            return p1.m_priority < p2.m_priority;
        }
    };

    class FileEntry
    {
    public:
        FileEntry()  = default;
        ~FileEntry() = default;

        FileEntry(int priority, Utility::File* file)
        {
            m_priority = priority;
            m_file     = file;
        }

        Utility::File* m_file     = nullptr;
        int            m_priority = 100;
    };

    struct CompareFileEntry
    {
        bool operator()(FileEntry const& p1, FileEntry const& p2)
        {
            // return "true" if "p1" is ordered
            // before "p2", for example:
            return p1.m_priority < p2.m_priority;
        }
    };

    class ResourceBundle
    {

    private:
        friend class ResourceManager;
        friend class Packager;

        // Pushes the given memory buffer to a resource map.
        void PushResourceFromMemory(const std::string& path, ResourceType type, std::vector<unsigned char>& data);

        // Loads all memory buffers stored in maps.
        void LoadAllMemoryMaps();

        // Calls LoadResourceFromFile on all resources within the folder.
        void LoadResourcesInFolder(Utility::Folder* root, const std::vector<ResourceType>& excludes, ResourceType onlyLoad = ResourceType::Unknown);

        void ScanFileResources(Utility::Folder* folder);
        void LoadAllFileResources();

        // Loads given resource from a file path into memory.
        void LoadResourceFromFile(Utility::File* file, ResourceType type);

        std::priority_queue<MemoryEntry, std::vector<MemoryEntry>, CompareMemEntry> m_memoryResources;
        std::vector<MemoryEntry>                                                    m_memoryResourcesParams;

        std::priority_queue<FileEntry, std::vector<FileEntry>, CompareFileEntry> m_fileResources;
    };
} // namespace Lina::Resources

#endif
