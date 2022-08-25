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

#ifndef IResourceLoader_HPP
#define IResourceLoader_HPP

// Headers here.
#include "Core/CommonApplication.hpp"
#include "Utility/StringId.hpp"
#include "Core/ResourcePackager.hpp"
#include "Data/HashMap.hpp"
#include "Data/HashSet.hpp"
#include "Data/PriorityQueue.hpp"
#include "Data/Vector.hpp"
#include "JobSystem/JobSystem.hpp"
#include "Core/CommonMemory.hpp"

namespace Lina
{
    namespace Utility
    {
        struct Folder;
        struct File;
    } // namespace Utility
} // namespace Lina

namespace Lina::Resources
{
    class ResourceLoader
    {
    public:
        struct LoadingPair
        {
        public:
            bool operator()(LoadingPair p1, LoadingPair p2)
            {
                return p1.priority > p2.priority;
            }

            TypeID tid      = 0;
            String path     = "";
            uint32 priority = 0;
        };
        ResourceLoader()
        {
        }
        virtual ~ResourceLoader(){};

        virtual void LoadResource(TypeID tid, const String& path, bool loadAsync, Memory::ResourceAllocator alloc = Memory::ResourceAllocator::None) = 0;
        virtual void LoadLevelResources(const HashMap<TypeID, HashSet<String>>& resourceMap)                                                         = 0;
        virtual void LoadStaticResources()                                                                                                           = 0;
        virtual void LoadEngineResources()                                                                                                           = 0;

        inline ResourcePackager& GetPackager()
        {
            return m_packager;
        }

    protected:
        friend class ResourcePackager;

        /// <summary>
        /// Loads a single file resource.
        /// </summary>
        bool LoadSingleResourceFromFile(TypeID tid, const String& fullpath, Memory::ResourceAllocator alloc = Memory::ResourceAllocator::None);

        /// <summary>
        /// Loads a memory file resource.
        /// </summary>
        bool LoadSingleResourceFromMemory(const String& path, Vector<unsigned char>& data, Memory::ResourceAllocator alloc = Memory::ResourceAllocator::None);

        /// <summary>
        /// Checks the current level resources to load and unloads any active resource that doesn't belong in the map.
        /// </summary>
        /// <param name="currentLevelResources"></param>
        void UnloadUnusedResources(const HashMap<TypeID, HashSet<String>>& levelResources);

    protected:
        ResourcePackager m_packager;
        TypeID           m_lastResourceTypeID   = -1;
        int              m_lastResourcePriority = 0;
        DEFINE_MUTEX(m_mutex);
    };
} // namespace Lina::Resources

#endif
