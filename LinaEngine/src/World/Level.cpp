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

#include "World/Level.hpp"
#include "Core/Engine.hpp"
#include "ECS/Registry.hpp"
#include "EventSystem/LevelEvents.hpp"
#include <cereal/archives/portable_binary.hpp>
#include <fstream>
#include <stdio.h>

namespace Lina::World
{
    void Level::Install()
    {
        ECS::Registry::s_ecs = &m_registry;
        LINA_TRACE("Level installed: {0}", m_path);
    }

    Level::Level(const Level& lvl)
    {
        m_ambientColor  = lvl.m_ambientColor;
        m_usedResources = lvl.m_usedResources;
    }

    void* Level::LoadFromMemory(const String& path, unsigned char* data, size_t dataSize)
    {
        std::string        dataStr((char*)data, dataSize);
        std::istringstream stream(dataStr, std::ios::binary);
        {
            cereal::PortableBinaryInputArchive iarchive(stream);
            iarchive(*this);
            m_registry.DeserializeComponentsInRegistry(iarchive);
        }
        IResource::SetSID(path);
        return static_cast<void*>(this);
    }

    void* Level::LoadFromFile(const String& path)
    {
        std::ifstream stream(path.c_str(), std::ios::binary);
        {
            cereal::PortableBinaryInputArchive iarchive(stream);
            iarchive(*this);
            m_registry.DeserializeComponentsInRegistry(iarchive);
        }
        IResource::SetSID(path);
        return static_cast<void*>(this);
    }

    void Level::Uninstall()
    {
        m_registry.clear();
        LINA_TRACE("Level uninstalled: {0}", m_path);
    }

    void Level::SaveToFile(const String& path)
    {
        if (Utility::FileExists(path))
            Utility::DeleteFileInPath(path);

        std::ofstream stream(path.c_str(), std::ios::binary);
        {
            cereal::PortableBinaryOutputArchive oarchive(stream);
            oarchive(*this);
            m_registry.SerializeComponentsInRegistry(oarchive);
        }
    }

    void Level::AddResourceReference(TypeID tid, StringIDType sid)
    {
        m_usedResources[tid].insert(sid);
    }

} // namespace Lina::World
